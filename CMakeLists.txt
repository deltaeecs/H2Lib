cmake_minimum_required(VERSION 3.12)
project(H2Lib 
    VERSION 1.0
    DESCRIPTION "Hierarchical Matrices Library"
    LANGUAGES C CXX
)

# Add cmake modules path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compile options
option(HARITH_RKMATRIX_QUICK_EXIT "Enable quick exit in HARITH RKMATRIX" OFF)
option(HARITH_AMATRIX_QUICK_EXIT "Enable quick exit in HARITH AMATRIX" OFF)
option(TRACE_MEMORY "Enable memory tracing" OFF)
option(BUILD_DOC "Build documentation" OFF)

# Build configuration options
option(BRIEF_OUTPUT "Brief output during build process" ON)
option(DEBUG "Build with debug information" ON)
option(OPT "Build with optimization" OFF)

# Set build type based on DEBUG/OPT options
if(DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
elseif(OPT)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler flags based on build type
if(MSVC)
    set(CMAKE_C_FLAGS_DEBUG "/Od /Zi /MDd")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /MD")
else()
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
endif()

# Set compile definitions based on options
if(HARITH_RKMATRIX_QUICK_EXIT)
    add_compile_definitions(HARITH_RKMATRIX_QUICK_EXIT)
endif()
if(HARITH_AMATRIX_QUICK_EXIT)
    add_compile_definitions(HARITH_AMATRIX_QUICK_EXIT)
endif()
if(TRACE_MEMORY)
    add_compile_definitions(TRACE_MEMORY)
endif()

# Feature options
option(USE_COMPLEX "Use complex numbers" ON)
option(USE_FLOAT "Use single precision floating point numbers" OFF)
option(USE_THREADSAFE_LAPACK "Use thread-safe version of LAPACK" OFF)
option(USE_SIMD "Use SIMD-instructions for compute intense tasks" ON)
option(USE_TRIQUADPOINTS "Precompute quadrature points on every triangle for BEM applications" OFF)

# Set feature compile definitions
if(USE_COMPLEX)
    add_compile_definitions(USE_COMPLEX)
endif()
if(USE_FLOAT)
    add_compile_definitions(USE_FLOAT)
endif()
if(USE_THREADSAFE_LAPACK)
    add_compile_definitions(USE_THREADSAFE_LAPACK)
endif()
if(USE_SIMD)
    add_compile_definitions(USE_SIMD)
endif()
if(USE_TRIQUADPOINTS)
    add_compile_definitions(USE_TRIQUADPOINTS)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/Library)

# External library options
option(USE_BLAS "Use BLAS/LAPACK for linear algebra" ON)
option(USE_CAIRO "Use Cairo for visualization" OFF)
option(USE_OPENMP "Use OpenMP for parallelization" ON)
option(USE_OPENCL "Use OpenCL for heterogeneous computing" OFF)
option(USE_NETCDF "Use NetCDF for writing and reading numerical data" OFF)
option(USE_ZLIB "Use zlib for compressing data files" ON)
option(USE_GTK3 "Use GTK+ 3.x for visualization" OFF)
option(USE_FREEGLUT "Use FreeGLUT for visualization" OFF)

# Find required packages
if(USE_BLAS)
    find_package(BLAS REQUIRED)
    find_package(LAPACK REQUIRED)
    add_compile_definitions(USE_BLAS)
endif()

# Windows specific libraries
if(WIN32)
    list(APPEND EXTRA_LIBS winmm)
endif()

# Find other external packages
if(USE_CAIRO)
    find_package(Cairo REQUIRED)
    add_compile_definitions(USE_CAIRO)
endif()

if(USE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_FOUND)
        add_compile_definitions(USE_OPENMP)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    endif()
endif()

if(USE_OPENCL)
    find_package(OpenCL)
    if(OpenCL_FOUND)
        add_compile_definitions(USE_OPENCL)
        list(APPEND EXTRA_LIBS ${OpenCL_LIBRARIES})
        include_directories(${OpenCL_INCLUDE_DIRS})
    endif()
endif()

if(USE_NETCDF)
    find_package(NetCDF REQUIRED)
    if(NOT USE_COMPLEX)
        add_compile_definitions(USE_NETCDF)
        list(APPEND EXTRA_LIBS ${NETCDF_LIBRARIES})
        include_directories(${NETCDF_INCLUDE_DIRS})
    endif()
endif()

if(USE_ZLIB)
    find_package(ZLIB REQUIRED)
    add_compile_definitions(USE_ZLIB)
    list(APPEND EXTRA_LIBS ZLIB::ZLIB)
endif()

if(USE_GTK3)
    find_package(GTK3 REQUIRED)
    add_compile_definitions(USE_GTK3)
    list(APPEND EXTRA_LIBS ${GTK3_LIBRARIES})
    include_directories(${GTK3_INCLUDE_DIRS})
endif()

if(USE_FREEGLUT)
    find_package(GLUT REQUIRED)
    add_compile_definitions(USE_FREEGLUT)
    list(APPEND EXTRA_LIBS ${GLUT_LIBRARIES})
    include_directories(${GLUT_INCLUDE_DIRS})
endif()

# Add subdirectories
add_subdirectory(Library)
add_subdirectory(Tests)
add_subdirectory(Examples)
add_subdirectory(src)

# Documentation
if(BUILD_DOC)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doc/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target(doc_doxygen
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# Install rules
install(TARGETS h2lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
file(GLOB H2LIB_HEADERS "Library/*.h")
install(FILES ${H2LIB_HEADERS} DESTINATION include/h2lib)