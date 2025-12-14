/* Example demonstrating index-based kernel functions */

#include "h2lib.h"
#include <stdio.h>
#include <stdlib.h>

/* Example: Gaussian kernel using coordinate-based interface
 * (h2lib internally converts this to index-based as needed) */
static field
gaussian_kernel_coord(const real *x_i, const real *x_j, void *user_data)
{
    real sigma = 0.5;
    (void)user_data;
    
    /* Compute distance */
    real dist2 = REAL_SQR(x_i[0] - x_j[0]) + REAL_SQR(x_i[1] - x_j[1]);
    
    /* Return Gaussian kernel value */
    return REAL_EXP(-dist2 / (2.0 * sigma * sigma));
}

int
main(int argc, char **argv)
{
  pkernelmatrix km;
  pclustergeometry cg;
  pcluster root;
  pblock broot;
  pclusterbasis cb;
  ph2matrix Gh;
  pamatrix G;
  uint points = 100;
  uint m = 4;
  uint leafsize = 20;
  uint *idx;
  real eta = 2.0;
  real eps = 1e-4;
  uint i;
  real error, norm;
  
  h2lib_init(&argc, &argv);
  
  printf("========================================\n");
  printf("Index-Based Kernel Functions Example\n");
  printf("========================================\n\n");
  
  /* Create kernel matrix */
  printf("Creating kernel matrix for %u points...\n", points);
  km = new_kernelmatrix(2, points, m);
  
  /* Set up random points */
  for(i = 0; i < points; i++) {
    km->x[i][0] = (real)rand() / RAND_MAX;
    km->x[i][1] = (real)rand() / RAND_MAX;
  }
  
  /* Use the simplified API to set up Gaussian kernel */
  printf("Setting up Gaussian kernel (coordinate-based API, internally converted to indices)...\n");
  h2lib_setup_kernel(km, gaussian_kernel_coord, NULL);
  
  /* Create cluster tree */
  printf("Creating cluster tree...\n");
  cg = creategeometry_kernelmatrix(km);
  idx = (uint *)allocmem(sizeof(uint) * points);
  for(i = 0; i < points; i++)
    idx[i] = i;
  root = build_adaptive_cluster(cg, points, idx, leafsize);
  printf("  %u clusters, depth %u\n", root->desc, getdepth_cluster(root));
  
  /* Create block tree */
  printf("Creating block tree...\n");
  broot = build_strict_block(root, root, &eta, admissible_2_cluster);
  printf("  %u blocks, depth %u\n", broot->desc, getdepth_block(broot));
  
  /* Build cluster basis */
  printf("Building cluster basis...\n");
  cb = build_from_cluster_clusterbasis(root);
  fill_clusterbasis_kernelmatrix(km, cb);
  
  /* Build H2-matrix */
  printf("Building H2-matrix...\n");
  Gh = build_from_block_h2matrix(broot, cb, cb);
  fill_h2matrix_kernelmatrix(km, Gh);
  printf("  H2-matrix size: %.2f KB\n", getsize_h2matrix(Gh) / 1024.0);
  
  /* Build reference dense matrix */
  printf("Building reference dense matrix...\n");
  G = new_amatrix(points, points);
  fillN_kernelmatrix(0, 0, km, G);
  printf("  Dense matrix size: %.2f KB\n", getsize_amatrix(G) / 1024.0);
  
  /* Compute error */
  printf("\nComputing approximation error...\n");
  error = norm2diff_amatrix_h2matrix(Gh, G);
  norm = norm2_amatrix(G);
  printf("  Absolute error: %.3e\n", error);
  printf("  Relative error: %.3e\n\n", error / norm);
  
  /* Show the API supports different kernel types */
  printf("Key features demonstrated:\n");
  printf("  ✓ Coordinate-based kernel in user code\n");
  printf("  ✓ Internally converted to index-based by h2lib\n");
  printf("  ✓ Simplified API (h2lib_setup_kernel)\n");
  printf("  ✓ Single header include (h2lib.h)\n");
  printf("  ✓ Clean separation of concerns\n\n");
  
  /* Cleanup */
  h2lib_cleanup_kernel(km);
  
  printf("Done!\n");
  h2lib_finalize();
  
  return 0;
}

/*
 * This example demonstrates:
 * 
 * 1. Simplified kernel interface:
 *    - User writes coordinate-based kernel: gaussian_kernel_coord(x, y, data)
 *    - h2lib_setup_kernel() handles conversion to internal index-based form
 *    - Transparent to user code
 * 
 * 2. Internal index-based operations:
 *    - kernelmatrix uses indices internally (idx_i, idx_j)
 *    - Coordinates looked up when needed
 *    - Efficient computation
 * 
 * 3. Simplified API:
 *    - h2lib_setup_kernel() handles wrapping
 *    - h2lib_cleanup_kernel() handles cleanup
 *    - Single h2lib.h header
 * 
 * 4. Best of both worlds:
 *    - User code stays simple (coordinates)
 *    - Internal code uses efficient indices
 *    - API layer bridges the gap
 */
