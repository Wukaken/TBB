#include <iostream>
#include <vector>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../ch04.h"
#include "../../utils.h"

void validateResults(int num_trails, const std::vector<float>& a)
{
    float r = num_trails * num_versions * 9 + 1;
    for(auto& i : a){
        if(r != i){
            std::cout << "ERROR: results did not match" << r << " != " << i << std::endl;
            return;
        }
    }
}
// a the fastest
void fig_4_10()
{
    const int num_trials = 1000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> a(n, 1.0), b(n, 3.0);
    for(int t = 0; t < num_trials; t++){
        warmupTBB();
        t0 = tbb::tick_count::now();
        std::transform(
            pstl::execution::par, 
            /* in1 range */ a.begin(), a.end(),
            /* in2 first */ b.begin(),
            /* out first */ a.begin(),
            [](float ae, float be) -> float{
                return ae + be * be;
            }
        );
        accumulateTime(t0, 4);
        std::transform(
            pstl::execution::par_unseq, 
            /* in1 range */ a.begin(), a.end(),
            /* in2 first */ b.begin(),
            /* out first */ a.begin(),
            [](float ae, float be) -> float{
                return ae + be * be;
            }
        );
        accumulateTime(t0, 5);
        tbb::parallel_for(
            0, n,
            [&a, &b](int i){
                a[i] = a[i] + b[i] * b[i];
            }
        );
        accumulateTime(t0, 6);
#pragma novector
        for(int i = 0; i < n; i++)
            a[i] = a[i] + b[i] * b[i];
        accumulateTime(t0, 0);
        std::transform(
            a.begin(), a.end(),
            b.begin(), a.begin(),
            [](float ae, float be) -> float {
                return ae + be * be;
            }
        );
        accumulateTime(t0, 1);
        std::transform(
            pstl::execution::seq,
            a.begin(), a.end(),
            b.begin(), a.begin(),
            [](float ae, float be) -> float {
                return ae + be * be;
            }
        );
        accumulateTime(t0, 2);
        std::transform(
            pstl::execution::unseq,
            a.begin(), a.end(),
            b.begin(), a.begin(),
            [](float ae, float be) -> float {
                return ae + be * be;
            }
        );
        accumulateTime(t0, 3);
    }
    validateResults(num_trials, a);
    dumpTimes();
}

int main(int argc, char *argv[])
{
    double total_time = 0.0;

    tbb::tick_count t0 = tbb::tick_count::now();
    fig_4_10();
    total_time = (tbb::tick_count::now() - t0).seconds();
    
    std::cout << "total_time == " << total_time << " seconds" << std::endl;
    return 0;
}