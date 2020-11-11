#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../ch05.h"
#include "../../utils.h"

int main(int argc, char *argv[])
{
    std::vector<uint8_t> image;
    initImage(image);

    int nth = 4;
    tbb::task_scheduler_init init(4);

    std::vector<int> hist(num_bins);
    tbb::tick_count t0 = tbb::tick_count::now();
    std::for_each(
        image.begin(), image.end(),
        [&hist](uint8_t i){
            hist[i]++;
        }
    );
    tbb::tick_count t1 = tbb::tick_count::now();
    double t_serial = (t1 - t0).seconds();

    std::vector<int> hist_p(num_bins);
    t0 = tbb::tick_count::now();
    parallel_for(
        tbb::blocked_range<size_t>(0, image.size()),
        [&](const tbb::blocked_range<size_t>& r){
            for(size_t i = r.begin(); i < r.end(); i++)
                hist_p[image[i]]++;
        }
    );
    t1 = tbb::tick_count::now();
    double t_parallel = (t1 - t0).seconds();

    std::cout << "Serial: " << t_serial << std::endl;
    std::cout << "Parallel: " << t_parallel << std::endl;
    std::cout << "Speed-up: " << t_serial / t_parallel << std::endl;

    if(hist != hist_p)
        std::cerr << "Parallel computation failed!!" << std::endl;
    return 0;
}