#include <iostream>
#include <vector>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include <pstl/numeric>
#include "../ch04.h"
#include "../../utils.h"

void validateResults(int num_trails, const std::vector<float>& a)
{
    float r = num_trails * num_versions;
    for(auto& i : a){
        if(r != i){
            
            return;
        }
    }
}
// a the fastest
void fig_4_12_a()
{
    const int num_trials = 1000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> a(n, 1.0), b(n, 1.0);
    for(int t = 0; t < num_trials; t++){
        warmupTBB();
        t0 = tbb::tick_count::now();
        auto v = std::transform_reduce(
            pstl::execution::par, 
            /* in1 range */ a.begin(), a.end(),
            /* in2 range */ b.begin(), 
            /* init */ 0.0,
            /* op1, the reduce */
            [](float ae, float be) -> float {
                return ae + be;
            },
            /* op2 the transform */
            [](float ae, float be) -> float {
                return ae * be;
            }
        );
        accumulateTime(t0, 4);
        v += std::transform_reduce(
            pstl::execution::par_unseq, a.begin(), a.end(),
            b.begin(), 0.0, 
            [](float ae, float be) -> float {
                return ae + be;
            },
            [](float ae, float be) -> float {
                return ae * be;
            }
        );
        accumulateTime(t0, 5);
#pragma novector
        for(int i = 0; i < n; i++)
            v += a[i] * b[i];
        accumulateTime(t0, 0);
        v += std::transform_reduce(
            pstl::execution::seq, a.begin(), a.end(),
            b.begin(), 0.0,
            [](float ae, float be) -> float {
                return ae + be;
            },
            [](float ae, float be) -> float {
                return ae * be;
            }
        );
        accumulateTime(t0, 2);
        v += std::transform_reduce(
            pstl::execution::unseq, a.begin(), a.end(),
            b.begin(), 0.0,
            [](float ae, float be) -> float {
                return ae + be;
            },
            [](float ae, float be) -> float {
                return ae * be;
            }
        );
        accumulateTime(t0, 3);
        if(v != 5 * n)
            std::cout << "ERROR: sum is not correct " << v << " != " << num_versions * n << std::endl;
    }
    dumpTimes();
}

void fig_4_12_b()
{
    const int num_trials = 1000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> a(n, 1.0), b(n, 1.0);
    for(int t = 0; t < num_trials; t++){
        warmupTBB();
        t0 = tbb::tick_count::now();
        auto v = std::transform_reduce(
            pstl::execution::par, 
            /* in1 range */ a.begin(), a.end(),
            /* in2 range */ b.begin(), 
            /* init */ 0.0
        );
        accumulateTime(t0, 4);
        v += std::transform_reduce(
            pstl::execution::par_unseq, a.begin(), a.end(),
            b.begin(), 0.0
        );
        accumulateTime(t0, 5);
#pragma novector
        for(int i = 0; i < n; i++)
            v += a[i] * b[i];
        accumulateTime(t0, 0);
        v += std::transform_reduce(
            pstl::execution::seq, a.begin(), a.end(),
            b.begin(), 0.0
        );
        accumulateTime(t0, 2);
        v += std::transform_reduce(
            pstl::execution::unseq, a.begin(), a.end(),
            b.begin(), 0.0
        );
        accumulateTime(t0, 3);
        if(v != 5 * n)
            std::cout << "ERROR: sum is not correct " << v << " != " << num_versions * n << std::endl;
    }
    dumpTimes();
}

int main(int argc, char *argv[])
{
    double total_time = 0.0;

    tbb::tick_count t0 = tbb::tick_count::now();
    fig_4_12_a();
    fig_4_12_b();
    total_time = (tbb::tick_count::now() - t0).seconds();
    
    std::cout << "total_time == " << total_time << " seconds" << std::endl;
    return 0;
}