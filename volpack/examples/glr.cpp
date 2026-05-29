/*
 * (c) 1996-2000 The Regents of
 * the University of California (through E.O. Lawrence Berkeley National
 * Laboratory), subject to approval by the U.S. Department of Energy.

 * Your use of this software is under license -- the license agreement
 * is attached and included in the directory as license.txt or you may
 * contact Berkeley Lab's Technology Transfer Department at
 * TTD@lbl.gov.

 * NOTICE OF U.S. GOVERNMENT RIGHTS.  The Software was developed under
 * funding from the U.S. Government which consequently retains certain
 * rights as follows: the U.S. Government has been granted for itself
 * and * others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, * worldwide license in the Software to reproduce,
 * prepare derivative * works, and perform publicly and display
 * publicly.  Beginning five (5) * years after the date permission to
 * assert copyright is obtained from * the U.S. Department of Energy,
 * and subject to any subsequent five (5) * year renewals, the
 * U.S. Government is granted for itself and others * acting on its
 * behalf a paid-up, nonexclusive, irrevocable, worldwide * license in
 * the Software to reproduce, prepare derivative works, * distribute
 * copies to the public, perform publicly and display * publicly, and
 * to permit others to do so.
 */

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>

#ifdef WIN32
namespace std
{
    using ::abs;
}
#endif

#ifdef VP_MPI
#include <mpi.h>
#endif

#include <GL/glut.h>

#include <volpack.h>
//#include "volume.H"

namespace
{
vpContext *vpc = 0;		/* rendering context */
const int NORMAL_OFFSET	= 0;
const int DENSITY_OFFSET = NORMAL_OFFSET + VP_NORM_SIZE;
const int GRADIENT_OFFSET = DENSITY_OFFSET + VP_SCALAR_SIZE;
const int BYTES_PER_VOXEL = GRADIENT_OFFSET + VP_GRAD_SIZE;

const int VOXEL_FIELDS = 3;	/* number of fields in voxel */
const int SHADE_FIELDS = 2;	/* number of fields used for shading
				   (normal and density); must be the
				   1st fields of RawVoxel */
const int CLSFY_FIELDS = 2;	/* number of fields used for classifying
				   (density and gradient); can be any fields
				   in the RawVoxel */
const int NORMAL_FIELD = 0;
const int NORMAL_SIZE = 2;
const int NORMAL_MAX = VP_NORM_MAX;

const int DENSITY_FIELD = 1;
const int DENSITY_SIZE = 1;
const int DENSITY_MAX = 255;

const int GRADIENT_FIELD = 2;
const int GRADIENT_SIZE = 1;
const int GRADIENT_MAX = VP_GRAD_MAX;

const int DENSITY_PARAM = 0;		/* classification parameters */
const int OCTREE_DENSITY_THRESH	= 4;
const int GRADIENT_PARAM = 1;
const int OCTREE_GRADIENT_THRESH = 4;
const int OCTREE_BASE_NODE_SIZE	= 4;

#if 0
const int DENSITY_RAMP_POINTS = 3;			/* classification ramps */
int DensityRampX[] =    {  0,  24, 255};
float DensityRampY[] =  {0.0, 1.0, 1.0};
#endif

const int DENSITY_RAMP_POINTS = 2;			/* classification ramps */
int DensityRampX[] =    {  0,  255};
float DensityRampY[] =  {0.0, 1.0, 1.0};

const int GRADIENT_RAMP_POINTS = 4;
int GradientRampX[] =   {  0,   5,  20, 221};
float GradientRampY[] = {0.0, 0.0, 1.0, 1.0};
const int MATERIALS = 1;
const int COLOR_CHANNELS = 1;
int image_width = 512;
int image_height = 512;
unsigned char* image;		//[COLOR_CHANNELS][image_width][image_height];	/* output image */
int use_octree;			/* if true, use octree with the unclassified volume */
int use_clvolume;		/* if true, use the classified volume */
int use_brutal;			/* if true, use the brutal method, instead of raw */
float density_ramp[DENSITY_MAX+1]; /* opacity as a function of density */
float gradient_ramp[GRADIENT_MAX+1]; /* opacity as a function 
					of gradient magnitude */
float* shade_table;		//[NORMAL_MAX+1][MATERIALS][COLOR_CHANNELS]; /* shading lookup table */
float* weight_table = 0;		// [][MATERIALS][COLOR_CHANNELS];
}

