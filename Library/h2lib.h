
/* ------------------------------------------------------------
 * This is the file "h2lib.h" of the H2Lib package.
 * Simplified API wrapper for easy third-party usage.
 * ------------------------------------------------------------ */

/** @file h2lib.h
 *  @brief Simplified API wrapper for H2Lib
 *  
 *  This header provides a simplified interface to the H2Lib library,
 *  allowing users to work with hierarchical matrices without needing
 *  to include many individual headers.
 */

#ifndef H2LIB_H
#define H2LIB_H

/* Core functionality */
#include "settings.h"
#include "basic.h"
#include "parameters.h"

/* Vector and matrix types */
#include "avector.h"
#include "realavector.h"
#include "amatrix.h"
#include "sparsematrix.h"

/* Clustering */
#include "cluster.h"
#include "clustergeometry.h"
#include "block.h"

/* Hierarchical matrices */
#include "rkmatrix.h"
#include "hmatrix.h"
#include "h2matrix.h"
#include "clusterbasis.h"

/* Kernel matrices */
#include "kernelmatrix.h"

/* Compression and arithmetic */
#include "truncation.h"
#include "h2compression.h"
#include "harith.h"
#include "h2arith.h"

/* Solvers */
#include "krylov.h"
#include "krylovsolvers.h"

/* Matrix norms */
#include "matrixnorms.h"

/* Visualization (if enabled) */
#ifdef USE_CAIRO
#include "visualize.h"
#endif

/** @defgroup h2lib_simple Simplified H2Lib API
 *  @brief Convenient wrapper functions for common operations
 *  @{ */

/** @brief Initialize H2Lib
 *  
 *  This function must be called before using any H2Lib functionality.
 *  It initializes the library and processes command-line arguments.
 *  
 *  @param argc Pointer to argument count from main()
 *  @param argv Pointer to argument vector from main()
 */
static inline void
h2lib_init(int *argc, char ***argv)
{
  init_h2lib(argc, argv);
}

/** @brief Finalize H2Lib
 *  
 *  This function should be called before program exit to clean up
 *  resources used by H2Lib.
 */
static inline void
h2lib_finalize(void)
{
  uninit_h2lib();
}

/** @brief Helper to create kernel data structure
 *  
 *  This helper function creates a kernel data structure that wraps
 *  a coordinate-based kernel function to work with the index-based
 *  kernel interface.
 *  
 *  @param x Array of point coordinates
 *  @param kernel_coord Coordinate-based kernel function
 *  @param user_data User data to pass to kernel function
 *  @returns Kernel data structure
 */
typedef struct {
  real **x;
  field (*kernel_coord)(const real *xx, const real *yy, void *data);
  void *user_data;
} h2lib_kernel_data;

/** @brief Index-based kernel wrapper function
 *  
 *  This function wraps a coordinate-based kernel to work with indices.
 *  
 *  @param ii Row index
 *  @param jj Column index
 *  @param data Pointer to h2lib_kernel_data structure
 *  @returns Kernel value at (ii, jj)
 */
static inline field
h2lib_kernel_wrapper(uint ii, uint jj, void *data)
{
  h2lib_kernel_data *kd = (h2lib_kernel_data *) data;
  return kd->kernel_coord(kd->x[ii], kd->x[jj], kd->user_data);
}

/** @brief Setup kernel matrix with coordinate-based kernel
 *  
 *  This convenience function sets up a kernel matrix with a coordinate-based
 *  kernel function, handling the conversion to index-based interface.
 *  
 *  @param km Kernel matrix object
 *  @param kernel_coord Coordinate-based kernel function
 *  @param user_data Optional user data for kernel function
 */
static inline void
h2lib_setup_kernel(pkernelmatrix km, 
                   field (*kernel_coord)(const real *xx, const real *yy, void *data),
                   void *user_data)
{
  h2lib_kernel_data *kd;
  
  kd = (h2lib_kernel_data *) allocmem(sizeof(h2lib_kernel_data));
  kd->x = km->x;
  kd->kernel_coord = kernel_coord;
  kd->user_data = user_data;
  
  km->data = kd;
  km->kernel = h2lib_kernel_wrapper;
  km->kernel_internal = kernel_coord;
}

/** @brief Cleanup kernel matrix data
 *  
 *  This function cleans up the kernel data structure created by
 *  h2lib_setup_kernel.
 *  
 *  @param km Kernel matrix object
 */
static inline void
h2lib_cleanup_kernel(pkernelmatrix km)
{
  if (km->data) {
    freemem(km->data);
    km->data = NULL;
  }
}

/** @} */

#endif /* H2LIB_H */
