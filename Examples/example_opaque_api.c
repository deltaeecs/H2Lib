/* Example demonstrating the proposed opaque API design */

#include "h2lib_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* User-defined kernel function - only needs indices */
static h2lib_field_t 
gaussian_kernel(h2lib_uint_t idx_i, h2lib_uint_t idx_j, void *user_data)
{
    /* The coordinates are handled internally by the library */
    /* User code never sees coordinate arrays */
    h2lib_kernelmatrix_t *km = (h2lib_kernelmatrix_t *)user_data;
    
    /* Library provides internal coordinate lookup */
    /* (This would be implemented in the API layer) */
    
    /* For this example, just return a simple value */
    double sigma = 0.5;
    double dist2 = (idx_i - idx_j) * (idx_i - idx_j) / 100.0; /* Simplified */
    return exp(-dist2 / (2.0 * sigma * sigma));
}

int main(int argc, char **argv)
{
    h2lib_context_t *ctx;
    h2lib_kernelmatrix_t *km;
    h2lib_h2matrix_t *H;
    h2lib_amatrix_t *A;
    h2lib_uint_t n_points = 100;
    h2lib_uint_t i;
    h2lib_real_t coords[2];
    h2lib_real_t error, norm;
    
    printf("========================================\n");
    printf("H2Lib Opaque API Example\n");
    printf("========================================\n\n");
    
    /* Initialize library - creates opaque context */
    ctx = h2lib_init(&argc, &argv);
    if (!ctx) {
        fprintf(stderr, "Failed to initialize H2Lib\n");
        return 1;
    }
    
    /* Create kernel matrix - opaque handle returned */
    km = h2lib_kernelmatrix_new(ctx, 2, n_points, 4);
    if (!km) {
        fprintf(stderr, "Failed to create kernel matrix: %s\n", 
                h2lib_get_error(ctx));
        h2lib_finalize(ctx);
        return 1;
    }
    
    /* Set point coordinates */
    printf("Setting up %u random points...\n", n_points);
    for (i = 0; i < n_points; i++) {
        coords[0] = (double)rand() / RAND_MAX;
        coords[1] = (double)rand() / RAND_MAX;
        h2lib_kernelmatrix_set_point(km, i, coords);
    }
    
    /* Set kernel function - user provides index-based function */
    h2lib_kernelmatrix_set_kernel(km, gaussian_kernel, km);
    
    /* Build H2-matrix */
    printf("Building H2-matrix...\n");
    H = h2lib_h2matrix_from_kernelmatrix(km, 20, 2.0, 1e-4);
    if (!H) {
        fprintf(stderr, "Failed to build H2-matrix: %s\n",
                h2lib_get_error(ctx));
        h2lib_kernelmatrix_delete(km);
        h2lib_finalize(ctx);
        return 1;
    }
    
    printf("  H2-matrix size: %.2f KB\n", 
           h2lib_h2matrix_getsize(H) / 1024.0);
    
    /* Build reference dense matrix */
    printf("Building reference dense matrix...\n");
    A = h2lib_amatrix_new(ctx, n_points, n_points);
    h2lib_amatrix_from_kernelmatrix(km, A);
    
    /* Compute error */
    printf("\nComputing approximation error...\n");
    error = h2lib_norm_diff(H, A);
    norm = h2lib_amatrix_norm(A);
    printf("  Absolute error: %.3e\n", error);
    printf("  Relative error: %.3e\n", error / norm);
    
    /* Cleanup - all done through opaque API */
    h2lib_amatrix_delete(A);
    h2lib_h2matrix_delete(H);
    h2lib_kernelmatrix_delete(km);
    h2lib_finalize(ctx);
    
    printf("\nDone!\n");
    return 0;
}

/*
 * Key advantages of this opaque API design:
 * 
 * 1. No internal headers exposed
 *    - User only includes h2lib_api.h
 *    - No dependencies on internal types
 * 
 * 2. Index-based kernel interface
 *    - Kernel function takes indices only
 *    - Coordinates handled internally
 * 
 * 3. Opaque handles
 *    - All pointers are to opaque types
 *    - Internal representation can change
 * 
 * 4. Error handling
 *    - Context-based error reporting
 *    - No need for global error state
 * 
 * 5. Resource management
 *    - Clear ownership through API
 *    - Explicit cleanup functions
 * 
 * This satisfies the requirements:
 * - Single header file
 * - Index-based kernel functions
 * - No coordinate exposure to users
 * - Clean encapsulation
 */
