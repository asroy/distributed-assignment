#!/bin/bash

rm -f CMakeCache.txt
rm -f *.cmake
rm -rf CMakeFiles

DistributedAssignment_Source=/homes/cliu/code/github/distributed-assignment
DistributedAssignment_Install=../install.dir

cmake                                                         \
-D CMAKE_INSTALL_PREFIX=${DistributedAssignment_Install}      \
-D CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -std=c++11"            \
-D CMAKE_BUILD_TYPE=Debug                                     \
-D CMAKE_VERBOSE_MAKEFILE:BOOL=ON                             \
${DistributedAssignment_Source}


