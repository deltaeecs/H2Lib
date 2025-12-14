# Example programs
set(EXAMPLE_SOURCES
    example_dh2matrix.c
    example_visualize.c
    example_amatrix_bem3d.c
    example_hmatrix_bem3d.c
    example_h2matrix_bem3d.c
    example_fem_p1_harith.c
    example_fem_rt0_harith.c
    example_simple_api.c
    example_index_kernels.c
)

# Create example executables
foreach(example_source ${EXAMPLE_SOURCES})
    # Get the file name without extension
    get_filename_component(example_name ${example_source} NAME_WE)
    
    # Add executable
    add_executable(${example_name} ${example_source})
    
    # Link with h2lib
    target_link_libraries(${example_name} PRIVATE h2lib)
endforeach()