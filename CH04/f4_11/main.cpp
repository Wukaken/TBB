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
void fig_4_11()
{
    const int num_trials = 1000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> a(n, 1.0);
    for(int t = 0; t < num_trials; t++){
        warmupTBB();
        t0 = tbb::tick_count::now();
        float sum = std::reduce(pstl::execution::par, a.begin(), a.end());
        accumulateTime(t0, 4);
        sum += std::reduce(pstl::execution::par_unseq, a.begin(), a.end());
        accumulateTime(t0, 5);
#pragma novector
        for(int i = 0; i < n; i++)
            sum += a[i];
        accumulateTime(t0, 0);
        sum += std::reduce(pstl::execution::seq, a.begin(), a.end());
        accumulateTime(t0, 2);
        sum += std::reduce(pstl::execution::unseq, a.begin(), a.end());
        accumulateTime(t0, 3);
        if(sum != 5 * n)
            std::cout << "ERROR: sum is not correct " << sum << " != " << num_versions * n << std::endl;
    }
    dumpTimes();
}

void fig_4_11_with_lambda()
{
    const int num_trials = 1000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> a(n, 1.0);
    for(int t = 0; t < num_trials; t++){
        warmupTBB();
        t0 = tbb::tick_count::now();
        auto sum = std::reduce(
            pstl::execution::par,
            /* in1 range */ a.begin(), a.end(),
            /* init */ 0.0,
            [](float ae, float be) -> float{
                return ae + be;
            }
        );
        accumulateTime(t0, 4);
        sum += std::reduce(
            pstl::execution::par_unseq,
            a.begin(), a.end(),
            0.0,
            [](float ae, float be) -> float{
                return ae + be;
            }
        );
        accumulateTime(t0, 5);
#pragma novector
        for(int i = 0; i < n; i++)
            sum += a[i];
        accumulateTime(t0, 0);
        sum += std::reduce(
            pstl::execution::seq,
            a.begin(), a.end(),
            0.0,
            [](float ae, float be) -> float{
                return ae + be;
            }
        );
        accumulateTime(t0, 2);
        sum += std::reduce(
            pstl::execution::unseq,
            a.begin(), a.end(),
            0.0,
            [](float ae, float be) -> float{
                return ae + be;
            }
        );
        accumulateTime(t0, 3);
        if(sum != 5 * n)
            std::cout << "ERROR: sum is not correct" << sum << " != " << num_versions * n << std::endl;
    }
    dumpTimes();
}

int main(int argc, char *argv[])
{
    double total_time = 0.0;

    tbb::tick_count t0 = tbb::tick_count::now();
    fig_4_11();
    fig_4_11_with_lambda();
    total_time = (tbb::tick_count::now() - t0).seconds();
    
    std::cout << "total_time == " << total_time << " seconds" << std::endl;
    return 0;
}