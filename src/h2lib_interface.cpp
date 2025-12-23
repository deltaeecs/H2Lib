#include "../include/h2lib_interface.hpp"

#include <complex.h>
#include <complex>

// Fix for H2Lib C headers in C++
#ifndef I
#define I ((__complex__ double) { 0.0, 1.0 })
#endif
#ifndef creal
#define creal(z) __real__(z)
#endif
#ifndef cimag
#define cimag(z) __imag__(z)
#endif

// H2Lib includes
// We define H2LIB_H to prevent re-inclusion if it was already included (unlikely)
extern "C" {
#include "../Library/aca.h"
#include "../Library/h2lib.h"
}

#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>

namespace h2lib {

// Static initialization helper
struct H2LibInitializer {
    H2LibInitializer()
    {
        int argc = 1;
        char* argv[] = { (char*)"h2lib_app", nullptr };
        char** argv_ptr = argv;
        init_h2lib(&argc, &argv_ptr);
        // set_verbosity(0); // Optional: reduce H2Lib output
    }
    ~H2LibInitializer()
    {
        uninit_h2lib();
    }
};

static void ensure_initialized()
{
    static H2LibInitializer init;
}

// Context for kernel callback
struct KernelContext {
    const KernelFunc* func;
    std::chrono::duration<double> kernel_time;
    long long kernel_calls;

    KernelContext()
        : func(nullptr)
        , kernel_time(0)
        , kernel_calls(0)
    {
    }
};

// H2Lib kernel callback (for H2 matrix and general kernelmatrix usage)
static field kernel_callback(uint i, uint j, void* data)
{
    KernelContext* ctx = static_cast<KernelContext*>(data);
    // We don't measure time here individually to avoid overhead if called frequently,
    // but for H2 construction it might be called less often than ACA.
    // Let's measure it.
    auto start = std::chrono::high_resolution_clock::now();
    Scalar val = (*ctx->func)(i, j);
    auto end = std::chrono::high_resolution_clock::now();
    ctx->kernel_time += (end - start);
    ctx->kernel_calls++;
    return *(field*)&val;
}

// ACA matrix entry callback (for H-matrix construction)
static void aca_entry_callback(const uint* ridx, const uint* cidx, void* data, const bool ntrans, pamatrix N)
{
    KernelContext* ctx = static_cast<KernelContext*>(data);

    // Debug print
    static int call_count = 0;
    /*
    if (call_count < 20) {
        std::cout << "Callback: " << N->rows << "x" << N->cols << " ntrans=" << ntrans
                  << " ridx=" << (void*)ridx << " cidx=" << (void*)cidx << std::endl;

        if (ridx) {
            std::cout << "ridx: ";
            for (uint k = 0; k < std::min((uint)N->rows, (uint)10); ++k) std::cout << ridx[k] << " ";
            std::cout << std::endl;
        }
        if (cidx) {
            std::cout << "cidx: ";
            for (uint k = 0; k < std::min((uint)N->cols, (uint)10); ++k) std::cout << cidx[k] << " ";
            std::cout << std::endl;
        }
        call_count++;
    }
    */

    // auto start = std::chrono::high_resolution_clock::now();

    for (uint j = 0; j < N->cols; ++j) {
        for (uint i = 0; i < N->rows; ++i) {
            uint row_idx, col_idx;

            if (ntrans) {
                row_idx = ridx ? ridx[j] : j;
                col_idx = cidx ? cidx[i] : i;
            } else {
                row_idx = ridx ? ridx[i] : i;
                col_idx = cidx ? cidx[j] : j;
            }

            if (row_idx >= 48 || col_idx >= 48) { // Hardcoded check for N=48
                // std::cerr << "ERROR: Index out of bounds! row=" << row_idx << " col=" << col_idx
                //           << " N=" << 48 << std::endl;
                // std::cerr << "Callback: " << N->rows << "x" << N->cols << " ntrans=" << ntrans << std::endl;
                // std::abort();
            }

            Scalar val;
            // The kernel function expects (row, col)
            // If ntrans is true, we are filling N with Transpose(SubMatrix).
            // But we already resolved row_idx and col_idx to be the Original matrix indices.
            // So we always call func(row_idx, col_idx).
            // Wait, let's check the original code.

            /* Original code:
            if (ntrans) {
                val = (*ctx->func)(col_idx, row_idx);
            } else {
                val = (*ctx->func)(row_idx, col_idx);
            }
            */

            // If ntrans is true, row_idx comes from ridx[j], col_idx comes from cidx[i].
            // ridx are ROW indices. cidx are COL indices.
            // So we want Original(row_idx, col_idx).
            // The original code swapped the arguments to func if ntrans was true.
            // That implies it thought col_idx was a row index and row_idx was a col index?
            // Or maybe it was trying to compute Transpose?

            // If ntrans is true, we are filling N(i, j) = Original(ridx[j], cidx[i]).
            // So we should call func(ridx[j], cidx[i]).
            // So we should call func(row_idx, col_idx).

            val = (*ctx->func)(row_idx, col_idx);

            N->a[i + j * N->ld] = *(field*)&val;
            // ctx->kernel_calls++; // Potential race condition
        }
    }

    // auto end = std::chrono::high_resolution_clock::now();
    // ctx->kernel_time += (end - start); // Potential race condition
}

// Recursive function to fill H-Matrix using ACA
static void fill_hmatrix_aca(phmatrix hm, void* data, double epsilon)
{
    if (hm->son) {
        for (uint i = 0; i < hm->rsons * hm->csons; ++i) {
            fill_hmatrix_aca(hm->son[i], data, epsilon);
        }
    } else {
        // Leaf
        uint* ridx = hm->rc->idx;
        uint rows = hm->rc->size;
        uint* cidx = hm->cc->idx;
        uint cols = hm->cc->size;

        if (hm->r) { // Admissible (rkmatrix)
            // Use ACA
            // decomp_partialaca_rkmatrix(entry, data, ridx, rows, cidx, cols, accur, rpivot, cpivot, R)
            decomp_partialaca_rkmatrix(aca_entry_callback, data, ridx, rows, cidx, cols, epsilon, NULL, NULL, hm->r);

            // Fix for LU: Diagonal blocks must be dense (amatrix)
            if (hm->rc == hm->cc) {
                // std::cout << "Converting diagonal R-block to F-block for LU safety" << std::endl;
                uint rows = hm->r->A.rows;
                uint cols = hm->r->B.rows;
                hm->f = new_amatrix(rows, cols);
                // Initialize to 0
                for (uint i = 0; i < hm->f->rows * hm->f->cols; ++i)
                    hm->f->a[i] = 0.0;

                add_rkmatrix_amatrix(1.0, false, hm->r, hm->f);
                del_rkmatrix(hm->r);
                hm->r = nullptr;
            }

        } else if (hm->f) { // Inadmissible (amatrix)
            // Fill dense
            aca_entry_callback(ridx, cidx, data, false, hm->f);
        }
    }
}

class H2LibMatrix : public MatrixInterface {
public:
    H2LibMatrix(BackendType type, const Factory::Config& config)
        : type_(type)
        , config_(config)
        , km_(nullptr)
        , cg_(nullptr)
        , ct_(nullptr)
        , bt_(nullptr)
        , hm_(nullptr)
        , h2_(nullptr)
        , cb_(nullptr)
        , L_(nullptr)
        , R_(nullptr)
        , rwf_(nullptr)
        , cwf_(nullptr)
        , rwflow_(nullptr)
        , cwflow_(nullptr)
        , rwfup_(nullptr)
        , cwfup_(nullptr)
    {
        ensure_initialized();
    }

