/* ------------------------------------------------------------
 * This is the file "h2lib_api.h" of the H2Lib package.
 * Public API with opaque pointers - no internal headers exposed.
 * ------------------------------------------------------------ */

/** @file h2lib_api.h
 *  @brief Public API for H2Lib with opaque pointers
 *  
 *  This header provides the complete public API for H2Lib without
 *  exposing any internal implementation details. All structures are
 *  opaque and manipulated through functions only.
 */

#ifndef H2LIB_API_H
#define H2LIB_API_H

#include <stddef.h>
#include <stdint.h>

/* Forward declarations of opaque types */
typedef struct h2lib_context_s h2lib_context_t;
typedef struct h2lib_kernelmatrix_s h2lib_kernelmatrix_t;
typedef struct h2lib_bem3d_s h2lib_bem3d_t;
typedef struct h2lib_bem2d_s h2lib_bem2d_t;
typedef struct h2lib_surface3d_s h2lib_surface3d_t;
typedef struct h2lib_curve2d_s h2lib_curve2d_t;
typedef struct h2lib_hmatrix_s h2lib_hmatrix_t;
typedef struct h2lib_h2matrix_s h2lib_h2matrix_t;
typedef struct h2lib_amatrix_s h2lib_amatrix_t;
typedef struct h2lib_cluster_s h2lib_cluster_t;
typedef struct h2lib_block_s h2lib_block_t;
typedef struct h2lib_clusterbasis_s h2lib_clusterbasis_t;

/* Type definitions */
#ifdef USE_COMPLEX
typedef double _Complex h2lib_field_t;
typedef const double _Complex h2lib_cfield_t;
#else
typedef double h2lib_field_t;
typedef const double h2lib_cfield_t;
#endif

typedef double h2lib_real_t;
typedef uint32_t h2lib_uint_t;

/* Kernel function type - index-based only */
typedef h2lib_field_t (*h2lib_kernel_func_t)(h2lib_uint_t idx_i, 
                                              h2lib_uint_t idx_j, 
                                              void *user_data);

/* ------------------------------------------------------------
 * Library initialization and cleanup
 * ------------------------------------------------------------ */

/**
 * @brief Initialize H2Lib
 * 
 * @param argc Pointer to argument count
 * @param argv Pointer to argument vector
 * @return Context handle, or NULL on failure
 */
h2lib_context_t* h2lib_init(int *argc, char ***argv);

/**
 * @brief Finalize H2Lib and free context
 * 
 * @param ctx Context handle
 */
void h2lib_finalize(h2lib_context_t *ctx);

/* ------------------------------------------------------------
 * Kernel Matrix API
 * ------------------------------------------------------------ */

/**
 * @brief Create a new kernel matrix
 * 
 * @param ctx Context handle
 * @param dim Spatial dimension
 * @param points Number of points
 * @param order Interpolation order
 * @return Kernel matrix handle, or NULL on failure
 */
h2lib_kernelmatrix_t* h2lib_kernelmatrix_new(h2lib_context_t *ctx,
                                              h2lib_uint_t dim,
                                              h2lib_uint_t points,
                                              h2lib_uint_t order);

/**
 * @brief Set point coordinates for kernel matrix
 * 
 * @param km Kernel matrix handle
 * @param idx Point index
 * @param coords Coordinates array (length must match dimension)
 */
void h2lib_kernelmatrix_set_point(h2lib_kernelmatrix_t *km,
                                   h2lib_uint_t idx,
                                   const h2lib_real_t *coords);

/**
 * @brief Set kernel function (index-based)
 * 
 * @param km Kernel matrix handle
 * @param kernel Kernel function
 * @param user_data User data passed to kernel function
 */
void h2lib_kernelmatrix_set_kernel(h2lib_kernelmatrix_t *km,
                                    h2lib_kernel_func_t kernel,
                                    void *user_data);

/**
 * @brief Delete kernel matrix
 * 
 * @param km Kernel matrix handle
 */
void h2lib_kernelmatrix_delete(h2lib_kernelmatrix_t *km);

/* ------------------------------------------------------------
 * BEM3D API
 * ------------------------------------------------------------ */

/**
 * @brief Problem types for BEM3D
 */
typedef enum {
    H2LIB_BEM3D_LAPLACE_SLP,
    H2LIB_BEM3D_LAPLACE_DLP,
    H2LIB_BEM3D_HELMHOLTZ_SLP,
    H2LIB_BEM3D_HELMHOLTZ_DLP
} h2lib_bem3d_problem_t;

/**
 * @brief Create a new BEM3D object
 * 
 * @param ctx Context handle
 * @param surface Surface mesh
 * @param problem Problem type
 * @return BEM3D handle, or NULL on failure
 */
h2lib_bem3d_t* h2lib_bem3d_new(h2lib_context_t *ctx,
                                h2lib_surface3d_t *surface,
                                h2lib_bem3d_problem_t problem);

