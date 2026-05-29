#include <torch/script.h> // TorchScript loader and runtime inference API.

#include <AMReX_PlotFileUtil.H> // AMReX helper for writing plotfile output.
#include <AMReX_ParmParse.H> // AMReX helper for reading runtime parameters.

#include "myfunc.H" // Declares main_main().

using namespace amrex; // Avoid having to prefix AMReX symbols everywhere.

int main (int argc, char* argv[])
{
    // Initialize AMReX, MPI, and any enabled runtime backends.
    amrex::Initialize(argc,argv);

    // Run the tutorial logic.
    main_main();

    // Shut AMReX down cleanly.
    amrex::Finalize();
    return 0;
}

void main_main ()
{
    // Start a profiling section named "Init".
    BL_PROFILE_VAR("Init",Init);

    // Capture wall-clock time at the start of the run.
    Real strt_time = ParallelDescriptor::second();
    // Accumulate only the time spent inside model evaluation.
    Real eval_time = 0.0;

    // **********************************
    // SIMULATION PARAMETERS

    // Number of cells along each axis of the domain.
    int n_cell;

    // Maximum number of cells per AMReX box.
    int max_grid_size;

    // Path to the TorchScript model file.
    std::string model_filename;

    // Read runtime parameters from Exec/inputs.
    {
        ParmParse pp; // Create a parameter parser for the inputs file.

        // Required parameter: total cells per side.
        pp.get("n_cell",n_cell);

        // Required parameter: maximum box size.
        pp.get("max_grid_size",max_grid_size);

        // Optional parameter: model filename.
        pp.query("model_file", model_filename);
    }

    // **********************************
    // SIMULATION SETUP

    // BoxArray describes how the domain is split into boxes.
    BoxArray ba;
    // Geometry stores the physical domain description.
    Geometry geom;

    // Lower corner of the integer domain.
    IntVect dom_lo(AMREX_D_DECL(       0,        0,        0));
    // Upper corner of the integer domain.
    IntVect dom_hi(AMREX_D_DECL(n_cell-1, n_cell-1, n_cell-1));

    // Create one box covering the whole domain.
    Box domain(dom_lo, dom_hi);

    // Initialize the BoxArray with that one domain box.
    ba.define(domain);

    // Split boxes so none exceeds max_grid_size in any direction.
    ba.maxSize(max_grid_size);

    // Define the physical domain as [0,1] in each active dimension.
    RealBox real_box({AMREX_D_DECL( 0., 0., 0.)},
                     {AMREX_D_DECL( 1., 1., 1.)});

    // Mark all directions as non-periodic.
    Array<int,AMREX_SPACEDIM> is_periodic{AMREX_D_DECL(0,0,0)};

    // Build the AMReX geometry object.
    geom.define(domain, real_box, CoordSys::cartesian, is_periodic);

    // Cell size in each spatial direction.
    GpuArray<Real,AMREX_SPACEDIM> dx = geom.CellSizeArray();

    // No ghost cells in this example.
    int Nghost = 0;

    // One input component per cell.
    int Nc_in = 1;
    // Two output components per cell.
    int Nc_out = 2;

    // Distribute boxes across MPI ranks.
    DistributionMapping dm(ba);

    // phi_in holds the input field values.
    MultiFab phi_in(ba, dm, Nc_in, Nghost);
    // phi_out holds the model predictions.
    MultiFab phi_out(ba, dm, Nc_out, Nghost);

    // **********************************
    // INITIALIZE DATA

    // End the "Init" profiling section.
    BL_PROFILE_VAR_STOP(Init);

    // Start a profiling section for initialization.
    BL_PROFILE_VAR("InitData",InitData);

    // Loop over each AMReX box in the input field.
    for (MFIter mfi(phi_in); mfi.isValid(); ++mfi)
    {
        // Get the valid cell box for this iteration.
        const Box& bx = mfi.validbox();

        // Get a writable view into the input data.
        const Array4<Real>& phi_input = phi_in.array(mfi);

        // Fill each cell with a simple normalized coordinate sum.
        amrex::ParallelFor(bx, [=] AMREX_GPU_DEVICE(int i, int j, int k) noexcept
        {
            // Simple test function used as input data.
            phi_input(i,j,k) = (i+j+k)/(n_cell-1.0);
        });
    }

    // End the initialization profiling section.
    BL_PROFILE_VAR_STOP(InitData);

    // Start profiling the plotfile write.
    BL_PROFILE_VAR("WriteInitPlot",WriteInitPlot);

    // Write the initial condition to a plotfile for inspection.
    const std::string& pltfile = "plt_inputs";
    WriteSingleLevelPlotfile(pltfile, phi_in, {"dt"}, geom, 0.0, 0);

    // End the input plotfile profiling section.
    BL_PROFILE_VAR_STOP(WriteInitPlot);

    // **********************************
    // LOAD PYTORCH MODEL

    // Start profiling model load time.
    BL_PROFILE_VAR("LoadPytorch",LoadPytorch);

    // TorchScript module that will hold the loaded model.
    torch::jit::script::Module module;
    try {
        // Deserialize the serialized TorchScript model from disk.
        module = torch::jit::load(model_filename);
    }
    catch (const c10::Error& e) {
        // Abort if the model cannot be loaded.
        amrex::Abort("Error loading the model\n");
    }

    // Confirm the model load succeeded.
    Print() << "Model loaded.\n";

    // Use double precision tensors in this example.
    auto dtype0 = torch::kFloat64;

#ifdef AMREX_USE_CUDA
    // Select the CUDA device when GPU support is enabled.
    torch::Device device0(torch::kCUDA);
    // Move the loaded model to the GPU.
    module.to(device0);
    amrex::Print() << "Copying model to GPU." << std::endl;

    // Tensor options for CUDA tensors with the chosen dtype.
    auto tensoropt = torch::TensorOptions().dtype(dtype0).device(device0);
#else
    // Tensor options for CPU tensors with the chosen dtype.
    auto tensoropt = torch::TensorOptions().dtype(dtype0);
#endif

    // End the model-load profiling section.
    BL_PROFILE_VAR_STOP(LoadPytorch);

    // **********************************
    // EVALUATE MODEL

    // Start profiling the evaluation loop.
    BL_PROFILE_VAR("Eval",Eval);

    // Loop over each AMReX box in the input field.
    for ( MFIter mfi(phi_in); mfi.isValid(); ++mfi )
    {
        // Get the valid cell box for this iteration.
        const Box& bx = mfi.validbox();

        // Get read-only input data for this box.
        const Array4<Real>& phi_input = phi_in.array(mfi);
        // Get writable output data for this box.
        const Array4<Real>& phi_output = phi_out.array(mfi);

        // Lower corner of the current box.
        const IntVect bx_lo = bx.smallEnd();
        // Cell counts in the current box.
        const IntVect nbox = bx.size();

        // Total number of cells in this box.
        int ncell = AMREX_SPACEDIM == 2 ?
            nbox[0] * nbox[1] : nbox[0] * nbox[1] * nbox[2];

        // Allocate contiguous temporary storage for the tensor input.
        amrex::Gpu::ManagedVector<Real> aux(ncell*Nc_in);
        // Raw pointer used by torch::from_blob().
        Real* AMREX_RESTRICT auxPtr = aux.dataPtr();

        // Copy AMReX box data into the flat temporary buffer.
        amrex::ParallelFor(bx, Nc_in, [=] AMREX_GPU_DEVICE(int i, int j, int k, int n) noexcept
        {
            // Convert global cell indices to box-local indices.
            int ii = i - bx_lo[0];
            int jj = j - bx_lo[1];
            // Flatten 2D or 3D coordinates into a single cell index.
            int index = jj*nbox[0] + ii;
#if AMREX_SPACEDIM == 3
            int kk = k - bx_lo[2];
            index += kk*nbox[0]*nbox[1];
#endif
            // Store data in [cell][component] layout.
            auxPtr[index*Nc_in + n] = phi_input(i, j, k, n);
        });

        // Create a Torch tensor view over the contiguous buffer.
        at::Tensor inputs_torch = torch::from_blob(auxPtr, {ncell, Nc_in}, tensoropt);

        // Mark the start of the model call.
        Real eval_t_start = ParallelDescriptor::second();

        // Run the TorchScript model on the input tensor.
        at::Tensor outputs_torch = module.forward({inputs_torch}).toTensor();
        // Make sure the output uses the same numeric type as the AMReX data.
        outputs_torch = outputs_torch.to(dtype0);

        // Accumulate wall time spent inside model inference.
        eval_time += ParallelDescriptor::second() - eval_t_start;

        // Get a fast read-only accessor for the tensor output.
#ifdef AMREX_USE_CUDA
        auto outputs_torch_acc = outputs_torch.packed_accessor64<Real,2>();
#else
        auto outputs_torch_acc = outputs_torch.accessor<Real,2>();
#endif

        // Copy the model output back into the AMReX output MultiFab.
        amrex::ParallelFor(bx, Nc_out, [=] AMREX_GPU_DEVICE(int i, int j, int k, int n) noexcept
        {
            // Convert global cell indices to box-local indices.
            int ii = i - bx_lo[0];
            int jj = j - bx_lo[1];
            // Flatten coordinates into a single cell index.
            int index = jj*nbox[0] + ii;
#if AMREX_SPACEDIM == 3
            int kk = k - bx_lo[2];
            index += kk*nbox[0]*nbox[1];
#endif
            // Copy the n-th output component for this cell.
            phi_output(i, j, k, n) = outputs_torch_acc[index][n];
        });
    }

    // End the evaluation profiling section.
    BL_PROFILE_VAR_STOP(Eval);

    // Start post-processing / final reporting profiling.
    BL_PROFILE_VAR("Post",Post);

    // Print a completion message from the I/O processor.
    amrex::Print() << "Finish evaluating model.\n";

    // Write the output field to a plotfile for inspection.
    const std::string& pltfile2 = "plt_outputs";
    WriteSingleLevelPlotfile(pltfile2, phi_out, {"X_0", "X_1"}, geom, 1.0, 1);

    // Compute total runtime for the slowest MPI rank.
    Real stop_time = ParallelDescriptor::second() - strt_time;
    ParallelDescriptor::ReduceRealMax(stop_time);
    amrex::Print() << "Run time = " << stop_time << std::endl;

    // Compute evaluation time for the slowest MPI rank.
    ParallelDescriptor::ReduceRealMax(eval_time);
    amrex::Print() << "Eval time = " << eval_time << std::endl;

    // End the post-processing profiling section.
    BL_PROFILE_VAR_STOP(Post);
}