    ~H2LibMatrix()
    {
        cleanup();
    }

    void build(const std::vector<double>& points, const KernelFunc& kernel) override
    {
        std::cout << "[H2Lib] Starting build..." << std::endl;
        cleanup();

        auto start_total = std::chrono::high_resolution_clock::now();
        ctx_.func = &kernel;
        ctx_.kernel_time = std::chrono::duration<double>::zero();
        ctx_.kernel_calls = 0;

        // 2. Setup Geometry
        size_t n_points = points.size() / 3;
        std::cout << "[H2Lib] Creating cluster geometry for " << n_points << " points..." << std::endl;
        cg_ = new_clustergeometry(3, n_points);
        for (size_t i = 0; i < n_points; ++i) {
            cg_->x[i][0] = points[3 * i + 0];
            cg_->x[i][1] = points[3 * i + 1];
            cg_->x[i][2] = points[3 * i + 2];
        }

        // 3. Build Cluster Tree
        std::cout << "[H2Lib] Building cluster tree..." << std::endl;
        uint* idx = (uint*)allocmem(sizeof(uint) * n_points);
        for (uint i = 0; i < n_points; i++)
            idx[i] = i;

        ct_ = build_adaptive_cluster(cg_, n_points, idx, config_.leaf_size);

        std::cout << "Clustering complete. idx array: ";
        for (size_t k = 0; k < std::min((size_t)n_points, (size_t)50); ++k)
            std::cout << idx[k] << " ";
        std::cout << std::endl;

        // 4. Build Block Tree
        std::cout << "[H2Lib] Building block tree..." << std::endl;
        real eta = config_.eta;
        bt_ = build_strict_block(ct_, ct_, &eta, admissible_2_cluster);

        // 5. Build H-Matrix
        std::cout << "[H2Lib] Building H-Matrix structure..." << std::endl;
        phmatrix hm_temp = build_from_block_hmatrix(bt_, 0);

        // Fill H-Matrix using ACA
        std::cout << "[H2Lib] Filling H-Matrix with ACA..." << std::endl;
        fill_hmatrix_aca(hm_temp, &ctx_, config_.tolerance);

        if (type_ == BackendType::HMatrix) {
            hm_ = hm_temp;
        } else {
            std::cout << "[H2Lib] Compressing to H2-Matrix..." << std::endl;
            truncmode tm;
            tm.absolute = false;
            tm.frobenius = true;
            tm.blocks = false;

            h2_ = compress_hmatrix_h2matrix(hm_temp, &tm, config_.tolerance);

            del_hmatrix(hm_temp);
        }
        std::cout << "[H2Lib] Build complete." << std::endl;

        auto end_total = std::chrono::high_resolution_clock::now();

        stats_.build_time = std::chrono::duration<double>(end_total - start_total).count();
        stats_.kernel_eval_time = ctx_.kernel_time.count();
        stats_.structure_time = stats_.build_time - stats_.kernel_eval_time;

        // Calculate compression ratio
        size_t dense_mem = n_points * n_points * sizeof(std::complex<double>); // Approx
        size_t actual_mem = 0;
        if (hm_) {
            actual_mem = getsize_hmatrix(hm_);
        } else if (h2_) {
            actual_mem = getsize_h2matrix(h2_);
        }
        stats_.compression_ratio = (double)actual_mem / (double)dense_mem;
        stats_.memory_usage = actual_mem;
    }

