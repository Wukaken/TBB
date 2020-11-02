#include "ch01.h"
#include "baseFunc.h"

ImagePtr applyGamma(ImagePtr image_ptr, double gamma){
    auto output_image_ptr = std::make_shared<ch01::Image>(image_ptr->name() + "_gamma",
        ch01::IMAGE_WIDTH, ch01::IMAGE_HEIGHT);
    auto in_rows = image_ptr->rows();
    auto out_rows = output_image_ptr->rows();
    const int height = in_rows.size();
    const int width = in_rows[1] - in_rows[0];
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            const ch01::Image::Pixel& p = in_rows[i][j];
            double v = 0.3 * p.bgra[2] + 0.59 * p.bgra[1] + 0.11 * p.bgra[0];   
            double res = pow(v, gamma);
            if(res > ch01::MAX_BGR_VALUE)
                res = ch01::MAX_BGR_VALUE;

            out_rows[i][j] = ch01::Image::Pixel(res, res, res);
        }
    }
    return output_image_ptr;
}

ImagePtr applyTint(ImagePtr image_ptr, const double *tints){
    auto output_image_ptr = std::make_shared<ch01::Image>(image_ptr->name() + "_tinted",
        ch01::IMAGE_WIDTH, ch01::IMAGE_HEIGHT);
    auto in_rows = image_ptr->rows();
    auto out_rows = output_image_ptr->rows();
    int height = in_rows.size();
    const int width = in_rows[1] - in_rows[0];

    for(int i = 0; i < height; i++){
        for(int j = 0; j < height; j++){
            const ch01::Image::Pixel& p = in_rows[i][j];
            std::uint8_t b = (double)p.bgra[0] + (ch01::MAX_BGR_VALUE - p.bgra[0]) * tints[0];
            std::uint8_t g = (double)p.bgra[1] + (ch01::MAX_BGR_VALUE - p.bgra[1]) * tints[1];
            std::uint8_t r = (double)p.bgra[2] + (ch01::MAX_BGR_VALUE - p.bgra[2]) * tints[2];
            out_rows[i][j] = ch01::Image::Pixel(
                (b > ch01::MAX_BGR_VALUE) ? ch01::MAX_BGR_VALUE : b,
                (g > ch01::MAX_BGR_VALUE) ? ch01::MAX_BGR_VALUE : g,
                (r > ch01::MAX_BGR_VALUE) ? ch01::MAX_BGR_VALUE : r
            );
        }
    }
    return output_image_ptr;
}

void writeImage(ImagePtr image_ptr){
    image_ptr->write((image_ptr->name() + ".bmp").c_str());
}