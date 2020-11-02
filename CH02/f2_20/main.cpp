#include <iostream>
#include <vector>
#include <tbb/tbb.h>
#include "../ch02.h"

void fig_2_20(std::vector<double> &x, const std::vector<double> &a,
              std::vector<double> &b)
{
    const int N = x.size();
    for(int i = 0; i < N; i++){
        for(int j = 0; j < i; j++)
            b[i] -= a[j + i * N] * x[j];
        x[i] = b[i] / a[i + i * N];
    }
}

int main(int argc, char* argv[])
{
    const int N = 32768;
    std::vector<double> a(N * N);
    std::vector<double> b(N);
    std::vector<double> x(N);

    auto x_gold = initForwardSubstitution(x, a, b);
    double serial_time = 0.0;

    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_20(x, a, b);
    serial_time = (tbb::tick_count::now() - t0).seconds();
    if(x_gold != x)
        std::cerr << "Error: results do not match gold version" << std::endl;
    
    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;
    return 0;
}