    void matvec(const std::vector<Scalar>& x, std::vector<Scalar>& y) override
    {
        size_t n = x.size();
        if (y.size() != n)
            y.resize(n);

        pavector vx = new_avector(n);
        pavector vy = new_avector(n);

        // Copy input
        for (size_t i = 0; i < n; ++i) {
            vx->v[i] = *(field*)&x[i];
            vy->v[i] = 0.0;
        }

        auto start = std::chrono::high_resolution_clock::now();

        field alpha = 1.0;
        if (type_ == BackendType::HMatrix && hm_) {
            addeval_hmatrix_avector(alpha, hm_, vx, vy);
        } else if (type_ == BackendType::H2Matrix && h2_) {
            addeval_h2matrix_avector(alpha, h2_, vx, vy);
        }

        auto end = std::chrono::high_resolution_clock::now();
        stats_.matvec_time = std::chrono::duration<double>(end - start).count();

        // Copy output
        for (size_t i = 0; i < n; ++i) {
            y[i] = *(Scalar*)&vy->v[i];
        }

        del_avector(vx);
        del_avector(vy);
    }

    void factorize() override
    {
        if (type_ == BackendType::HMatrix && hm_) {
            auto start = std::chrono::high_resolution_clock::now();

            truncmode tm;
            tm.absolute = false;
            tm.frobenius = true;
            tm.blocks = false;

            // LU Decomposition (LR decomposition in H2Lib terms)
            lrdecomp_hmatrix(hm_, &tm, config_.tolerance);

            auto end = std::chrono::high_resolution_clock::now();
            stats_.factorize_time = std::chrono::duration<double>(end - start).count();

        } else if (type_ == BackendType::H2Matrix && h2_) {
            auto start = std::chrono::high_resolution_clock::now();

            // Prepare structures for LR decomposition
            pccluster root = h2_->rb->t;

            pclusterbasis rblow = build_from_cluster_clusterbasis(root);
            pclusterbasis cblow = build_from_cluster_clusterbasis(root);
            L_ = build_from_block_lower_h2matrix(bt_, rblow, cblow);

            pclusterbasis rbup = build_from_cluster_clusterbasis(root);
            pclusterbasis cbup = build_from_cluster_clusterbasis(root);
            R_ = build_from_block_upper_h2matrix(bt_, rbup, cbup);

            truncmode tm;
            tm.frobenius = true;
            tm.absolute = false;
            tm.blocks = false;
            // Ensure other fields are zeroed if any (though struct seems small)
            // Better to be safe if struct has padding or other fields I missed

            rwf_ = prepare_row_clusteroperator(h2_->rb, h2_->cb, &tm);
            cwf_ = prepare_col_clusteroperator(h2_->rb, h2_->cb, &tm);
            rwflow_ = prepare_row_clusteroperator(L_->rb, L_->cb, &tm);
            cwflow_ = prepare_col_clusteroperator(L_->rb, L_->cb, &tm);
            rwfup_ = prepare_row_clusteroperator(R_->rb, R_->cb, &tm);
            cwfup_ = prepare_col_clusteroperator(R_->rb, R_->cb, &tm);

            lrdecomp_h2matrix(h2_, rwf_, cwf_, L_, rwflow_, cwflow_, R_, rwfup_, cwfup_, &tm, config_.tolerance);

            auto end = std::chrono::high_resolution_clock::now();
            stats_.factorize_time = std::chrono::duration<double>(end - start).count();
        }
    }

