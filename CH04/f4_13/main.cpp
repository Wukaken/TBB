#include <iostream>
#include <vector>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include <pstl/numeric>
#include "../ch04.h"
#include "../../utils.h"

const int num_intervals = 1 << 22;

float fig_4_13()
{
    constexpr const float dx = 1.0 / num_intervals;
    float sum = std::transform_reduce(
        pstl::execution::par_unseq,
        tbb::counting_iterator<int>(0),
        tbb::counting_iterator<int>(num_intervals),
        0.0,
        [](float x, float y) -> float{
            return x + y;
        },
        [=](int i) -> float{
            float x = (i + 0.5) * dx;
            float h = sqrt(1 - x * x);
            return h * dx;
        }
    );
    return 4 * sum;
}

void run_fig_4_13()
{
    warmupTBB();
    tbb::tick_count t0 = tbb::tick_count::now();
    auto pi = fig_4_13();
    double version_time = (tbb::tick_count::now() - t0).seconds();
    std::cout << "pi with par_unseq == " << pi << std::endl;
    std::cout << "time == " << version_time << " seconds for par_unseq" << std::endl;
}

template <typename Policy>
float pi_template(const Policy& p)
{
    constexpr const float dx = 1.0 / num_intervals;
    float sum = std::transform_reduce(
        p, 
        tbb::counting_iterator<int>(0), 
        tbb::counting_iterator<int>(num_intervals), 0.0,
        [](float x, float y) -> float{
            return x + y;
        },
        [=](int i) -> float {
            float x = (i + 0.5) * dx;
            float h = sqrt(1 - x * x);
            return h * dx;
        }
    );
    return 4 * sum;
}

template <typename Policy>
void run_pi_template(const Policy& p, const std::string& name)
{
    warmupTBB();
    tbb::tick_count t0 = tbb::tick_count::now();
    auto pi = pi_template(p);
    double version_time = (tbb::tick_count::now() - t0).seconds();
    std::cout << "pi with " << name << " == " << pi << std::endl;
    std::cout << "time == " << version_time << " seconds for " << name << std::endl;
}

int main(int argc, char *argv[])
{
    run_pi_template(pstl::execution::seq, "seq");
    run_pi_template(pstl::execution::unseq, "unseq");
    run_pi_template(pstl::execution::par, "par");
    run_pi_template(pstl::execution::par_unseq, "par_unseq");
    run_fig_4_13();
    std::cout << "Done." << std::endl;
    return 0;
}