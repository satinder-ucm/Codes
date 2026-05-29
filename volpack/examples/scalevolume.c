/*
 * scalevolume.c
 *
 * Scale a volume in .den format to a different resolution.
 * Usage: scalevolume input.den output.den xscale yscale zscale [option]
 * Options:
 *    -b	use box filter
 *    -t	use triangle filter
 *    -c	use cubic bspline filter
 *    -m	use Mitchell filter
 *    -g	use Gaussian filter
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
 * $Date: 2000-10-02 18:12:05 $
 * $Revision: 1.3 $
 */

#include <stdio.h>
#include <volpack.h>
#include <stdlib.h>
#include <string.h>

unsigned char* read_den(char* filename, int* xptr, int* yptr, int* zptr);
int write_den(char* filename, unsigned char* data, int xlen, int ylen, int zlen);

int
main(argc, argv)
int argc;
char **argv;
{
    char *src_file;				/* input file name */
    char *dst_file;				/* output file name */
    double xscale, yscale, zscale;		/* scale factors */
    int filter;					/* filter type code */
    unsigned char *src_volume;			/* buffer for input data */
    unsigned char *dst_volume;			/* buffer for output data */
    int src_xlen, src_ylen, src_zlen;	/* size of input data */
    int dst_xlen, dst_ylen, dst_zlen;	/* size of output data */
    unsigned dst_size;				/* size in bytes of output */
    int code;

    /* parse arguments */
    if (argc < 6 || argc > 7) {
	fprintf(stderr, "Usage: scalevolume input.den output.den ");
	fprintf(stderr, "xscale yscale zscale [-b -t -c -m -g]\n");
	exit(1);
    }
    src_file = argv[1];
    dst_file = argv[2];
    xscale = atof(argv[3]);
    yscale = atof(argv[4]);
    zscale = atof(argv[5]);
    filter = VP_LINEAR_FILTER;
    if (argc == 7) {
	if (!strcmp(argv[6], "-b")) {
	    filter = VP_BOX_FILTER;
	} else if (!strcmp(argv[6], "-t")) {
	    filter = VP_LINEAR_FILTER;
	} else if (!strcmp(argv[6], "-c")) {
	    filter = VP_BSPLINE_FILTER;
	} else if (!strcmp(argv[6], "-m")) {
	    filter = VP_MITCHELL_FILTER;
	} else if (!strcmp(argv[6], "-g")) {
	    filter = VP_GAUSSIAN_FILTER;
	} else {
	    fprintf(stderr, "invalid filter option %s\n", argv[6]);
	    exit(1);
	}
    }

    /* load input volume */
    src_volume = read_den(src_file, &src_xlen, &src_ylen, &src_zlen);
    if (src_volume == NULL)
	exit(1);

    /* allocate space for the output volume */
    dst_xlen = src_xlen * xscale;
    dst_ylen = src_ylen * yscale;
    dst_zlen = src_zlen * zscale;
    dst_size = dst_xlen * dst_ylen * dst_zlen;
    dst_volume = (unsigned char *)malloc(dst_size);
    if (dst_volume == NULL) {
	fprintf(stderr, "out of memory\n");
	exit(1);
    }

    /* scale the volume */
    if ((code = vpResample3D(src_volume, src_xlen, src_ylen, src_zlen,
			     dst_volume, dst_xlen, dst_ylen, dst_zlen,
			     VP_UCHAR, filter)) != VP_OK) {
	fprintf(stderr, "VolPack error: %s\n", vpGetErrorString(code));
	exit(1);
    }

    /* store the output volume */
    if (!write_den(dst_file, dst_volume, dst_xlen, dst_ylen, dst_zlen))
	exit(1);

    exit(0);
}


