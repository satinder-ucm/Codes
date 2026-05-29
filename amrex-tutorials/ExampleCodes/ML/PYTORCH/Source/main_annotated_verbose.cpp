#include <torch/script.h> // TorchScript API: load a serialized model and run inference.

#include <AMReX_PlotFileUtil.H> // AMReX utility for writing plotfiles.
#include <AMReX_ParmParse.H> // AMReX utility for reading runtime input parameters.

#include "myfunc.H" // Declares main_main().

using namespace amrex; // Lets us refer to AMReX symbols without the amrex:: prefix.

int main (int argc, char* argv[])
{
    // argc = number of command-line arguments.
    // argv = array of command-line argument strings.
    // AMReX uses these to parse its own runtime options.
    amrex::Initialize(argc,argv);

    // The real work happens in main_main().
    main_main();

    // Clean shutdown of AMReX and MPI.
    amrex::Finalize();
    return 0;
}

void main_main ()
{
    // Start an AMReX profiling timer for the initialization section.
    BL_PROFILE_VAR("Init",Init);

    // Wall-clock time at the beginning of the program.
    Real strt_time = ParallelDescriptor::second();

    // Accumulator for time spent only inside Torch model inference.
    Real eval_time = 0.0;

    // **********************************
    // SIMULATION PARAMETERS

    // Number of cells per direction in the domain.
    int n_cell;

    // Maximum number of cells in each AMReX box edge.
    int max_grid_size;

    // Name of the TorchScript model file to load.
    std::string model_filename;

    // Read values from Exec/inputs.
    {
        // ParmParse reads key/value pairs from the AMReX inputs file.
        ParmParse pp;

        // Required parameter. Sets the domain size.
        pp.get("n_cell", n_cell);

        // Required parameter. Controls AMReX box splitting.
        pp.get("max_grid_size", max_grid_size);

        // Optional parameter. Path to the serialized model file.
        pp.query("model_file", model_filename);
    }

    // **********************************
    // SIMULATION SETUP

    // BoxArray stores the decomposition of the domain into boxes.
    BoxArray ba;

    // Geometry stores physical domain information.
    Geometry geom;

    // Integer lower bound of the domain.
    IntVect dom_lo(AMREX_D_DECL(0, 0, 0));

    // Integer upper bound of the domain.
    IntVect dom_hi(AMREX_D_DECL(n_cell - 1, n_cell - 1, n_cell - 1));

    // Create the full domain box from the bounds above.
    Box domain(dom_lo, dom_hi);

    // Put the full domain into the BoxArray.
    ba.define(domain);

    // Split the domain into boxes no larger than max_grid_size.
    ba.maxSize(max_grid_size);

    // Define the physical domain as a unit interval in each direction.
    RealBox real_box({AMREX_D_DECL(0., 0., 0.)},
                     {AMREX_D_DECL(1., 1., 1.)});

    // Non-periodic in all directions.
    Array<int, AMREX_SPACEDIM> is_periodic{AMREX_D_DECL(0, 0, 0)};

    // Combine integer domain, physical domain, coordinate system, and periodicity.
    geom.define(domain, real_box, CoordSys::cartesian, is_periodic);

    // Physical size of one cell in each direction.
    GpuArray<Real, AMREX_SPACEDIM> dx = geom.CellSizeArray();

    // No ghost cells in this example.
    int Nghost = 0;

    // One input feature per cell.
    int Nc_in = 1;

    // Two output values per cell.
    int Nc_out = 2;

    // Assign boxes to MPI ranks.
    DistributionMapping dm(ba);

    // Input field. Each cell has Nc_in components.
    MultiFab phi_in(ba, dm, Nc_in, Nghost);

    // Output field. Each cell has Nc_out components.
    MultiFab phi_out(ba, dm, Nc_out, Nghost);

    // **********************************
    // INITIALIZE DATA

    // End the Init profiling section.
    BL_PROFILE_VAR_STOP(Init);

    // Start timing the initialization of the input field.
    BL_PROFILE_VAR("InitData", InitData);

    // Loop over every box in phi_in.
    for (MFIter mfi(phi_in); mfi.isValid(); ++mfi)
    {
        // Valid region of the current box.
        const Box& bx = mfi.validbox();

        // Direct access to the cell data for this box.
        const Array4<Real>& phi_input = phi_in.array(mfi);

        // Fill the input field with a simple deterministic test pattern.
        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k) noexcept
        {
            // Expression uses integer cell indices and normalizes by n_cell - 1.
            phi_input(i, j, k) = (i + j + k) / (n_cell - 1.0);
        });
    }

    // End timing the input initialization.
    BL_PROFILE_VAR_STOP(InitData);

    // Start timing the plotfile write for the input field.
    BL_PROFILE_VAR("WriteInitPlot", WriteInitPlot);

    // Write phi_in to disk so it can be visualized later.
    const std::string& pltfile = "plt_inputs";
    WriteSingleLevelPlotfile(pltfile, phi_in, {"dt"}, geom, 0.0, 0);

    // End timing the input plotfile write.
    BL_PROFILE_VAR_STOP(WriteInitPlot);

    // **********************************
    // LOAD PYTORCH MODEL

    // Start timing model loading.
    BL_PROFILE_VAR("LoadPytorch", LoadPytorch);

    // TorchScript module object that will own the loaded network.
    torch::jit::script::Module module;

    try {
        // Read the serialized model from model_filename.
        module = torch::jit::load(model_filename);
    }
    catch (const c10::Error& e) {
        // Stop the program if loading fails.
        amrex::Abort("Error loading the model\n");
    }

    // Print a simple confirmation.
    Print() << "Model loaded.\n";

    // Use double precision for tensor data.
    auto dtype0 = torch::kFloat64;

