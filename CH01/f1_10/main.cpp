#include <iostream>
#include <vector>
#include <tbb/tbb.h>
#include "ch01.h"
#include "baseFunc.h"

void fig_1_10(const std::vector<ImagePtr>& image_vector)
{
    const double tint_array[] = {0.75, 0, 0};
    tbb::flow::graph g;

    int i = 0;
    tbb::flow::source_node<ImagePtr> src(g,
        [&i, &image_vector] (ImagePtr& out) -> bool {
            if(i < image_vector.size()){
                out = image_vector[i++];
                return true;
            }
            else{
                return false;
            }
        }, 
        false
    );

    tbb::flow::function_node<ImagePtr, ImagePtr> gamma(g,
        tbb::flow::unlimited, [] (ImagePtr img) -> ImagePtr{
            return applyGamma(img, 1.4);
        }
    );

    tbb::flow::function_node<ImagePtr, ImagePtr> tint(g,
        tbb::flow::unlimited, [tint_array] (ImagePtr img) -> ImagePtr{
            return applyTint(img, tint_array);
        }
    );

    tbb::flow::function_node<ImagePtr> write(g, 
        tbb::flow::unlimited, [] (ImagePtr img) {
            writeImage(img);
        }
    );

    tbb::flow::make_edge(src, gamma);
    tbb::flow::make_edge(gamma, tint);
    tbb::flow::make_edge(tint, write);
    src.activate();
    g.wait_for_all();
}

int main(int argc, char* argv[])
{
    std::vector<ImagePtr> image_vector;
    for(int i = 2000; i < 20000000; i *= 10)
        image_vector.push_back(ch01::makeFractalImage(i));

    // warmup the scheduler
    tbb::parallel_for(
        0, 
        tbb::task_scheduler_init::default_num_threads(), 
        [](int) {
            tbb::tick_count t0 = tbb::tick_count::now();
            while((tbb::tick_count::now() - t0).seconds() < 0.01);
        }
    );

    tbb::tick_count t0 = tbb::tick_count::now();
    fig_1_10(image_vector);
    std::cout << "Time : " << (tbb::tick_count::now() - t0).seconds()
              << " seconds" << std::endl;
    return 0;
}