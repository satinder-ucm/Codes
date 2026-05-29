# (c) 1996-2000 The Regents of
# the University of California (through E.O. Lawrence Berkeley National
# Laboratory), subject to approval by the U.S. Department of Energy.

# Your use of this software is under license -- the license agreement is
# attached and included in the directory as license.txt or you may
# contact Berkeley Lab's Technology Transfer Department at TTD@lbl.gov.

# NOTICE OF U.S. GOVERNMENT RIGHTS.  The Software was developed under
# funding from the U.S. Government which consequently retains certain
# rights as follows: the U.S. Government has been granted for itself and
# others acting on its behalf a paid-up, nonexclusive, irrevocable,
# worldwide license in the Software to reproduce, prepare derivative
# works, and perform publicly and display publicly.  Beginning five (5)
# years after the date permission to assert copyright is obtained from
# the U.S. Department of Energy, and subject to any subsequent five (5)
# year renewals, the U.S. Government is granted for itself and others
# acting on its behalf a paid-up, nonexclusive, irrevocable, worldwide
# license in the Software to reproduce, prepare derivative works,
# distribute copies to the public, perform publicly and display
# publicly, and to permit others to do so.

COMP=g++
MPI=TRUE
DEBUG=TRUE
PROFILE=FALSE

VOLPACK = ..

include $(VOLPACK)/Include.mk

VPINCL = -I$(VOLPACK)
VPLIB = $(VOLPACK)/libvolpack.a

CPPFLAGS += -I. ${VPINCL} 
CFLAGS   += ${CCOPT_FLAGS}
CXXFLAGS += $(CXXOPT_FLAGS)

libraries = ${VPLIB} $(XTRALIBS) -lm

opengl = -lglut -lGL -lGLU -lXi -lXmu -lXext -lX11
opengl_ldflags = -L/usr/X11R6/lib

all: makevolume makeoctree classifyvolume rendervolume scalevolume glr

makevolume: makevolume.c volume.h $(VPLIB)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -o makevolume makevolume.c $(libraries)

makeoctree: makeoctree.c volume.h $(VPLIB)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -o makeoctree makeoctree.c $(libraries)

classifyvolume: classifyvolume.c volume.h $(VPLIB)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -o classifyvolume classifyvolume.c $(libraries)

rendervolume: rendervolume.c volume.h $(VPLIB)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -o rendervolume rendervolume.c $(libraries)

scalevolume: scalevolume.c denfile.o $(VPLIB)
	$(CC) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -o scalevolume scalevolume.c denfile.o $(libraries)

glr: glr.cpp $(VPLIB) denfile.o
	$(CXX) $(LDFLAGS) $(opengl_ldflags) $(CPPFLAGS) $(CXXFLAGS) -o glr glr.cpp denfile.o $(libraries) $(opengl)

clean:
	$(RM) makevolume makeoctree classifyvolume rendervolume scalevolume glr
	$(RM) scalevolume.o denfile.o
	$(RM) brainsmall.rv brainsmall.oct brainsmall.cv brainsmall.ppm
	$(RM) brainsmall1.ppm brainsmall2.ppm
	$(RM) brainsmall3.ppm brainsmall4.ppm
	$(RM) TAGS
	$(RM) *~
