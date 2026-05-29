/*
 * classifyvolume.c
 *
 * Create a classified volume from the brainsmall data set.
 *
 * Copyright (c) 1994 The Board of Trustees of The Leland Stanford
 * Junior University.  All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice and this permission notice appear in
 * all copies of this software and that you do not sell the software.
 * Commercial licensing is available by contacting the author.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author:
 *    Phil Lacroute
 *    Computer Systems Laboratory
 *    Electrical Engineering Dept.
 *    Stanford University
 *
 * This version of the VolPack library is a modified version of the
 * original available from Stanford University.  Modifications were
 * made by the Center for Computational Sciences and Engineering,
 * Lawrence Berkeley National Laboratory.  Modifications to VolPack
 * (c) 2000 The Regents of the University of California (through
 * E.O. Lawrence Berkeley National Laboratory), subject to approval by
 * the U.S. Department of Energy.  Your use of this software is under
 * license from Stanford University with respect to the underlying
 * original VolPack code (see copyright notice and permission above)
 * and The Regents of the University of California with respect to
 * modifications thereto (see AmrVis license.txt file for applicable
 * license terms).  Contact Berkeley Lab's Center for Computational
 * Sciences and Engineering at webmaster@mothra.lbl.gov or Berkeley
 * Lab's Technology Transfer Department at TTD@lbl.gov for questions
 * or to receive more information.
 *
 */

/*
 * $Date: 2002-01-30 23:34:35 $
 * $Revision: 1.5 $
 */

#include "volume.h"
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

main(argc, argv)
int argc;
char **argv;
{
    vpContext *vpc;	/* rendering context */
    int volume_fd;	/* file descriptor for volume (input) */
    int octree_fd;	/* file descriptor for octree (input) */
    int density_fd;	/* file descriptor for raw volume data (input) */
    int output_fd;	/* file descriptor for classified volume (output) */
    int use_rawdata;	/* if true, use raw data instead of volume */
    int use_octree;	/* if true, use octree with the volume */
    unsigned char *density;	/* buffer for density data */
    unsigned density_size;/* size of density data */
    float density_ramp[DENSITY_MAX+1];	/* opacity as a function of density */
    float gradient_ramp[GRADIENT_MAX+1];/* opacity as a function 
					   of gradient magnitude */

    /* check command-line arguments */
    use_octree = 0;
    use_rawdata = 0;
    if (argc > 1) {
	if (!strcmp(argv[1], "-octree"))
	    use_octree = 1;
	else if (!strcmp(argv[1], "-rawdata"))
	    use_rawdata = 1;
	else {
	    fprintf(stderr, "Usage: %s [-octree | -rawdata]\n", argv[0]);
	    exit(1);
	}
    }

    /* create a context */
    vpc = vpCreateContext();

    /* load input data: either raw data, or an unclassified volume with no
       octree, or an unclassified volume with an octree */
    if (use_rawdata) {
	/* describe the layout of the volume */
	vpSetVolumeSize(vpc, BRAIN_XLEN, BRAIN_YLEN, BRAIN_ZLEN);
	vpSetVoxelSize(vpc, BYTES_PER_VOXEL, VOXEL_FIELDS,
		       SHADE_FIELDS, CLSFY_FIELDS);
	vpSetVoxelField(vpc, NORMAL_FIELD, NORMAL_SIZE, NORMAL_OFFSET,
			NORMAL_MAX);
	vpSetVoxelField(vpc, DENSITY_FIELD, DENSITY_SIZE, DENSITY_OFFSET,
			DENSITY_MAX);
	vpSetVoxelField(vpc, GRADIENT_FIELD, GRADIENT_SIZE, GRADIENT_OFFSET,
			GRADIENT_MAX);

	/* allocate space for the raw data */
	density_size = BRAIN_XLEN * BRAIN_YLEN * BRAIN_ZLEN;
	density = (unsigned char*)malloc(density_size);
	if (density == NULL) {
	    fprintf(stderr, "out of memory\n");
	    exit(1);
	}

	/* load the raw data */
	if ((density_fd = open(BRAIN_FILE, O_BINARY)) < 0) {
	    perror("open");
	    fprintf(stderr, "could not open %s\n", BRAIN_FILE);
	    exit(1);
	}
	if (lseek(density_fd, BRAIN_HEADER, 0) < 0) {
	    perror("seek");
	    fprintf(stderr, "could not read data from %s\n", BRAIN_FILE);
	    exit(1);
	}
	if (read(density_fd, density, density_size) != density_size) {
	    perror("read");
	    fprintf(stderr, "could not read data from %s\n", BRAIN_FILE);
	    exit(1);
	}
	close(density_fd);
    } else {
	/* load the unclassified volume data */
	if ((volume_fd = open(VOLUME_FILE, O_BINARY)) < 0) {
	    perror("open");
	    fprintf(stderr, "could not open %s\n", VOLUME_FILE);
	    exit(1);
	}
	if (vpLoadRawVolume(vpc, volume_fd) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    fprintf(stderr, "could not load the volume from file %s\n",
		    VOLUME_FILE);
	    exit(1);
	}
	close(volume_fd);
    }

    /* set the classification function */
    vpRamp(density_ramp, sizeof(float), DENSITY_RAMP_POINTS, DensityRampX,
	   DensityRampY);
    vpSetClassifierTable(vpc, DENSITY_PARAM, DENSITY_FIELD, density_ramp,
			 sizeof(density_ramp));
    vpRamp(gradient_ramp, sizeof(float), GRADIENT_RAMP_POINTS, GradientRampX,
	   GradientRampY);
    vpSetClassifierTable(vpc, GRADIENT_PARAM, GRADIENT_FIELD,
			 gradient_ramp, sizeof(gradient_ramp));
    vpSetd(vpc, VP_MIN_VOXEL_OPACITY, 0.05);

    /* load the octree */
    if (use_octree) {
	/* load the octree */
	if ((octree_fd = open(OCTREE_FILE, O_BINARY)) < 0) {
	    perror("open");
	    fprintf(stderr, "could not open %s\n", OCTREE_FILE);
	    exit(1);
	}
	if (vpLoadMinMaxOctree(vpc, octree_fd) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    fprintf(stderr, "could not load the octree from file %s\n",
		    OCTREE_FILE);
	    exit(1);
	}
	close(octree_fd);
    }

    /* classify */
    if (use_rawdata) {
	if (vpClassifyScalars(vpc, density, density_size, DENSITY_FIELD,
			      GRADIENT_FIELD, NORMAL_FIELD) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    exit(1);
	}
    } else {
	if (vpClassifyVolume(vpc) != VP_OK) {
	    fprintf(stderr, "VolPack error: %s\n",
		    vpGetErrorString(vpGetError(vpc)));
	    exit(1);
	}
    }

    /* store the classified volume */
    if ((output_fd = open(CLVOLUME_FILE, O_BINARY|O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
	perror("open");
	fprintf(stderr, "could not open %s\n", CLVOLUME_FILE);
	exit(1);
    }
    if (vpStoreClassifiedVolume(vpc, output_fd) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }
    close(output_fd);

    return(0);
}
