#define NOMINMAX

#include <iostream>
#include <tbb/tbb.h>
#include "../ch02.h"
#include "../pngUtils.h"

void fig_2_30(int num_tokens)
{
    using Image = PNGImage;
    using ImagePair = std::pair<PNGImage, PNGImage>;
    tbb::parallel_pipeline(
        num_tokens,
        tbb::make_filter<void, Image>(
            tbb::filter::serial_in_order,
            [&] (tbb::flow_control& fc) -> Image{
                if(uint64_t frame_number = getNextFrameNumber())
                    return getLeftImage(frame_number);
                else{
                    fc.stop();
                    return Image{};
                }
            }
        ) &
        tbb::make_filter<Image, ImagePair>(
            tbb::filter::serial_in_order,
            [&](Image left) -> ImagePair{
                return ImagePair(left, getRightImage(left.frameNumber));
            }
        ) &
        tbb::make_filter<ImagePair, ImagePair>(
            tbb::filter::parallel,
            [&](ImagePair p) ->ImagePair{
                increasePNGChannel(p.first, Image::redOffset, 10);
                return p;                
            }
        ) & 
        tbb::make_filter<ImagePair, ImagePair>(
            tbb::filter::parallel,
            [&](ImagePair p) ->ImagePair{
                increasePNGChannel(p.second, Image::blueOffset, 10);
                return p;                
            }
        ) & 
        tbb::make_filter<ImagePair, Image>(
            tbb::filter::parallel,
            [&](ImagePair p) ->Image{
                mergePNGImages(p.second, p.first);
                return p.second;
            }
        ) &
        tbb::make_filter<Image, void>(
            tbb::filter::parallel,
            [&](Image img){
                img.write();
            }
        )
    );
    while(uint64_t frameNumber = getNextFrameNumber()){
        auto left = getLeftImage(frameNumber);
        auto right = getRightImage(frameNumber);
        increasePNGChannel(left, PNGImage::redOffset, 10);
        increasePNGChannel(right, PNGImage::blueOffset, 10);
        mergePNGImages(right, left);
        right.write();
    }
}

int main(int argc, char* argv[])
{
    int num_tokens = tbb::task_scheduler_init::default_num_threads();
    int num_images = 3;
    initStereo3D(num_images);

    warmupTBB();
    double parallel_time = 0.0;
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_30(num_tokens);
    parallel_time = (tbb::tick_count::now() - t0).seconds();

    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    return 0;
}