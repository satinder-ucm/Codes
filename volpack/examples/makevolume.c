/*
 * makevolume.c
 *
 * Create a volume from the brainsmall data set.
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
#include <stdlib.h>
#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif

main()
{
    vpContext *vpc;	/* rendering context */
    unsigned char *density; /* buffer for density data */
    unsigned density_size;/* size of density data */
    char *volume;	/* volume data */
    unsigned volume_size;/* size of volume */
    int density_fd;	/* file descriptor for density file (input) */
    int volume_fd;	/* file descriptor for volume (output) */

    /* create a context */
    vpc = vpCreateContext();

    /* describe the layout of the volume */
    vpSetVolumeSize(vpc, BRAIN_XLEN, BRAIN_YLEN, BRAIN_ZLEN);
    vpSetVoxelSize(vpc, BYTES_PER_VOXEL, VOXEL_FIELDS,
		   SHADE_FIELDS, CLSFY_FIELDS);
    vpSetVoxelField(vpc, NORMAL_FIELD, NORMAL_SIZE, NORMAL_OFFSET, NORMAL_MAX);
    vpSetVoxelField(vpc, DENSITY_FIELD, DENSITY_SIZE, DENSITY_OFFSET,
		    DENSITY_MAX);
    vpSetVoxelField(vpc, GRADIENT_FIELD, GRADIENT_SIZE, GRADIENT_OFFSET,
		    GRADIENT_MAX);

    /* allocate space for the raw data and the volume */
    density_size = BRAIN_XLEN * BRAIN_YLEN * BRAIN_ZLEN;
    density = (unsigned char*)malloc(density_size);
    volume_size = BRAIN_XLEN * BRAIN_YLEN * BRAIN_ZLEN * BYTES_PER_VOXEL;
    volume = (char*)malloc(volume_size);
    if (density == NULL || volume == NULL) {
	fprintf(stderr, "out of memory\n");
	exit(1);
    }
    vpSetRawVoxels(vpc, volume, volume_size, BYTES_PER_VOXEL,
		   BRAIN_XLEN * BYTES_PER_VOXEL,
		   BRAIN_YLEN * BRAIN_XLEN * BYTES_PER_VOXEL);

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

    /* compute surface normals (for shading) and
       gradient magnitudes (for classification) */
    if (vpVolumeNormals(vpc, density, density_size, DENSITY_FIELD,
			GRADIENT_FIELD, NORMAL_FIELD) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }

    /* store volume in a file */
    if ((volume_fd = open(VOLUME_FILE, O_BINARY|O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
	perror("open");
	fprintf(stderr, "could not open %s\n", VOLUME_FILE);
	exit(1);
    }
    if (vpStoreRawVolume(vpc, volume_fd) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }
    close(volume_fd);

    return(0);
}
