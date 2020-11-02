#include <iostream>
#include <algorithm>
#include <tbb/tbb.h>
#include "../ch02.h"

void spinWaitForAtLeast(double sec)
{
    if(sec == 0.0)
        return;

    tbb::tick_count t0 = tbb::tick_count::now();
    while((tbb::tick_count::now() - t0).seconds() < sec);
}

void f(int v)
{
    if(v % 2){
        spinWaitForAtLeast(0.001);
    }else{
        spinWaitForAtLeast(0.002);
    }
}

void fig_2_6(int N, const std::vector<int>& a)
{
    tbb::parallel_for(0, N, 1, 
        [a](int i){
            f(a[i]);
        });
}

void serialImpl(int N, const std::vector<int> &a)
{
    for(int i = 0; i < N; i++)
        f(a[i]);
}

int main(int argc, char *argv[])
{
    const int N = 1000;
    std::vector<int> v(N, 0);
    int i = 0;
    std::generate(v.begin(), v.end(), [&i](){ return i++; });

    double serial_time = 0.0, parallel_time = 0.0;
    {
        tbb::tick_count t0 = tbb::tick_count::now();
        serialImpl(N, v);
        serial_time = (tbb::tick_count::now() - t0).seconds();
    }

    warmupTBB();
    {
        tbb::tick_count t0 = tbb::tick_count::now();
        fig_2_6(N, v);
        parallel_time = (tbb::tick_count::now() - t0).seconds();
    }

    std::cout << "Num threads: " << tbb::task_scheduler_init::default_num_threads() << std::endl;
    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;
    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    std::cout << "speedup == " << serial_time / parallel_time << std::endl;

    return 0;
}