    void solve(const std::vector<Scalar>& b, std::vector<Scalar>& x) override
    {
        size_t n = b.size();
        if (x.size() != n)
            x.resize(n);

        pavector vb = new_avector(n);

        for (size_t i = 0; i < n; ++i) {
            vb->v[i] = *(field*)&b[i];
        }

        auto start = std::chrono::high_resolution_clock::now();

        if (type_ == BackendType::HMatrix && hm_) {
            // Solve using LR factors
            // lrsolve_hmatrix_avector(bool atrans, pchmatrix a, pavector x)
            // x is overwritten by solution.
            lrsolve_hmatrix_avector(false, hm_, vb);
        } else if (type_ == BackendType::H2Matrix && L_ && R_) {
            lrsolve_h2matrix_avector(L_, R_, vb);
        } else {
            // Fallback or error
            std::cerr << "Warning: Solve not implemented for this backend or matrix not factorized." << std::endl;
        }

        auto end = std::chrono::high_resolution_clock::now();
        stats_.solve_time = std::chrono::duration<double>(end - start).count();

        for (size_t i = 0; i < n; ++i) {
            x[i] = *(Scalar*)&vb->v[i];
        }
        del_avector(vb);
    }

    Stats getStats() const override
    {
        return stats_;
    }

private:
    void cleanup()
    {
        if (hm_) {
            del_hmatrix(hm_);
            hm_ = nullptr;
        }
        if (h2_) {
            del_h2matrix(h2_);
            h2_ = nullptr;
        }
        if (bt_) {
            del_block(bt_);
            bt_ = nullptr;
        }
        if (ct_) {
            del_cluster(ct_);
            ct_ = nullptr;
        }
        if (cg_) {
            del_clustergeometry(cg_);
            cg_ = nullptr;
        }
        if (km_) {
            del_kernelmatrix(km_);
            km_ = nullptr;
        }

        // Cleanup H2 Factorization
        if (L_) {
            del_h2matrix(L_);
            L_ = nullptr;
        }
        if (R_) {
            del_h2matrix(R_);
            R_ = nullptr;
        }
        if (rwf_) {
            del_clusteroperator(rwf_);
            rwf_ = nullptr;
        }
        if (cwf_) {
            del_clusteroperator(cwf_);
            cwf_ = nullptr;
        }
        if (rwflow_) {
            del_clusteroperator(rwflow_);
            rwflow_ = nullptr;
        }
        if (cwflow_) {
            del_clusteroperator(cwflow_);
            cwflow_ = nullptr;
        }
        if (rwfup_) {
            del_clusteroperator(rwfup_);
            rwfup_ = nullptr;
        }
        if (cwfup_) {
            del_clusteroperator(cwfup_);
            cwfup_ = nullptr;
        }
    }

    BackendType type_;
    Factory::Config config_;
    Stats stats_;
    KernelContext ctx_;

    pkernelmatrix km_;
    pclustergeometry cg_;
    pcluster ct_;
    pblock bt_;
    phmatrix hm_;
    ph2matrix h2_;
    pclusterbasis cb_;

    // H2 Factorization data
    ph2matrix L_;
    ph2matrix R_;
    pclusteroperator rwf_;
    pclusteroperator cwf_;
    pclusteroperator rwflow_;
    pclusteroperator cwflow_;
    pclusteroperator rwfup_;
    pclusteroperator cwfup_;
};

// Factory implementation
std::unique_ptr<MatrixInterface> Factory::create(BackendType type, const Config& config)
{
    return std::make_unique<H2LibMatrix>(type, config);
}

} // namespace h2lib
