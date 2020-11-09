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
void fig_4_8a()
{
    const int num_trials = 1000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> a(n, 1.0), b(n, 3.0);
    for(int t = 0; t < num_trials; t++){
        warmupTBB();
        t0 = tbb::tick_count::now();
        std::for_each(
            pstl::execution::par, 
            tbb::counting_iterator<int>(0),
            tbb::counting_iterator<int>(n),
            [&a, &b](int i){
                a[i] = a[i] + b[i] * b[i];
            }
        );
        accumulateTime(t0, 4);
        std::for_each(
            pstl::execution::par_unseq,
            tbb::counting_iterator<int>(0),
            tbb::counting_iterator<int>(n),
            [&a, &b](int i){
                a[i] = a[i] + b[i] * b[i];
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
        for(int i = 0; i < n; i++){
            a[i] = a[i] + b[i] * b[i];
        }
        accumulateTime(t0, 0);
        std::for_each(
            tbb::counting_iterator<int>(0),
            tbb::counting_iterator<int>(n),
            [&a, &b](int i){
                a[i] = a[i] + b[i] * b[i];
            }
        );
        accumulateTime(t0, 1);
        std::for_each(
            pstl::execution::seq, 
            tbb::counting_iterator<int>(0),
            tbb::counting_iterator<int>(n),
            [&a, &b](int i){
                a[i] = a[i] + b[i] * b[i];
            }
        );
        accumulateTime(t0, 2);
        std::for_each(
            pstl::execution::unseq,
            tbb::counting_iterator<int>(0),
            tbb::counting_iterator<int>(n),
            [&a, &b](int i){
                a[i] = a[i] + b[i] * b[i];
            }
        );
        accumulateTime(t0, 3);
    }
    validateResults(num_trials, a);
}

void fig_4_8b()
{
    const int num_trials = 1000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> a(n, 1.0), b(n, 3.0);
    for(int t = 0; t < num_trials; t++){
        auto begin = tbb::make_zip_iterator(a.begin(), b.begin());
        auto end = tbb::make_zip_iterator(a.end(), b.end());

        warmupTBB();
        t0 = tbb::tick_count::now();
        std::for_each(
            pstl::execution::par, begin, end,
            [](const std::tuple<float&, float&>& v){
                float& a = std::get<0>(v);
                float b = std::get<1>(v);
                a = a + b * b;
            }
        );
        accumulateTime(t0, 4);
        std::for_each(
            pstl::execution::par_unseq, begin, end,
            [](const std::tuple<float&, float&>& v){
                float& a = std::get<0>(v);
                float b = std::get<1>(v);
                a = a + b * b;
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
        std::for_each(
            begin, end,
            [](const std::tuple<float&, float&>& v){
                float& a = std::get<0>(v);
                float b = std::get<1>(v);
                a = a + b * b;
            }
        );
        accumulateTime(t0, 1);
        std::for_each(
            pstl::execution::seq,
            begin, end,
            [](const std::tuple<float&, float&>& v){
                float& a = std::get<0>(v);
                float b = std::get<1>(v);
                a = a + b * b;
            }
        );
        accumulateTime(t0, 2);
        std::for_each(
            pstl::execution::unseq,
            begin, end,
            [](const std::tuple<float&, float&>& v){
                float& a = std::get<0>(v);
                float b = std::get<1>(v);
                a = a + b * b;
            }
        );
        accumulateTime(t0, 2);
    }
    validateResults(num_trials, a);
}

void fig_4_8c()
{
    const int num_trials = 1000;
    const int n = 65536;
    tbb::tick_count t0;

    std::vector<float> a(n, 1.0), b(n, 3.0);
    for(int t = 0; t < num_trials; t++){
        auto zbegin = tbb::make_zip_iterator(a.begin(), b.begin());
        auto zend = tbb::make_zip_iterator(a.end(), b.end());

        auto square_b = [](const std::tuple<float&, float&>& v){
            float b = std::get<1>(v);
            return std::tuple<float&, float>(std::get<0>(v), b * b);
        };
        auto begin = tbb::make_transform_iterator(zbegin, square_b);
        auto end = tbb::make_transform_iterator(zend, square_b);

        warmupTBB();
        t0 = tbb::tick_count::now();
        std::for_each(
            pstl::execution::par, begin, end,
            [](const std::tuple<float&, float>& v){
                float& a = std::get<0>(v);
                a = a + std::get<1>(v);
            }
        );
        accumulateTime(t0, 4);
        std::for_each(
            pstl::execution::par_unseq, begin, end,
            [](const std::tuple<float&, float>& v){
                float& a = std::get<0>(v);
                a = a + std::get<1>(v);
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
        for(int i = 0; i < n; i++){
            a[i] = a[i] + b[i] * b[i];
        }
        accumulateTime(t0, 0);
        std::for_each(
            begin, end,
            [](const std::tuple<float&, float>& v){
                float& a = std::get<0>(v);
                a = a + std::get<1>(v);
            }
        );
        accumulateTime(t0, 1);
        std::for_each(
            pstl::execution::seq, begin, end,
            [](const std::tuple<float&, float>& v){
                float& a = std::get<0>(v);
                a = a + std::get<1>(v);
            }
        );
        accumulateTime(t0, 2);
        std::for_each(
            pstl::execution::unseq, begin, end,
            [](const std::tuple<float&, float>& v){
                float& a = std::get<0>(v);
                a = a + std::get<1>(v);
            }
        );
        accumulateTime(t0, 3);
    }
    validateResults(num_trials, a);
}

int main(int argc, char *argv[])
{
    double total_time = 0.0;
    warmupTBB();
    
    tbb::tick_count t0 = tbb::tick_count::now();
    std::cout << "fig_4_8(a)" << std::endl
    fig_4_8a();
    dumpTimes();
    std::cout << std::endl;
    std::cout << "fig_4_8(b)" << std::endl;
    fig_4_8b();
    dumpTimes();
    std::cout << std::endl;
    std::cout << "fig_4_8(c)" << std::endl;
    fig_4_8c();
    dumpTimes();
    std::cout << std::endl;
    total_time = (tbb::tick_count::now() - t0).seconds();
    
    std::cout << "total_time == " << total_time << " seconds" << std::endl;
    return 0;
}