#ifdef AMREX_USE_CUDA
    // CUDA device handle.
    torch::Device device0(torch::kCUDA);

    // Move the model parameters to GPU memory.
    module.to(device0);

    // Print a message so you know the model is on the GPU.
    amrex::Print() << "Copying model to GPU." << std::endl;

    // Tensor options for CUDA tensors with double precision.
    auto tensoropt = torch::TensorOptions().dtype(dtype0).device(device0);
#else
    // Tensor options for CPU tensors with double precision.
    auto tensoropt = torch::TensorOptions().dtype(dtype0);
#endif

    // End timing model loading.
    BL_PROFILE_VAR_STOP(LoadPytorch);

    // **********************************
    // EVALUATE MODEL

    // Start timing inference.
    BL_PROFILE_VAR("Eval", Eval);

    // Loop over each AMReX box again, this time to run inference.
    for (MFIter mfi(phi_in); mfi.isValid(); ++mfi)
    {
        // Current box region.
        const Box& bx = mfi.validbox();

        // Read-only access to input field data.
        const Array4<Real>& phi_input = phi_in.array(mfi);

        // Writable access to output field data.
        const Array4<Real>& phi_output = phi_out.array(mfi);

        // Lower index corner of the current box.
        const IntVect bx_lo = bx.smallEnd();

        // Size of the current box in cells along each direction.
        const IntVect nbox = bx.size();

        // Total number of cells in the current box.
        int ncell = AMREX_SPACEDIM == 2 ?
            nbox[0] * nbox[1] : nbox[0] * nbox[1] * nbox[2];

        // ManagedVector allocates contiguous memory visible to CPU/GPU.
        amrex::Gpu::ManagedVector<Real> aux(ncell * Nc_in);

        // Raw pointer to the first element of the contiguous buffer.
        Real* AMREX_RESTRICT auxPtr = aux.dataPtr();

        // Copy the AMReX box data into a flat [cell][component] buffer.
        amrex::ParallelFor(bx, Nc_in, [=] AMREX_GPU_DEVICE(int i, int j, int k, int n) noexcept
        {
            // Convert global cell coordinates to box-local coordinates.
            int ii = i - bx_lo[0];
            int jj = j - bx_lo[1];

            // Flatten the 2D coordinates into a single linear cell index.
            int index = jj * nbox[0] + ii;

#if AMREX_SPACEDIM == 3
            // In 3D, add the slab offset for the z dimension.
            int kk = k - bx_lo[2];
            index += kk * nbox[0] * nbox[1];
#endif

            // Write the input value into the flat buffer.
            auxPtr[index * Nc_in + n] = phi_input(i, j, k, n);
        });

        // Create a Torch tensor that views the contiguous buffer.
        at::Tensor inputs_torch = torch::from_blob(auxPtr, {ncell, Nc_in}, tensoropt);

        // Capture the start time for just the forward pass.
        Real eval_t_start = ParallelDescriptor::second();

        // Run the TorchScript model.
        at::Tensor outputs_torch = module.forward({inputs_torch}).toTensor();

        // Force the output tensor to use the same precision as AMReX Real.
        outputs_torch = outputs_torch.to(dtype0);

        // Add the elapsed inference time for this box.
        eval_time += ParallelDescriptor::second() - eval_t_start;

        // Build a read-only tensor accessor for fast indexed access.
#ifdef AMREX_USE_CUDA
        auto outputs_torch_acc = outputs_torch.packed_accessor64<Real, 2>();
#else
        auto outputs_torch_acc = outputs_torch.accessor<Real, 2>();
#endif

        // Copy predictions from the tensor into the AMReX output MultiFab.
        amrex::ParallelFor(bx, Nc_out, [=] AMREX_GPU_DEVICE(int i, int j, int k, int n) noexcept
        {
            // Convert global cell coordinates to box-local coordinates.
            int ii = i - bx_lo[0];
            int jj = j - bx_lo[1];

            // Flatten coordinates into one linear index.
            int index = jj * nbox[0] + ii;

#if AMREX_SPACEDIM == 3
            // Add the 3D slab offset if needed.
            int kk = k - bx_lo[2];
            index += kk * nbox[0] * nbox[1];
#endif

            // Store the n-th output component for this cell.
            phi_output(i, j, k, n) = outputs_torch_acc[index][n];
        });
    }

    // End timing inference.
    BL_PROFILE_VAR_STOP(Eval);

    // Start the final reporting section.
    BL_PROFILE_VAR("Post", Post);

    // Print a message when inference is finished.
    amrex::Print() << "Finish evaluating model.\n";

    // Write the predicted output field to disk.
    const std::string& pltfile2 = "plt_outputs";
    WriteSingleLevelPlotfile(pltfile2, phi_out, {"X_0", "X_1"}, geom, 1.0, 1);

    // Total runtime since the beginning.
    Real stop_time = ParallelDescriptor::second() - strt_time;

    // Reduce so all MPI ranks report the slowest one.
    ParallelDescriptor::ReduceRealMax(stop_time);
    amrex::Print() << "Run time = " << stop_time << std::endl;

    // Reduce model evaluation time across MPI ranks.
    ParallelDescriptor::ReduceRealMax(eval_time);
    amrex::Print() << "Eval time = " << eval_time << std::endl;

    // End final profiling section.
    BL_PROFILE_VAR_STOP(Post);
}
