#define NOMINMAX

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <tbb/tbb.h>
#include "../../lodepng.h"
#include "../../pngUtils.h"
#include "../../utils.h"

void fig_3_10()
{
    using Image = PNGImage;

    tbb::flow::graph g;

    tbb::flow::source_node<uint64_t> frame_no_node(
        g,
        [](uint64_t& frame_number) -> bool {
            if((frame_number = getNextFrameNumber()))
                return true;
            else
                return false;
        },
        false /* start inactive */
    );
    tbb::flow::function_node<uint64_t, Image> get_left_node{
        g, 
        tbb::flow::unlimited,
        [](uint64_t frame_number) -> Image{
            return getLeftImage(frame_number);
        }
    };
    tbb::flow::function_node<uint64_t, Image> get_right_node{
        g, 
        tbb::flow::unlimited,
        [](uint64_t frame_number) -> Image{
            return getRightImage(frame_number);
        }
    };
    tbb::flow::function_node<Image, Image> increase_left_node{
        g,
        tbb::flow::unlimited,
        [](Image left) -> Image{
            increasePNGChannel(left, Image::redOffset, 10);
            return left;
        }
    };
    tbb::flow::function_node<Image, Image> increase_right_node{
        g, 
        tbb::flow::unlimited,
        [](Image right) -> Image{
            increasePNGChannel(right, Image::blueOffset, 10);
            return right;
        }
    };
    tbb::flow::join_node<std::tuple<Image, Image>, tbb::flow::tag_matching>
        join_images_node(g, [](Image left) {return left.frameNumber; }, 
                            [](Image right) {return right.frameNumber; } );
    tbb::flow::function_node<std::tuple<Image, Image>, Image> merge_images_node{
        g,
        tbb::flow::unlimited,
        [](std::tuple<Image, Image> t) -> Image{
            auto &l = std::get<0>(t);
            auto &r = std::get<1>(t);
            mergePNGImages(r, l);
            return r;
        }
    };
    tbb::flow::function_node<Image> write_node{
        g,
        tbb::flow::unlimited,
        [](Image img){
            img.write();
        }
    };
    
    tbb::flow::make_edge(frame_no_node, get_left_node);
    tbb::flow::make_edge(frame_no_node, get_right_node);
    tbb::flow::make_edge(get_left_node, increase_left_node);
    tbb::flow::make_edge(get_right_node, increase_right_node);
    tbb::flow::make_edge(increase_left_node,
                         tbb::flow::input_port<0>(join_images_node));
    tbb::flow::make_edge(increase_right_node,
                         tbb::flow::input_port<1>(join_images_node));
    tbb::flow::make_edge(join_images_node, merge_images_node);
    tbb::flow::make_edge(merge_images_node, write_node);

    frame_no_node.activate();
    g.wait_for_all();
}

int main(int argc, char *argv[])
{
    int num_images = 3;
    initStereo3D(num_images);

    warmupTBB();
    double parallel_time = 0.0;
    {
        tbb::tick_count t0 = tbb::tick_count::now();
        fig_3_10();
        parallel_time = (tbb::tick_count::now() - t0).seconds();
    }

    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    return 0;
}
