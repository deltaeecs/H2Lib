# Complete Kernel Function Refactoring Plan

## Scope Analysis

Based on the feedback, the requirement is to refactor ALL kernel functions throughout the project to use matrix indices instead of coordinates. This is a comprehensive refactoring affecting:

### Files Requiring Changes

#### Core Kernel Modules
1. `Library/kernelmatrix.h/c` - ✅ Already completed
2. `Library/bem2d.h/c` - Needs refactoring
3. `Library/bem3d.h/c` - Needs refactoring

#### BEM Implementations
4. `Library/laplacebem2d.h/c` - Needs refactoring
5. `Library/laplacebem3d.h/c` - Needs refactoring  
6. `Library/helmholtzbem3d.h/c` - Needs refactoring
7. `Library/laplaceoclbem3d.h/c` - Needs refactoring (OpenCL)
8. `Library/helmholtzoclbem3d.h/c` - Needs refactoring (OpenCL)

#### Test Files (8 files)
- `Tests/test_laplacebem2d.c`
- `Tests/test_laplacebem3d.c`
- `Tests/test_laplacebem3d_ocl.c`
- `Tests/test_helmholtzbem3d.c`
- `Tests/test_helmholtzbem3d_ocl.c`
- `Tests/test_kernelmatrix.c` - ✅ Already completed

#### Example Files (3 files)
- `Examples/example_amatrix_bem3d.c`
- `Examples/example_h2matrix_bem3d.c`
- `Examples/example_hmatrix_bem3d.c`
- `Examples/example_simple_api.c` - ✅ Already completed

## Technical Challenges

### 1. BEM Kernel Functions
Current BEM kernels take coordinates directly:
```c
typedef field (*kernel_func3d)(const real *x, const real *y, 
                                const real *nx, const real *ny, void *data);
```

These need to become:
```c
typedef field (*kernel_func3d)(uint idx_x, uint idx_y, void *data);
```

**Issue**: BEM methods fundamentally work with:
- Point coordinates (x, y)
- Normal vectors (nx, ny)
- Surface geometries

All of these are coordinate-based. Converting to indices requires:
- Storing all point coordinates in data structures
- Storing all normal vectors in data structures
- Creating lookup mechanisms
- Ensuring numerical correctness is maintained

### 2. Fundamental Solution Evaluation
BEM kernels evaluate fundamental solutions like:
- Laplace: `G(x,y) = 1/(4π|x-y|)` for 3D
- Helmholtz: `G(x,y) = e^(ik|x-y|)/(4π|x-y|)`

These require actual coordinate distances. Index-based approach needs:
- Internal coordinate lookup
- Distance calculations
- Normal vector operations

### 3. OpenCL Kernels
OpenCL implementations work with raw memory and coordinates. Converting to indices requires significant changes to GPU kernel code.

## Proposed Implementation Strategy

### Phase 1: Extend bem3d/bem2d Core Structures
1. Add point coordinate storage to `bem3d`/`bem2d` structures
2. Add normal vector storage
3. Create index-to-coordinate lookup functions
4. Maintain backward compatibility with wrapper functions

### Phase 2: Refactor Kernel Function Signatures
1. Change `kernel_func3d` typedef to use indices
2. Change `kernel_func2d` typedef to use indices
3. Update all struct members using these types
4. Create internal coordinate-based implementations

### Phase 3: Update BEM Implementations
1. `laplacebem2d.c` - fundamental solution kernels
2. `laplacebem3d.c` - fundamental solution kernels
3. `helmholtzbem3d.c` - wave equation kernels
4. Update OpenCL implementations

### Phase 4: Update Tests and Examples
1. Refactor all 8 test files
2. Refactor all 3 example files
3. Verify numerical correctness

### Phase 5: Simplified API Enhancement
Create truly opaque API using pimpl pattern:
- Forward declarations only in public header
- All implementation details hidden
- Single `h2lib_api.h` header
- Factory functions for object creation
- No internal headers exposed

## Risk Assessment

**High Risk Areas:**
- Numerical accuracy (fundamental solutions depend on precise coordinate calculations)
- Performance (extra indirection for coordinate lookup)
- OpenCL kernels (GPU code is sensitive to memory access patterns)
- Backward compatibility (breaks existing user code)

**Estimated Effort:**
- Phase 1-2: 20-30 files modified
- Phase 3: 40-50 function signatures changed
- Phase 4: 10+ test/example files updated
- Phase 5: New API design and implementation
- Total: ~60-80 files, 200+ function changes, extensive testing required

## Alternative Approach

Given the scope and risks, an alternative is:
1. Keep BEM kernels coordinate-based internally (they need coordinates mathematically)
2. Provide index-based wrapper API at application level
3. Use adapter pattern to convert between index and coordinate interfaces
4. Focus on simplified public API (Phase 5) rather than internal refactoring

This maintains numerical correctness while achieving the goal of a simplified API for users.

## Recommendation

I recommend starting with the alternative approach:
1. Complete the simplified API properly (opaque pointers, single header)
2. Provide index-based factory functions and setup routines
3. Keep internal kernels coordinate-based where mathematically necessary
4. Document the design decision

If full index-based refactoring is still required after this, proceed with phased implementation and extensive testing.
