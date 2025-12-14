# Library sources
set(H2LIB_CORE0
    basic.c
    settings.c
    parameters.c
    opencl.c
)

set(H2LIB_CORE1
    avector.c
    realavector.c
    amatrix.c
    factorizations.c
    eigensolvers.c
    sparsematrix.c
    sparsepattern.c
    gaussquad.c
    krylov.c
)

set(H2LIB_CORE2
    cluster.c
    clustergeometry.c
    block.c
    clusterbasis.c
    clusteroperator.c
    uniform.c
    h2matrix.c
    rkmatrix.c
    hmatrix.c
    krylovsolvers.c
    kernelmatrix.c
)

set(H2LIB_CORE3
    truncation.c
    harith.c
    harith2.c
    hcoarsen.c
    h2compression.c
    h2update.c
    h2arith.c
    aca.c
    visualize.c
    matrixnorms.c
)

set(H2LIB_DIRECTIONAL
    dcluster.c
    dblock.c
    dclusterbasis.c
    duniform.c
    dh2matrix.c
    dh2compression.c
    dclusteroperator.c
)

set(H2LIB_SIMPLE
    ie1d.c
)

set(H2LIB_FEM
    tri2d.c
    tri2dp1.c
    tet3d.c
    tet3dp1.c
    ddcluster.c
    tri2drt0.c
    tet3drt0.c
)

set(H2LIB_BEM
    curve2d.c
    singquad1d.c
    bem2d.c
    laplacebem2d.c
    surface3d.c
    macrosurface3d.c
    singquad2d.c
    bem3d.c
    oclbem3d.c
    laplacebem3d.c
    laplaceoclbem3d.c
    helmholtzbem3d.c
    helmholtzoclbem3d.c
)

# OpenCL file generation
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/laplaceoclbem3d.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/laplacebem3d.cl
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/laplacebem3d.cl ${CMAKE_CURRENT_SOURCE_DIR}/laplaceoclbem3d.c
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/helmholtzoclbem3d.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/helmholtzbem3d.cl
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/helmholtzbem3d.cl ${CMAKE_CURRENT_SOURCE_DIR}/helmholtzoclbem3d.c
)

# Combine all sources
set(H2LIB_SOURCES
    ${H2LIB_CORE0}
    ${H2LIB_CORE1}
    ${H2LIB_CORE2}
    ${H2LIB_CORE3}
    ${H2LIB_DIRECTIONAL}
    ${H2LIB_SIMPLE}
    ${H2LIB_FEM}
    ${H2LIB_BEM}
)

# Create static library
add_library(h2lib STATIC ${H2LIB_SOURCES})

# Link with required libraries
target_link_libraries(h2lib PUBLIC 
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
    ${EXTRA_LIBS}
    m
)

# Set include directories
target_include_directories(h2lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Additional headers that need to be installed but are not compiled
set(EXTRA_HEADERS
    blas.h
    clsettings.h
    simd_avx.h
    simd_sse2.h
    simd.h
)