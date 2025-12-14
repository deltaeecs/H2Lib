# Install script for directory: /home/runner/work/H2Lib/H2Lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/H2Lib/H2Lib/build/Library/libh2lib.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/h2lib" TYPE FILE FILES
    "/home/runner/work/H2Lib/H2Lib/Library/aca.h"
    "/home/runner/work/H2Lib/H2Lib/Library/amatrix.h"
    "/home/runner/work/H2Lib/H2Lib/Library/avector.h"
    "/home/runner/work/H2Lib/H2Lib/Library/basic.h"
    "/home/runner/work/H2Lib/H2Lib/Library/bem2d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/bem3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/blas.h"
    "/home/runner/work/H2Lib/H2Lib/Library/block.h"
    "/home/runner/work/H2Lib/H2Lib/Library/clsettings.h"
    "/home/runner/work/H2Lib/H2Lib/Library/cluster.h"
    "/home/runner/work/H2Lib/H2Lib/Library/clusterbasis.h"
    "/home/runner/work/H2Lib/H2Lib/Library/clustergeometry.h"
    "/home/runner/work/H2Lib/H2Lib/Library/clusteroperator.h"
    "/home/runner/work/H2Lib/H2Lib/Library/curve2d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/dblock.h"
    "/home/runner/work/H2Lib/H2Lib/Library/dcluster.h"
    "/home/runner/work/H2Lib/H2Lib/Library/dclusterbasis.h"
    "/home/runner/work/H2Lib/H2Lib/Library/dclusteroperator.h"
    "/home/runner/work/H2Lib/H2Lib/Library/ddcluster.h"
    "/home/runner/work/H2Lib/H2Lib/Library/dh2compression.h"
    "/home/runner/work/H2Lib/H2Lib/Library/dh2matrix.h"
    "/home/runner/work/H2Lib/H2Lib/Library/duniform.h"
    "/home/runner/work/H2Lib/H2Lib/Library/eigensolvers.h"
    "/home/runner/work/H2Lib/H2Lib/Library/factorizations.h"
    "/home/runner/work/H2Lib/H2Lib/Library/gaussquad.h"
    "/home/runner/work/H2Lib/H2Lib/Library/h2arith.h"
    "/home/runner/work/H2Lib/H2Lib/Library/h2compression.h"
    "/home/runner/work/H2Lib/H2Lib/Library/h2matrix.h"
    "/home/runner/work/H2Lib/H2Lib/Library/h2update.h"
    "/home/runner/work/H2Lib/H2Lib/Library/harith.h"
    "/home/runner/work/H2Lib/H2Lib/Library/harith2.h"
    "/home/runner/work/H2Lib/H2Lib/Library/hcoarsen.h"
    "/home/runner/work/H2Lib/H2Lib/Library/helmholtzbem3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/helmholtzoclbem3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/hmatrix.h"
    "/home/runner/work/H2Lib/H2Lib/Library/ie1d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/kernelmatrix.h"
    "/home/runner/work/H2Lib/H2Lib/Library/krylov.h"
    "/home/runner/work/H2Lib/H2Lib/Library/krylovsolvers.h"
    "/home/runner/work/H2Lib/H2Lib/Library/laplacebem2d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/laplacebem3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/laplaceoclbem3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/macrosurface3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/matrixnorms.h"
    "/home/runner/work/H2Lib/H2Lib/Library/oclbem3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/opencl.h"
    "/home/runner/work/H2Lib/H2Lib/Library/parameters.h"
    "/home/runner/work/H2Lib/H2Lib/Library/realavector.h"
    "/home/runner/work/H2Lib/H2Lib/Library/rkmatrix.h"
    "/home/runner/work/H2Lib/H2Lib/Library/settings.h"
    "/home/runner/work/H2Lib/H2Lib/Library/simd.h"
    "/home/runner/work/H2Lib/H2Lib/Library/simd_avx.h"
    "/home/runner/work/H2Lib/H2Lib/Library/simd_sse2.h"
    "/home/runner/work/H2Lib/H2Lib/Library/singquad1d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/singquad2d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/sparsematrix.h"
    "/home/runner/work/H2Lib/H2Lib/Library/sparsepattern.h"
    "/home/runner/work/H2Lib/H2Lib/Library/surface3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/tet3d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/tet3dp1.h"
    "/home/runner/work/H2Lib/H2Lib/Library/tet3drt0.h"
    "/home/runner/work/H2Lib/H2Lib/Library/tri2d.h"
    "/home/runner/work/H2Lib/H2Lib/Library/tri2dp1.h"
    "/home/runner/work/H2Lib/H2Lib/Library/tri2drt0.h"
    "/home/runner/work/H2Lib/H2Lib/Library/truncation.h"
    "/home/runner/work/H2Lib/H2Lib/Library/uniform.h"
    "/home/runner/work/H2Lib/H2Lib/Library/visualize.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/runner/work/H2Lib/H2Lib/build/Library/cmake_install.cmake")
  include("/home/runner/work/H2Lib/H2Lib/build/Tests/cmake_install.cmake")
  include("/home/runner/work/H2Lib/H2Lib/build/Examples/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/H2Lib/H2Lib/build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/H2Lib/H2Lib/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
