/*
 * makeoctree.c
 *
 * Create a min-max octree for the brainsmall data set.
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

main()
{
    vpContext *vpc;	/* rendering context */
    int volume_fd;	/* file descriptor for volume data (input) */
    int octree_fd;	/* file descriptor for octree (output) */

    /* create a context */
    vpc = vpCreateContext();

    /* load the volume */
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

    /* compute the octree */
    vpSetClassifierTable(vpc, DENSITY_PARAM, DENSITY_FIELD, NULL, 0);
    vpSetClassifierTable(vpc, GRADIENT_PARAM, GRADIENT_FIELD, NULL, 0);
    vpMinMaxOctreeThreshold(vpc, DENSITY_PARAM, OCTREE_DENSITY_THRESH);
    vpMinMaxOctreeThreshold(vpc, GRADIENT_PARAM, OCTREE_GRADIENT_THRESH);
    if (vpCreateMinMaxOctree(vpc, 0, OCTREE_BASE_NODE_SIZE) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }

    /* store octree in a file */
    if ((octree_fd = open(OCTREE_FILE, O_BINARY|O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
	perror("open");
	fprintf(stderr, "could not open %s\n", OCTREE_FILE);
	exit(1);
    }
    if (vpStoreMinMaxOctree(vpc, octree_fd) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n",
		vpGetErrorString(vpGetError(vpc)));
	exit(1);
    }
    close(octree_fd);

    return(0);
}
