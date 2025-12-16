#pragma once

#include <complex>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace h2lib {

using Scalar = std::complex<double>;
// Kernel function: takes row index i, col index j, returns value.
using KernelFunc = std::function<Scalar(size_t, size_t)>;

struct Stats {
    double build_time = 0.0;
    double matvec_time = 0.0;
    double lu_time = 0.0;
    double compression_ratio = 0.0; // (Compressed Size) / (Dense Size)
    size_t memory_usage = 0; // Bytes
    size_t dense_memory_usage = 0; // Bytes (theoretical)
};

enum class BackendType {
    HMatrix,
    H2Matrix
};

class MatrixInterface {
public:
    virtual ~MatrixInterface() = default;

    // Build the matrix from points and kernel
    // points: flat array of [x, y, z] coordinates (size 3*N)
    virtual void build(const std::vector<double>& points, const KernelFunc& kernel) = 0;

    // Matrix-Vector Multiplication: y = A * x
    virtual void matvec(const std::vector<Scalar>& x, std::vector<Scalar>& y) = 0;

    // LU Factorization (in-place or internal)
    virtual void factorize() = 0;

    // Solve system: A * x = b (requires factorize() called first)
    virtual void solve(const std::vector<Scalar>& b, std::vector<Scalar>& x) = 0;

    // Get statistics
    virtual Stats getStats() const = 0;
};

class Factory {
public:
    struct Config {
        double eta = 2.0; // Admissibility parameter
        int leaf_size = 40; // Max leaf size
        double tolerance = 1e-6; // Approximation tolerance
        int min_rank = 1; // Minimum rank (for H2)
    };

    static std::unique_ptr<MatrixInterface> create(BackendType type, const Config& config = Config());
};

} // namespace h2lib
