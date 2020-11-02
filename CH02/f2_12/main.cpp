#define NOMINMAX

#include <cmath>
#include <limits>
#include <iostream>
#include <tbb/tbb.h>
#include "../ch02.h"

double fig_2_12(int num_intervals){
    double dx = 1.0 / num_intervals;
    double sum = tbb::parallel_reduce(
        tbb::blocked_range<int>(0, num_intervals),
        0.0,
        [=](const tbb::blocked_range<int>& r, double init) -> double {
            for(int i = r.begin(); i != r.end(); i++){
                double x = (i + 0.5) * dx;
                double h = std::sqrt(1 - x * x);
                init += h * dx;
            }
            return init;
        },
        [](double x, double y) -> double {
            return x + y;
        }
    );

    double pi = 4 * sum;
    return pi;
}

int main(int argc, char* argv[])
{
    const int num_intervals = std::numeric_limits<int>::max();
    double parallel_time = 0.0;
    warmupTBB();

    tbb::tick_count t0 = tbb::tick_count::now();
    double pi = fig_2_12(num_intervals);
    parallel_time = (tbb::tick_count::now() - t0).seconds();

    std::cout << "parallel_pi == " << pi << std::endl;
    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;

    return 0;
}