add_library(h2lib_cpp STATIC h2lib_interface.cpp)

# Link against the C library
target_link_libraries(h2lib_cpp PUBLIC h2lib)

# Include directories
target_include_directories(h2lib_cpp PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/Library
)

set_target_properties(h2lib_cpp PROPERTIES CXX_STANDARD 17)
