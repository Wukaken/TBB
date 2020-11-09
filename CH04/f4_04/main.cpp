#include <iostream>
#include <vector>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../../utils.h"

inline void f(float& i){ i += 1; }

void accumulateTime(tbb::tick_count& t0, int version);
void validateResults(int num_trails, const std::vector<float>& v);
void dumpTimes();    

void fig_4_4()
{
    const int num_trials = 10000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> v(n, 0);
    for(int t = 0; t < num_trials; t++){
        warmupTBB();
        t0 = tbb::tick_count::now();
        std::for_each(pstl::execution::par, v.begin(), v.end(),
            [](float& i){
                f(i);
            }
        );
        accumulateTime(t0, 4);
        std::for_each(pstl::execution::par_unseq, v.begin(), v.end(),
            [](float& i){
                f(i);
            }
        );
        accumulateTime(t0, 5);
        tbb::parallel_for(0, (int)v.size(), 
            [&v](int i){
                f(v[i]);
            }
        );
        accumulateTime(t0, 6);
#pragma novector
        for(auto &i : v){
            f(i);
        }
        accumulateTime(t0, 0);
        std::for_each(v.begin(), v.end(),
            [](float& i){
                f(i);
            }
        );
        accumulateTime(t0, 1);
        std::for_each(pstl::execution::seq, v.begin(), v.end(),
            [](float& i){
                f(i);
            }
        );
        accumulateTime(t0, 2);
        std::for_each(pstl::execution::unseq, v.begin(), v.end(),
            [](float& i){
                f(i);
            }
        );
        accumulateTime(t0, 3);
    }
    validateResults(num_trials, v);
}

const int num_versions = 7;
double total_times[num_versions] = {0, 0, 0, 0, 0, 0, 0};

void accumulateTime(tbb::tick_count& t0, int version)
{
    if(version >= 0)   {
        double elapsed_time = (tbb::tick_count::now() - t0).seconds();
        total_times[version] += elapsed_time;
        t0 = tbb::tick_count::now();
    }
}

void dumpTimes()
{
    const char *versions[num_versions] = {
        "for", "none", "seq", "unseq", "par", "par_unseq", "pfor"
    };
    for(int i = 0; i < num_versions; i++)
        std::cout << versions[i] << ", " << total_times[i] << std::endl;
}

void validateResults(int num_trails, const std::vector<float>& v)
{
    float r = num_trails * num_versions;
    for(auto& i : v){
        if(r != i){
            std::cout << "ERROR: results did not match" << std::endl;
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    warmupTBB();
    
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_4_4();
    double total_time = (tbb::tick_count::now() - t0).seconds();
    dumpTimes();
    std::cout << "total_time == " << total_time << " seconds" << std::endl;
    return 0;
}