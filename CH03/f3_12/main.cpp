#include <iostream>
#include <tbb/tbb.h>
#include "../../utils.h"

void fig_3_12(std::vector<double>& x, const std::vector<double>& a,
              std::vector<double>& b)
{
    const int N = x.size();
    const int block_size = 512;
    const int num_blocks = N / block_size;
    for(int r = 0; r < num_blocks; r++){
        for(int c = 0; c <= r; c++){
            int i_start = r * block_size;
            int i_end = i_start + block_size;
            int j_start = c * block_size;
            int j_max = j_start + block_size - 1;
            for(int i = i_start; i < i_end; i++){
                int j_end = (i <= j_max) ? i : j_max + 1;
                for(int j = j_start; j < j_end; j++)
                    b[i] -= a[j + i * N] * x[j];
                if(j_end == i)
                    x[i] = b[i] / a[i + i * N];
            }
        }
    }
}

static std::vector<double> initForwardSubstitution(
    std::vector<double>& x, std::vector<double>& a,
    std::vector<double>& b)
{
    const int N = x.size();
    for(int i = 0; i < N; i++){
        x[i] = 0;
        b[i] = i * i;
        for(int j = 0; j <= i; j++)
            a[j + i * N] = 1 + j * i;
    }

    std::vector<double> b_tmp = b;
    std::vector<double> x_gold = x;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < i; j++)
            b_tmp[i] -= a[j + i * N] * x_gold[j];
        x_gold[i] = b_tmp[i] / a[i + i * N];
    }
    return x_gold;
}

int main(int argc, char *argv[])
{
    const int N = 32768;

    std::vector<double> a(N * N);
    std::vector<double> b(N);
    std::vector<double> x(N);

    auto x_gold = initForwardSubstitution(x, a, b);
    double serial_time = 0.0;
    {
        tbb::tick_count t0 = tbb::tick_count::now();
        fig_3_12(x, a, b);
        serial_time = (tbb::tick_count::now() - t0).seconds();
    }

    for(int i = 0; i < N; i++){
        if(x[i] > 1.1 * x_gold[i] || x[i] < 0.9 * x_gold[i])
            std::cerr << "  at " << i << " " << x[i] << " != " << x_gold[i] << std::endl;
    }
    
    std::cout << "serial_tiled_time == " << serial_time << " seconds" << std::endl;
    return 0;
}
