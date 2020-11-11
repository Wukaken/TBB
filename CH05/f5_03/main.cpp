#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../../utils.h"


int main(int argc, char *argv[])
{
    constexpr long int n = 10000000;
    constexpr int num_bins = 256;

    std::random_device seed;
    std::mt19937 mte{seed()};
    std::uniform_int_distribution<> uniform(0, num_bins);

    std::vector<uint8_t> image;
    image.reserve(n);
    std::generate_n(
        std::back_inserter(image), n,
        [&]{ return uniform(mte); }
    );

    std::vector<int> hist(num_bins);

    tbb::tick_count t0 = tbb::tick_count::now();
    std::for_each(
        image.begin(), image.end(),
        [&hist](uint8_t i){
            hist[i]++;
        }
    );
    double total_time = (tbb::tick_count::now() - t0).seconds();

    std::cout << "Serial time: " << total_time << " seconds" << std::endl;
    return 0;
}