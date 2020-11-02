#include <vector>
#include <iostream>
#include <tbb/tbb.h>
#include "../ch02.h"

int fig_2_14(const std::vector<int> &v, std::vector<int> &rsum){
    int N = v.size();
    rsum[0] = v[0];
    int final_sum = tbb::parallel_scan(
        tbb::blocked_range<int>(1, N),
        int(0),
        [&v, &rsum](const tbb::blocked_range<int> &r, int sum, bool is_final_scan) -> int {
            for(int i = r.begin(); i < r.end(); i++){
                sum += v[i];
                if(is_final_scan)
                    rsum[i] = sum;
            }
            return sum;
        },  
        [](int x, int y){
            return x + y;
        }
    );
    return final_sum;
}

int serialImpl(const std::vector<int> &v, std::vector<int> &rsum)
{
    int N = v.size();
    rsum[0] = v[0];
    for(int i = 1; i < N; i++)
        rsum[i] = rsum[i - 1] + v[i];

    int final_sum = rsum[N - 1];
    return final_sum;
}

int main(int argc, char* argv[])
{
    const int N = 1e4;
    std::vector<int> v(N, 0);
    std::vector<int> serial_rsum(N, 0);
    std::vector<int> parallel_rsum(N, 0);
    for(int i = 0; i < N; i++)
        v[i] = i;

    double serial_time = 0.0, parallel_time = 0.0;
    tbb::tick_count t0 = tbb::tick_count::now();
    int final_sum = serialImpl(v, serial_rsum);
    serial_time = (tbb::tick_count::now() - t0).seconds();
    int correct_sum = N * (N - 1) / 2;
    if(final_sum != correct_sum)
        std::cerr << "Error: serial final_sum is wrong " << final_sum << " != " << correct_sum << std::endl;

    warmupTBB();
    t0 = tbb::tick_count::now();
    final_sum = fig_2_14(v, parallel_rsum);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
    if(final_sum != correct_sum)
        std::cerr << "Error: parallel final_sum is wrong " << final_sum << " != " << correct_sum << std::endl;

    if(serial_rsum != parallel_rsum)
        std::cerr << "Error: rsum vectors do not match!" << std::endl;

    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;
    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    std::cout << "speedup == " << serial_time / parallel_time << std::endl;

    return 0;
}