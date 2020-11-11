#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <tbb/tbb.h>
#include <pstl/execution>
#include <pstl/algorithm>
#include "../ch05.h"
#include "../../utils.h"

alignas(64) tbb::atomic<uint32_t> v(1);

void fetch_and_triple(tbb::atomic<uint32_t>& v)
{
    uint32_t old_v;
    do{
        old_v = v;
    }while(v.compare_and_swap(old_v * 3, old_v) != old_v);
}

int main(int argc, char *argv[])
{
    long int N = 100;
    int nth = 2;

    std::cout << "N = " << N << " and nth = " << nth << "\t";
    tbb::task_scheduler_init init(nth);

    tbb::parallel_invoke(
        [&](){
            for(int i = 0; i < N; i++)
                fetch_and_triple(v);
        },
        [&](){
            for(int i = 0; i < N; i++)
                fetch_and_triple(v);
        }
    );

    std::cout << " v = " << v << std::endl;
    return 0;
}