void checkGL()
{
  GLenum errcode = glGetError();
  if ( errcode != GL_NO_ERROR )
    {
      std::cerr << "OpenGL Error: " << gluErrorString(errcode) << std::endl;
    }
}

void InitGL()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glClearColor(0.0, 0.0, 0.0, 0.0);
}

extern "C"
void Idle( void )
{
  /* update animation vars */
  glutPostRedisplay();
}


extern "C"
void Display( void )
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  //glPushMatrix();
  if ( true )
    {
      // glPixelZoom(0.0,0.0);
      glRasterPos2i(0, 0);
    }
  else
    {
      glRasterPos2i(0, 0);
    }
  checkGL();
  glDrawPixels(image_width, image_height, (COLOR_CHANNELS==1)?GL_LUMINANCE:GL_RGB, GL_UNSIGNED_BYTE, image);
  checkGL();
  /* draw stuff here */

  //glPopMatrix();

  glutSwapBuffers();
}


extern "C" void Reshape( int width, int height )
{
  glViewport( 0, 0, width, height );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D(0.0, GLdouble(width), 0.0, GLdouble(height));
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}

void RenderNow();
double spin_x = 0.0;
double spin_y = 0.0;
double spin_z = 0.0;

void
redisplay_all()
{
  vpIdentityMatrix(vpc);
  vpRotate (vpc, VP_X_AXIS, spin_y/2);
  vpRotate (vpc, VP_Y_AXIS, spin_x/2);
  vpRotate (vpc, VP_Z_AXIS, spin_z/2);
  RenderNow();
  glutPostRedisplay();
}

extern "C" void Key( unsigned char key, int x, int y )
{
  switch (key)
    {
    case 27:
    case 'q':
      exit(0);
      break;
    case 'x':
      spin_x += 20;
      break;
    case 'X':
      spin_x -= 20;
      break;
    case 'y':
      spin_y += 20;
      break;
    case 'Y':
      spin_y -= 20;
      break;
    case 'z':
      spin_z += 20;
      break;
    case 'Z':
      spin_z -= 20;
      break;
    case 'i':
      vpTranslate(vpc, +0.01, 0.0, 0.0);
      break;
    case 'I':
      vpTranslate(vpc, -0.01, 0.0, 0.0);
      break;
    case 'j':
      vpTranslate(vpc, +0.0, 0.01, 0.0);
      break;
    case 'J':
      vpTranslate(vpc,  0.0, -0.01, 0.0);
      break;
    case 'k':
      vpTranslate(vpc, +0.0, 0.0, 0.01);
      break;
    case 'K':
      vpTranslate(vpc, 0.0, 0.0, -0.01);
      break;
    case 'r':
      vpIdentityMatrix(vpc);
      // vpRotate(vpc, VP_X_AXIS, -15.0);
      // vpRotate(vpc, VP_Y_AXIS, 130.0);
      break;
    }
  redisplay_all();
}

extern "C" void screen_mouse(int button, int state, int x, int y)
{
  if ( button != GLUT_LEFT_BUTTON ) return;

  static int old_x;
  static int old_y;

  if ( state == GLUT_DOWN )
    {
      old_x = x;
      old_y = y;
    }
  else if ( state == GLUT_UP )
    {
      spin_x += x - old_x;
      spin_y += y - old_y;
      redisplay_all();
    }
}

void Init(int& argc, char**& argv);

