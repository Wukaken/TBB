#define NOMINMAX

#include <iostream>
#include <tbb/tbb.h>
#include "../ch02.h"
#include "../pngUtils.h"

void fig_2_28()
{
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
    int num_images = 3;
    initStereo3D(num_images);

    double serial_time = 0.0;
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_28();
    serial_time = (tbb::tick_count::now() - t0).seconds();

    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;
    return 0;
}