#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../ch05.h"
#include "../../utils.h"

struct atom_bin{
    alignas(128) tbb::atomic<int> count;
};

double getParallelForTime(const std::vector<uint8_t>& image, 
                          std::vector<atom_bin, tbb::cache_aligned_allocator<atom_bin>>& hist)
{
    tbb::tick_count t0 = tbb::tick_count::now();
    parallel_for(
        tbb::blocked_range<size_t>(0, image.size()),
        [&](const tbb::blocked_range<size_t>& r){
            std::for_each(
                image.data() + r.begin(), image.data() + r.end(),
                [&](const int i){ hist[i].count++; }
            );
        }
    );
    tbb::tick_count t1 = tbb::tick_count::now();
    double t = (t1 - t0).seconds();
    return t;
}

int main(int argc, char *argv[])
{
    std::vector<uint8_t> image;
    image.reserve(n);
    std::generate_n(
        std::back_inserter(image), n,
        [&]{ return uniform(mte); }
    );

    int nth = 4;
    tbb::task_scheduler_init init(4);

    std::vector<int> hist(num_bins);
    double t_serial = getSerialTime(image, hist);

    std::vector<atom_bin, tbb::cache_aligned_allocator<atom_bin>> hist_p(num_bins);
    double t_parallel = getParallelForTime(image, hist_p);
    
    std::cout << "Serial: " << t_serial << std::endl;
    std::cout << "Parallel: " << t_parallel << std::endl;
    std::cout << "Speed-up: " << t_serial / t_parallel << std::endl;

    for(size_t idx = 0; idx < hist_p.size(); idx++){
        if(hist[idx] != hist_p[idx].count){
            std::cerr << "Index " << idx << " failed: ";
            std::cerr << hist[idx] << " != " << hist_p[idx].count << std::endl;
        }
    }
        
    return 0;
}