#!/bin/csh -f

echo "*** makevolume ***"
time ./makevolume
echo "*** rendervolume ***"
time ./rendervolume
mv brainsmall.pgm brainsmall1.pgm

echo "*** makeoctree ***"
time ./makeoctree
echo "*** rendervolume -octree ***"
time ./rendervolume -octree
mv brainsmall.pgm brainsmall2.pgm

echo "*** classifyvolume ***"
time ./classifyvolume
echo "*** rendervolume -classified ***"
time ./rendervolume -classified
mv brainsmall.pgm brainsmall3.pgm

echo "*** classifyvolume -octree ***"
time ./classifyvolume -octree
echo "*** rendervolume -classified ***"
time ./rendervolume -classified
mv brainsmall.pgm brainsmall4.pgm
