#include <iostream>
#include <vector>
#include <tbb/tbb.h>

const int num_versions = 7;
double total_times[num_versions] = {0, 0, 0, 0, 0, 0, 0};

void accumulateTime(tbb::tick_count& t0, int version)
{
    if(version >= 0)   {
        double elapsed_time = (tbb::tick_count::now() - t0).seconds();
        total_times[version] += elapsed_time;
        t0 = tbb::tick_count::now();
    }
}

void dumpTimes()
{
    const char *versions[num_versions] = {
        "for", "none", "seq", "unseq", "par", "par_unseq", "pfor"
    };
    for(int i = 0; i < num_versions; i++)
        std::cout << versions[i] << ", " << total_times[i] << std::endl;
}

