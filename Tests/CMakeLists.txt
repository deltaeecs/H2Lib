# Test programs
set(TEST_SOURCES
    test_amatrix.c
    test_eigen.c
    test_hmatrix.c
    test_h2matrix.c
    test_kernelmatrix.c
    test_krylov.c
    test_krylovsolvers.c
    test_laplacebem2d.c
    test_laplacebem3d.c
    test_laplacebem3d_ocl.c
    test_helmholtzbem3d.c
    test_helmholtzbem3d_ocl.c
    test_h2compression.c
    test_tet3dp1.c
    test_tri2dp1.c
    test_ddcluster.c
    test_tri2drt0.c
    test_tet3drt0.c
)

# Create test executables
foreach(test_source ${TEST_SOURCES})
    # Get the file name without extension
    get_filename_component(test_name ${test_source} NAME_WE)
    
    # Add executable
    add_executable(${test_name} ${test_source})
    
    # Link with h2lib
    target_link_libraries(${test_name} PRIVATE h2lib)
    
    # Add to CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# Enable testing
enable_testing()