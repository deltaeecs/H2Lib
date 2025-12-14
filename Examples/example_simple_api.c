/* Simple example demonstrating the simplified H2Lib API for kernel matrices */

#include "h2lib.h"
#include <stdio.h>

/* Example kernel function: Gaussian kernel */
static field
kernel_gaussian(const real *xx, const real *yy, void *data)
{
  real norm2, sigma;
  
  (void) data;
  
  sigma = 0.5;
  norm2 = REAL_SQR(xx[0] - yy[0]) + REAL_SQR(xx[1] - yy[1]);
  
  return REAL_EXP(-norm2 / (2.0 * sigma * sigma));
}

int
main(int argc, char **argv)
{
  pkernelmatrix km;
  pclustergeometry cg;
  pcluster root;
  pblock broot;
  pclusterbasis cb;
  ph2matrix G;
  pamatrix A;
  uint points = 100;
  uint m = 4;
  uint leafsize = 20;
  uint *idx;
  real eta = 2.0;
  uint i;
  
  /* Initialize H2Lib */
  h2lib_init(&argc, &argv);
  
  printf("========================================\n");
  printf("Simple H2Lib Example - Kernel Matrices\n");
  printf("========================================\n\n");
  
  /* Create kernel matrix with 100 random points in 2D */
  printf("Creating kernel matrix for %u points...\n", points);
  km = new_kernelmatrix(2, points, m);
  
  /* Generate random points in [0,1]^2 */
  for(i = 0; i < points; i++) {
    km->x[i][0] = (real) rand() / RAND_MAX;
    km->x[i][1] = (real) rand() / RAND_MAX;
  }
  
  /* Setup kernel using simplified API */
  printf("Setting up Gaussian kernel...\n");
  h2lib_setup_kernel(km, kernel_gaussian, NULL);
  
  /* Create cluster geometry */
  printf("Creating cluster tree...\n");
  cg = creategeometry_kernelmatrix(km);
  
  /* Build cluster tree */
  idx = (uint *) allocmem(sizeof(uint) * points);
  for(i = 0; i < points; i++)
    idx[i] = i;
  root = build_adaptive_cluster(cg, points, idx, leafsize);
  printf("  %u clusters, depth %u\n", root->desc, getdepth_cluster(root));
  
  /* Build block tree */
  printf("Creating block tree...\n");
  broot = build_strict_block(root, root, &eta, admissible_2_cluster);
  printf("  %u blocks, depth %u\n", broot->desc, getdepth_block(broot));
  
  /* Create cluster basis */
  printf("Creating and filling cluster basis...\n");
  cb = build_from_cluster_clusterbasis(root);
  fill_clusterbasis_kernelmatrix(km, cb);
  
  /* Create and fill H2-matrix */
  printf("Creating and filling H2-matrix...\n");
  G = build_from_block_h2matrix(broot, cb, cb);
  fill_h2matrix_kernelmatrix(km, G);
  
  printf("  H2-matrix size: %.2f KB\n", 
         getsize_h2matrix(G) / 1024.0);
  
  /* Compare with dense matrix */
  printf("Creating reference dense matrix...\n");
  A = new_amatrix(points, points);
  fillN_kernelmatrix(0, 0, km, A);
  
  printf("  Dense matrix size: %.2f KB\n", 
         getsize_amatrix(A) / 1024.0);
  
  /* Compute approximation error */
  printf("\nComputing approximation error...\n");
  real error = norm2diff_amatrix_h2matrix(G, A);
  real norm = norm2_amatrix(A);
  printf("  Absolute error: %.3e\n", error);
  printf("  Relative error: %.3e\n", error / norm);
  
  /* Cleanup - Let H2Lib handle most cleanup automatically */
  printf("\nCleaning up...\n");
  h2lib_cleanup_kernel(km);
  
  printf("Done!\n\n");
  
  /* Finalize H2Lib - this will clean up all remaining structures */
  h2lib_finalize();
  
  return 0;
}
