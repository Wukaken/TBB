#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../ch05.h"
#include "../../utils.h"

std::vector<int> getParallelForTime(const std::vector<uint8_t>& image, 
                                    double& t)
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
    vector_t tem = priv_h.combine(
        [](vector_t a, vector_t b) -> vector_t{
            std::transform(
                a.begin(), a.end(),
                b.begin(), a.begin(),
                std::plus<int>()
            );
            return a;
            //return a;
        }
    );

    tbb::tick_count t1 = tbb::tick_count::now();
    t = (t1 - t0).seconds();
    return tem;
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

    double t_parallel;
    std::vector<int> hist_p = getParallelForTime(image, t_parallel);
    
    std::cout << "Serial: " << t_serial << std::endl;
    std::cout << "Parallel: " << t_parallel << std::endl;
    std::cout << "Speed-up: " << t_serial / t_parallel << std::endl;

    //if(!std::equal(hist.begin(), hist.end(), hist_p.begin()))
    if(hist != hist_p)
        std::cerr << "Parallel computation failed!!" << std::endl;
    //for(int i = 0; i < hist.size(); i++){
      //  if(hist[i] != hist_p[i]){
        //    std::cerr << "id: " << i << " " << hist[i] << " " << hist_p[i] << std::endl;
            //std::cerr << "Parallel computation failed!!" << std::endl;
        //}
    //}            
    return 0;
}