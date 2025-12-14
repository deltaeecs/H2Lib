# H2Lib Simplified API

This document describes the simplified API provided by `h2lib.h` for easy integration of H2Lib into third-party applications.

## Overview

The simplified API provides a single header file (`h2lib.h`) that includes all commonly used H2Lib functionality, along with convenient helper functions for common operations.

## Key Features

1. **Single Header Include**: Just include `h2lib.h` instead of multiple individual headers
2. **Simplified Kernel Setup**: Easy-to-use functions for setting up kernel matrices
3. **Index-Based Kernel Functions**: Kernel functions now use matrix indices instead of coordinates
4. **Automatic Memory Management**: Helper functions for proper cleanup

## Quick Start

### Basic Usage

```c
#include "h2lib.h"

int main(int argc, char **argv)
{
    /* Initialize H2Lib */
    h2lib_init(&argc, &argv);
    
    /* Your code here */
    
    /* Finalize H2Lib */
    h2lib_finalize();
    
    return 0;
}
```

### Kernel Matrix Example

```c
#include "h2lib.h"

/* Define a coordinate-based kernel function */
static field
my_kernel(const real *xx, const real *yy, void *data)
{
    real norm2 = REAL_SQR(xx[0] - yy[0]) + REAL_SQR(xx[1] - yy[1]);
    return REAL_EXP(-norm2);
}

int main(int argc, char **argv)
{
    pkernelmatrix km;
    uint points = 100;
    uint m = 4;
    
    h2lib_init(&argc, &argv);
    
    /* Create kernel matrix */
    km = new_kernelmatrix(2, points, m);
    
    /* Initialize point coordinates */
    for(uint i = 0; i < points; i++) {
        km->x[i][0] = /* x coordinate */;
        km->x[i][1] = /* y coordinate */;
    }
    
    /* Setup kernel using simplified API */
    h2lib_setup_kernel(km, my_kernel, NULL);
    
    /* Use the kernel matrix... */
    
    /* Cleanup */
    h2lib_cleanup_kernel(km);
    
    h2lib_finalize();
    
    return 0;
}
```

## API Reference

### Initialization and Cleanup

- `h2lib_init(int *argc, char ***argv)` - Initialize H2Lib
- `h2lib_finalize(void)` - Finalize H2Lib and clean up resources

### Kernel Functions

- `h2lib_setup_kernel(km, kernel_coord, user_data)` - Setup a kernel matrix with a coordinate-based kernel function
- `h2lib_cleanup_kernel(km)` - Clean up kernel data

### Kernel Function Signature

Coordinate-based kernel functions should have the following signature:

```c
field my_kernel(const real *xx, const real *yy, void *data);
```

Where:
- `xx` - coordinates of the first point
- `yy` - coordinates of the second point  
- `data` - optional user data
- Returns the kernel value at (xx, yy)

## Migration Guide

### From Old API

**Before:**
```c
#include "kernelmatrix.h"
#include "basic.h"
#include "h2compression.h"
#include "h2matrix.h"
#include "parameters.h"
#include "matrixnorms.h"

field kernel(const real *xx, const real *yy, void *data) {
    /* ... */
}

int main(int argc, char **argv) {
    init_h2lib(&argc, &argv);
    km->kernel = kernel;
    /* ... */
    uninit_h2lib();
}
```

**After:**
```c
#include "h2lib.h"

field kernel(const real *xx, const real *yy, void *data) {
    /* ... */
}

int main(int argc, char **argv) {
    h2lib_init(&argc, &argv);
    h2lib_setup_kernel(km, kernel, NULL);
    /* ... */
    h2lib_cleanup_kernel(km);
    h2lib_finalize();
}
```

## Examples

See `Examples/example_simple_api.c` for a complete working example demonstrating:
- Kernel matrix creation
- Cluster tree construction
- H2-matrix assembly
- Error computation
- Proper cleanup

## Notes

1. The simplified API automatically handles the conversion between coordinate-based and index-based kernel functions internally.
2. Memory management is simplified - call `h2lib_cleanup_kernel()` before `h2lib_finalize()` to properly clean up kernel data.
3. Many H2Lib structures are automatically cleaned up by `h2lib_finalize()`, so explicit deletion is often not necessary.

## See Also

- Full H2Lib documentation: See `Doc/` directory
- API examples: See `Examples/` directory
- Test cases: See `Tests/` directory
