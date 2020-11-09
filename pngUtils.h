#define NOMINMAX

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "lodepng.h"

class PNGImage{
public:
    uint64_t frameNumber = -1;
    unsigned int width = 0, height = 0;
    std::shared_ptr<std::vector<unsigned char>> buffer;
    static const int numChannels = 4;
    static const int redOffset = 0;
    static const int greenOffset = 1;
    static const int blueOffset = 2;

    PNGImage() {}
    PNGImage(uint64_t frame_number, const std::string& file_name);
    PNGImage(const PNGImage& p);
    virtual ~PNGImage() {}
    void write() const;
};

int getNextFrameNumber();
PNGImage getLeftImage(uint64_t frameNumber);
PNGImage getRightImage(uint64_t frameNumber);
void increasePNGChannel(PNGImage& image, int channel_offset, int increase);
void mergePNGImages(PNGImage& right, const PNGImage& left);

PNGImage::PNGImage(uint64_t frame_number, const std::string& file_name) :
    frameNumber(frame_number),
    buffer(std::make_shared<std::vector<unsigned char> > ())
{
    if(lodepng::decode(*buffer, width, height, file_name)){
        std::cerr << "Error: could not read PNG file!" << std::endl;
        width = height = 0;
    }
}

PNGImage::PNGImage(const PNGImage& p) : 
    frameNumber(p.frameNumber), width(p.width), height(p.height),
    buffer(p.buffer)
{}

void PNGImage::write() const 
{
    std::string file_name = std::string("out") + std::to_string(frameNumber) + ".png";
    if(lodepng::encode(file_name, *buffer, width, height))
        std::cerr << "Error: could not write PNG file!" << std::endl;
}

static int stereo3DFrameCounter = 0;
static int stereo3DNumImages = 0;

void initStereo3D(int num_images)
{
    stereo3DFrameCounter = 0;
    stereo3DNumImages = num_images;
}

int getNextFrameNumber()
{
    if(stereo3DFrameCounter < stereo3DNumImages)
        return ++stereo3DFrameCounter;
    else
        return 0;
}

PNGImage getLeftImage(uint64_t frameNumber)
{
    return PNGImage(frameNumber, "input1.png");
}

PNGImage getRightImage(uint64_t frameNumber)
{
    return PNGImage(frameNumber, "input2.png");
}

/* PNG structure {RGBA RGBA RGBA...}(len(width))*/
void increasePNGChannel(PNGImage& image, int channel_offset, int increase)
{
    const int height_base = PNGImage::numChannels * image.width;
    std::vector<unsigned char>& buffer = *image.buffer;

    for(unsigned int y = 0; y < image.height; y++){
        const int height_offset = height_base * y;
        // 4 * 960 * y == width, width, width, width * y
        for(unsigned int x = 0; x < image.width; x++){
            int pixel_offset = height_offset + PNGImage::numChannels * x + channel_offset;
            buffer[pixel_offset] = static_cast<uint8_t>(std::min(buffer[pixel_offset] + increase, 255));
        }
    }
}

void mergePNGImages(PNGImage& right, const PNGImage& left)
{
    const int channels_per_pixel = PNGImage::numChannels;
    const int height_base = channels_per_pixel * right.width;
    std::vector<unsigned char>& left_buffer = *left.buffer;
    std::vector<unsigned char>& right_buffer = *right.buffer;

    for(unsigned int y = 0; y < right.height; y++){
        const int height_offset = height_base * y;
        for(unsigned int x = 0; x < right.width; x++){
            const int pixel_offset = height_offset + channels_per_pixel * x;
            const int red_index = pixel_offset + PNGImage::redOffset;
            right_buffer[red_index] = left_buffer[red_index];
        }
    }
}