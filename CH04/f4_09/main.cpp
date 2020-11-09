#include <iostream>

#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../ch04.h"
#include "../../utils.h"

inline void fig_4_9(float *a, float *b, float *c)
{
    const int M = 1024;
    std::for_each(
        pstl::execution::par_unseq,
        tbb::counting_iterator<int>(0),
        tbb::counting_iterator<int>(M),
        [&a, &b, &c, M](int i){
            for(int j = 0; j < M; j++){
                int c_index = i * M + j;
                for(int k = 0; k < M; k++)
                    c[c_index] += a[i * M + k] * b[k * M + j];
            }
        }
    );
}

void run_fig_4_9()
{
    const int M = 1024;
    const int MxM = M * M;
    float *a = new float[MxM];
    float *b = new float[MxM];
    float *c = new float[MxM];
    float *c_expect = new float[MxM];

    std::fill(a, a + MxM, 1.0);
    std::fill(b, b + MxM, 1.0);
    std::fill(c, c + MxM, 0.0);
    std::fill(c_expect, c_expect + MxM, M);

    warmupTBB();
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_4_9(a, b, c);
    double version_time = (tbb::tick_count::now() - t0).seconds();
    if(!std::equal(c, c + MxM, c_expect))
        std::cerr << "Error: c array does not match expected values for fig_4_9" << std::endl;
    
    std::cout << "time == " << version_time << " seconds for par_unseq" << std::endl;
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] c_expect;
}

template <int M, typename Policy>
inline void mxm_template(const Policy& p, float* a, float* b, float* c)
{
    std::for_each(
        p, 
        tbb::counting_iterator<int>(0),
        tbb::counting_iterator<int>(M),
        [&a, &b, &c](int i){
#pragma novector
            for(int j = 0; j < M; j++){
                int c_index = i * M + j;
#pragma novector
                for(int k = 0; k < M; k++)
                    c[c_index] += a[i * M + k] * b[k * M + j];
            }
        }
    );
}

template <typename Policy>
void run_mxm_template(const Policy& p, const std::string& name)
{
    const int M = 1024;
    const int MxM = M * M;
    float *a = new float[MxM];
    float *b = new float[MxM];
    float *c = new float[MxM];
    float *c_expected = new float[MxM];

    std::fill(a, a + MxM, 1.0);
    std::fill(b, b + MxM, 1.0);
    std::fill(c, c + MxM, 0.0);
    std::fill(c_expected, c_expected + MxM, M);

    warmupTBB();
    tbb::tick_count t0 = tbb::tick_count::now();
    mxm_template<1024>(p, a, b, c);
    double version_time = (tbb::tick_count::now() - t0).seconds();
    if(!std::equal(c, c + MxM, c_expected))
        std::cerr << "Error: c array does not match expected values for " << name << std::endl;

    std::cout << "time == " << version_time << " seconds for " << name << std::endl;
}

int main(int argc, char *argv[])
{
    run_mxm_template(pstl::execution::seq, "seq");
    run_mxm_template(pstl::execution::unseq, "unseq");
    warmupTBB();
    run_mxm_template(pstl::execution::par, "par");
    warmupTBB();
    run_fig_4_9();
    
    std::cout << "Done" << std::endl;
    return 0;
}