int main( int argc, char **argv )
{
  glutInit( &argc, argv );
  glutInitWindowSize( image_width, image_height );

  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

  glutCreateWindow(argv[0]);

  InitGL();
  Init(argc, argv);

  glutReshapeFunc( Reshape );
  glutKeyboardFunc( Key );
  glutMouseFunc(screen_mouse);
  glutDisplayFunc( Display );
  // glutIdleFunc( Idle );

  glutMainLoop();
  return 0;
}

void PutPGM(const unsigned char *image, int width, int height, const char *filename);
unsigned char* GetPGM(int& width, int& height, const char* filename);
void load_volume();
void build_model();

void
Init(int& argc, char**& argv)
{
#ifdef VP_MPI
  MPI_Init(&argc, &argv);
#endif

  /* check command-line arguments */
  use_octree = 0;
  use_clvolume = 0;
  use_brutal = 0;
  while (--argc > 0)
    {
      argv++;
      if (!strcmp(*argv, "-brutal"))
	use_brutal = 1;
      else if (!strcmp(*argv, "-octree"))
	use_octree = 1;
      else if (!strcmp(*argv, "-classify"))
	use_clvolume = 1;
      else
	{
	  fprintf(stderr, "Usage: %s [[-octree ] [-classify]|[-brutal]] \n",
		  argv[0]);
	  exit(1);
	}
    }

  /* create a context */
  vpc = vpCreateContext();
  // vpSetDebug(vpc, VPDEBUG_COMPOSITE, 1);
  std::cout << "building model..." ; std::flush(std::cout);
  build_model();
  std::cout << "done" << std::endl;
  
  /* set the classification function */
  vpRamp(density_ramp, sizeof(float), DENSITY_RAMP_POINTS,
	 DensityRampX, DensityRampY);
  vpSetClassifierTable(vpc, DENSITY_PARAM, DENSITY_FIELD,
		       density_ramp, sizeof(density_ramp));
  vpRamp(gradient_ramp, sizeof(float), GRADIENT_RAMP_POINTS,
	 GradientRampX, GradientRampY);
  vpSetClassifierTable(vpc, GRADIENT_PARAM, GRADIENT_FIELD,
		       gradient_ramp, sizeof(gradient_ramp));
  vpSetd(vpc, VP_MIN_VOXEL_OPACITY, 0.05);
  vpSetd(vpc, VP_MAX_RAY_OPACITY, 0.95);


  /* set the image buffer */
  image = new unsigned char [image_width*image_height*COLOR_CHANNELS];
  vpSetImage(vpc, (unsigned char *)image, image_width, image_height,
 	     image_width*(COLOR_CHANNELS), (COLOR_CHANNELS==1)?VP_LUMINANCE:VP_RGB);

  if ( use_octree )
    {
      vpMinMaxOctreeThreshold(vpc, DENSITY_PARAM, OCTREE_DENSITY_THRESH);
      vpMinMaxOctreeThreshold(vpc, GRADIENT_PARAM, OCTREE_GRADIENT_THRESH);
      if (vpCreateMinMaxOctree(vpc, 0, OCTREE_BASE_NODE_SIZE) != VP_OK)
	{
	  fprintf(stderr, "VolPack error: %s\n",
		  vpGetErrorString(vpGetError(vpc)));
	  exit(1);
	}
    }

  /* set the shading parameters */
  int shade_table_size = (NORMAL_MAX+1)*MATERIALS*COLOR_CHANNELS;
  shade_table = new float [shade_table_size]; /* shading lookup table */
  if ( false )
    {
      weight_table = new float [0];
    }
  vpSetLookupShader(vpc, COLOR_CHANNELS, MATERIALS,
		    NORMAL_FIELD, shade_table, shade_table_size*sizeof(float),
		    0, 0, 0);
  //vpSetMaterial(vpc, VP_MATERIAL0, VP_AMBIENT, VP_BOTH_SIDES, 0.18, 0.18, 0.18);
  //vpSetMaterial(vpc, VP_MATERIAL0, VP_DIFFUSE, VP_BOTH_SIDES, 0.35, 0.35, 0.35);
  //vpSetMaterial(vpc, VP_MATERIAL0, VP_SPECULAR, VP_BOTH_SIDES, 0.79, 0.39, 0.39);
  vpSetMaterial(vpc, VP_MATERIAL0, VP_AMBIENT, VP_BOTH_SIDES, 1.0, 1.0, 1.0);
  vpSetMaterial(vpc, VP_MATERIAL0, VP_DIFFUSE, VP_BOTH_SIDES, 1.0, 1.0, 1.0);
  vpSetMaterial(vpc, VP_MATERIAL0, VP_SPECULAR, VP_BOTH_SIDES, 1.0, 1.0, 1.0);
  vpSetMaterial(vpc, VP_MATERIAL0, VP_SHINYNESS, VP_BOTH_SIDES, 10.0, 0.0, 0.0);

  vpSetLight(vpc, VP_LIGHT0, VP_DIRECTION, 0.3, 0.3, 1.0);
  vpSetLight(vpc, VP_LIGHT0, VP_COLOR, 1.0, 1.0, 1.0);
  vpEnable(vpc, VP_LIGHT0, 1);

  vpSetDepthCueing(vpc, 1.4, 1.5);
  //vpEnable(vpc, VP_DEPTH_CUE, 1);


  /* set the initial viewing parameters */
  // vpSeti(vpc, VP_CONCAT_MODE, VP_CONCAT_LEFT);
  // vpRotate(vpc, VP_X_AXIS, -15.0);
  // vpRotate(vpc, VP_Y_AXIS, 130.0);
  vpCurrentMatrix(vpc, VP_PROJECT);
  vpIdentityMatrix(vpc);
  vpWindow(vpc, VP_PARALLEL, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
  vpCurrentMatrix(vpc, VP_MODEL);

  RenderNow();
#ifdef VP_MPI
  MPI_Finalize();
#endif
}

void
RenderNow()
{
  /* compute shading lookup table */
  if ( vpShadeTable(vpc) != VP_OK )
    {
      fprintf(stderr, "VolPack error: %s\n",
	      vpGetErrorString(vpGetError(vpc)));
      exit(1);
    }

  /* render */
  if ( use_clvolume )
    {
      static bool classified = false;
      if ( !classified )
	{
	  if ( vpClassifyVolume(vpc) != VP_OK )
	    {
	      fprintf(stderr, "VolPack error: %s\n",
		      vpGetErrorString(vpGetError(vpc)));
	      exit(1);
	    }
	  classified = true;
	}
      if ( vpRenderClassifiedVolume(vpc) != VP_OK )
	{
	  fprintf(stderr, "VolPack error: %s\n",
		  vpGetErrorString(vpGetError(vpc)));
	  exit(1);
	}
    }
  else if ( use_brutal )
    {
      if ( vpBruteForceRender(vpc) != VP_OK )
	{
	  fprintf(stderr, "VolPack error: %s\n",
		  vpGetErrorString(vpGetError(vpc)));
	  exit(1);
	}
    }
  else
    {
      if ( vpRenderRawVolume(vpc) != VP_OK )
	{
	  fprintf(stderr, "VolPack error: %s\n",
		  vpGetErrorString(vpGetError(vpc)));
	  exit(1);
	}
    }
}

template <class T>
class toyFab
{
public:
  toyFab(T* data_, int xlen_, int ylen_, int zlen_)
    : xlen(xlen_), ylen(ylen_), zlen(zlen_), data(data_)
  {}
  T& operator()(int i, int j, int k)
  {
    return data[i + xlen*(j + k*ylen)];
  }
  const T& operator()(int i, int j, int k) const
  {
    return data[i + xlen*(j + ylen*k)];
  }
  toyFab& operator=(const T& v)
  {
    for (int k = 0; k < zlen; ++k )
      {
	for (int j = 0; j < ylen; ++j)
	  {
	    for (int i = 0; i < xlen; ++i)
	      {
		operator()(i,j,k) = v;
	      }
	  }
      }
    return *this;
  }
    
private:
  const int xlen;
  const int ylen;
  const int zlen;
  T* data;
};

inline double hypot_squared(int i, int j, int k, double x, double y, double z)
{
  return (i-x)*(i-x)+(j-y)*(j-y)+(k-z)*(k-z);
}

void
build_model()
{
  int xlen = 128;
  int ylen = 128;
  int zlen = 128;
  
  vpSetVolumeSize(vpc, xlen, ylen, zlen);
  vpSetVoxelSize(vpc, BYTES_PER_VOXEL, VOXEL_FIELDS, SHADE_FIELDS, CLSFY_FIELDS);
  vpSetVoxelField(vpc, NORMAL_FIELD, NORMAL_SIZE, NORMAL_OFFSET, NORMAL_MAX);
  vpSetVoxelField(vpc, DENSITY_FIELD, DENSITY_SIZE, DENSITY_OFFSET, DENSITY_MAX);
  vpSetVoxelField(vpc, GRADIENT_FIELD, GRADIENT_SIZE, GRADIENT_OFFSET, GRADIENT_MAX);
  
  int density_size = xlen*ylen*zlen;
  unsigned char* density = new unsigned char[xlen*xlen*zlen];

  toyFab<unsigned char> df_density(density, xlen, ylen, zlen);

  df_density = 0;
  double cent_I = xlen/2;
  double cent_J = ylen/2;
  double cent_K = zlen/2;
  double cent_i[8] = { 25.0, 25.0, 25.0, 25.0, 75.0, 75.0, 75.0, 75.0};
  double cent_j[8] = { 25.0, 25.0, 75.0, 75.0, 25.0, 25.0, 75.0, 75.0};
  double cent_k[8] = { 25.0, 75.0, 25.0, 75.0, 25.0, 75.0, 25.0, 75.0};
  double rad[8]    = { 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0};
  unsigned char val[8];
  for (int i = 0; i < 8; ++i )
    {
      val[i] = ((i+1)*255)/8;
    }
  for (int k = 0; k < zlen; ++k )
    {
      for (int j = 0; j < ylen; ++j)
	{
	  for (int i = 0; i < xlen; ++i)
	    {
	      for ( int b = 0; b < 8; b++ )
		{
		  if ( hypot_squared(i,j,k,
			     cent_i[b]*xlen/100,
			     cent_j[b]*ylen/100,
			     cent_k[b]*zlen/100) <= rad[b]*rad[b])
		    {
		      df_density(i,j,k) = val[b];
		    }
		  if ( std::abs(i - cent_I) < 15
		       && std::abs(j-cent_J) < 25
		       && std::abs(k-cent_K) < 10)
		    {
		      df_density(i,j,k) = 128;
		    }
		}
	    }
	}
    }

  int volume_size = density_size*BYTES_PER_VOXEL;
  char* volume = new char[density_size*BYTES_PER_VOXEL];

  if ( vpSetRawVoxels(vpc, volume, volume_size,
		      BYTES_PER_VOXEL,
		      xlen*BYTES_PER_VOXEL,
		      xlen*ylen*BYTES_PER_VOXEL) )
    {
      throw vpGetErrorString(vpGetError(vpc));
    }
  if ( vpVolumeNormals(vpc, density, density_size,
		       DENSITY_FIELD, GRADIENT_FIELD, NORMAL_FIELD) )
    {
      throw vpGetErrorString(vpGetError(vpc));
    }
}

#if 0
void
load_volume()
{
  int volume_fd;
  if ((volume_fd = open(VOLUME_FILE, O_BINARY)) < 0)
    {
      perror("open");
      fprintf(stderr, "could not open %s\n", VOLUME_FILE);
      exit(1);
    }
  if (vpLoadRawVolume(vpc, volume_fd) != VP_OK)
    {
      fprintf(stderr, "VolPack error: %s\n",
	      vpGetErrorString(vpGetError(vpc)));
      fprintf(stderr, "could not load the volume from file %s\n",
	      VOLUME_FILE);
      exit(1);
    }
  close(volume_fd);
}
#endif