/**
 * @brief Set wavenumber for Helmholtz problems
 * 
 * @param bem BEM3D handle
 * @param k Wavenumber
 */
void h2lib_bem3d_set_wavenumber(h2lib_bem3d_t *bem, h2lib_field_t k);

/**
 * @brief Delete BEM3D object
 * 
 * @param bem BEM3D handle
 */
void h2lib_bem3d_delete(h2lib_bem3d_t *bem);

/* ------------------------------------------------------------
 * Surface3D API
 * ------------------------------------------------------------ */

/**
 * @brief Create surface from file
 * 
 * @param ctx Context handle
 * @param filename File path
 * @return Surface handle, or NULL on failure
 */
h2lib_surface3d_t* h2lib_surface3d_from_file(h2lib_context_t *ctx,
                                              const char *filename);

/**
 * @brief Create sphere surface
 * 
 * @param ctx Context handle
 * @param center Center coordinates [x, y, z]
 * @param radius Radius
 * @param n_vertices Number of vertices
 * @return Surface handle, or NULL on failure
 */
h2lib_surface3d_t* h2lib_surface3d_sphere(h2lib_context_t *ctx,
                                           const h2lib_real_t center[3],
                                           h2lib_real_t radius,
                                           h2lib_uint_t n_vertices);

/**
 * @brief Delete surface
 * 
 * @param surface Surface handle
 */
void h2lib_surface3d_delete(h2lib_surface3d_t *surface);

/* ------------------------------------------------------------
 * H2-Matrix API
 * ------------------------------------------------------------ */

/**
 * @brief Build H2-matrix from kernel matrix
 * 
 * @param km Kernel matrix handle
 * @param leafsize Cluster tree leaf size
 * @param eta Admissibility parameter
 * @param eps Compression tolerance
 * @return H2-matrix handle, or NULL on failure
 */
h2lib_h2matrix_t* h2lib_h2matrix_from_kernelmatrix(h2lib_kernelmatrix_t *km,
                                                     h2lib_uint_t leafsize,
                                                     h2lib_real_t eta,
                                                     h2lib_real_t eps);

/**
 * @brief Build H2-matrix from BEM3D
 * 
 * @param bem BEM3D handle
 * @param leafsize Cluster tree leaf size
 * @param eta Admissibility parameter
 * @param eps Compression tolerance
 * @return H2-matrix handle, or NULL on failure
 */
h2lib_h2matrix_t* h2lib_h2matrix_from_bem3d(h2lib_bem3d_t *bem,
                                             h2lib_uint_t leafsize,
                                             h2lib_real_t eta,
                                             h2lib_real_t eps);

/**
 * @brief Get matrix size
 * 
 * @param H H2-matrix handle
 * @return Size in bytes
 */
size_t h2lib_h2matrix_getsize(h2lib_h2matrix_t *H);

/**
 * @brief Delete H2-matrix
 * 
 * @param H H2-matrix handle
 */
void h2lib_h2matrix_delete(h2lib_h2matrix_t *H);

/* ------------------------------------------------------------
 * Dense Matrix API (for reference/comparison)
 * ------------------------------------------------------------ */

/**
 * @brief Create dense matrix
 * 
 * @param ctx Context handle
 * @param rows Number of rows
 * @param cols Number of columns
 * @return Dense matrix handle, or NULL on failure
 */
h2lib_amatrix_t* h2lib_amatrix_new(h2lib_context_t *ctx,
                                    h2lib_uint_t rows,
                                    h2lib_uint_t cols);

/**
 * @brief Fill dense matrix from kernel matrix
 * 
 * @param km Kernel matrix handle
 * @param A Dense matrix to fill
 */
void h2lib_amatrix_from_kernelmatrix(h2lib_kernelmatrix_t *km,
                                      h2lib_amatrix_t *A);

/**
 * @brief Compute norm of dense matrix
 * 
 * @param A Dense matrix handle
 * @return Spectral norm
 */
h2lib_real_t h2lib_amatrix_norm(h2lib_amatrix_t *A);

/**
 * @brief Delete dense matrix
 * 
 * @param A Dense matrix handle
 */
void h2lib_amatrix_delete(h2lib_amatrix_t *A);

/* ------------------------------------------------------------
 * Utility Functions
 * ------------------------------------------------------------ */

/**
 * @brief Compute approximation error between H2-matrix and dense matrix
 * 
 * @param H H2-matrix handle
 * @param A Dense matrix handle
 * @return Approximation error (spectral norm of difference)
 */
h2lib_real_t h2lib_norm_diff(h2lib_h2matrix_t *H, h2lib_amatrix_t *A);

/**
 * @brief Get last error message
 * 
 * @param ctx Context handle
 * @return Error message string, or NULL if no error
 */
const char* h2lib_get_error(h2lib_context_t *ctx);

#endif /* H2LIB_API_H */
