#include <iostream>
#include <vector>
#include <tbb/tbb.h>
#include "../ch02.h"

void fig_2_23(std::vector<double> &x, const std::vector<double> &a,
              std::vector<double> &b)
{
    const int N = x.size();
    const int block_size = 512;
    const int num_blocks = N / block_size;
    std::vector<tbb::atomic<char>> ref_count(num_blocks * num_blocks);
    for(int r = 0; r < num_blocks; r++){
        for(int c = 0; c <= r; c++){
            if(r == 0 && c == 0)
                ref_count[r * num_blocks + c] = 0;
            else if(c == 0 || r == c)
                ref_count[r * num_blocks + c] = 1;
            else
                ref_count[r * num_blocks + c] = 2;
        }
    }

    using BlockIndex = std::pair<size_t, size_t>;
    BlockIndex top_left(0, 0);

    tbb::parallel_do(&top_left, &top_left + 1,
        [&](const BlockIndex &bi, tbb::parallel_do_feeder<BlockIndex> &feeder){
            size_t r = bi.first;
            size_t c = bi.second;
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
            // add successor to right if ready
            if(c + 1 <= r && --ref_count[r * num_blocks + c + 1] == 0)
                feeder.add(BlockIndex(r, c + 1));
            // add successor below if ready
            if(r + 1 < (size_t)num_blocks && --ref_count[(r + 1) * num_blocks + c] == 0)
                feeder.add(BlockIndex(r + 1, c));
        }
    );
}

int main(int argc, char* argv[])
{
    const int N = 32768;
    std::vector<double> a(N * N);
    std::vector<double> b(N);
    std::vector<double> x(N);

    auto x_gold = initForwardSubstitution(x, a, b);
    double parallel_time = 0.0;
    warmupTBB();

    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_23(x, a, b);
    parallel_time = (tbb::tick_count::now() - t0).seconds();

    for(int i = 0; i < N; i++){
        if(x[i] > 1.1 * x_gold[i] || x[i] < 0.9 * x_gold[i])
        std::cerr << "  at " << i << " " << x[i] << " != " << x_gold[i] << std::endl;
    }
    
    std::cout << "parallel_do_time == " << parallel_time << " seconds" << std::endl;
    return 0;
}