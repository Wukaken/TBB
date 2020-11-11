#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../ch05.h"
#include "../../utils.h"

double getParallelForTime(const std::vector<uint8_t>& image, 
                          std::vector<tbb::atomic<int>>& hist)
{
    using vector_t = std::vector<int>;
    using priv_h_t = tbb::enumerable_thread_specific<vector_t>;
    priv_h_t priv_h(num_bins);
    tbb::tick_count t0 = tbb::tick_count::now();
    parallel_for(
        tbb::blocked_range<size_t>(0, image.size()),
        [&](const tbb::blocked_range<size_t>& r){
            priv_h_t::reference my_hist = priv_h.local();
            for(size_t i = r.begin(); i < r.end(); i++){
                my_hist[image[i]]++;
            }
        }
    );
    //Sequential reduction of the private histograms
    for(auto i : priv_h){
        std::transform(hist.begin(), hist.end(),
                       i.begin(), hist.begin(), 
                       std::plus<int>());
    }
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

    std::vector<tbb::atomic<int>> hist_p(num_bins);
    double t_parallel = getParallelForTime(image, hist_p);
    
    std::cout << "Serial: " << t_serial << std::endl;
    std::cout << "Parallel: " << t_parallel << std::endl;
    std::cout << "Speed-up: " << t_serial / t_parallel << std::endl;

    if(!std::equal(hist.begin(), hist.end(), hist_p.begin()))
        std::cerr << "Parallel computation failed!!" << std::endl;
            
    return 0;
}