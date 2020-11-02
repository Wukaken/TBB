#include <iostream>
#include "../ch02.h"

int main(int argc, char *argv[])
{
    const int N = 1000000;
    QSVector v = makeQSData(N);

    warmupTBB();
    double parallel_time = 0.0;
    {
        tbb::tick_count t0 = tbb::tick_count::now();
        parallelQuicksort(v.begin(), v.end());
        parallel_time = (tbb::tick_count::now() - t0).seconds();
        if(!checkIsSorted(v))
            std::cerr << "Error: tbb sorted list out of order" << std::endl;
    }

    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    return 0;
}