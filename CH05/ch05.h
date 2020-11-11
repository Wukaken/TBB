#include <random>
#include <vector>

const long int n = 1000000000;
const int num_bins = 256;

std::random_device seed;
std::mt19937 mte(seed());
std::uniform_int_distribution<> uniform(0, num_bins);

void initImage(std::vector<uint8_t>& image)
{
    image.reserve(n);
    std::generate_n(
        std::back_inserter(image), n,
        [&]{ return uniform(mte); }
    );
}

double getSerialTime(const std::vector<uint8_t>& image, std::vector<int>& hist)
{
    tbb::tick_count t0 = tbb::tick_count::now();
    std::for_each(
        image.cbegin(), image.cend(),
        [&](uint8_t i){ hist[i]++; }
    );
    tbb::tick_count t1 = tbb::tick_count::now();
    double t = (t1 - t0).seconds();
    return t;
}