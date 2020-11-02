#include <algorithm>
#include <iostream>
#include <tbb/tbb.h>

void fig_2_7(int M, double *a, double *b, double *c)
{
    for(int i = 0; i < M; i++){
        for(int j = 0; j < M; j++){
            int c_index = i * M + j;
            for(int k = 0; k < M; k++){
                //std::cout << "id: " << i << " " << j << " " << k << " " << c_index << " " << i * M + k << " " << k * M + j << std::endl;
                c[c_index] += a[i * M + k] * b[k * M + j];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    const int M = 1024;
    const int MxM = M * M;
    double *a = new double[MxM];
    double *b = new double[MxM];
    double *c = new double[MxM];
    double *c_expected = new double[MxM];

    std::fill(a, a + MxM, 1.0);
    std::fill(b, b + MxM, 1.0);
    std::fill(c, c + MxM, 0.0);
    std::fill(c_expected, c_expected + MxM, M);
    
    double serial_time = 0.0;
    {
        tbb::tick_count t0 = tbb::tick_count::now();
        fig_2_7(M, a, b, c);
        serial_time = (tbb::tick_count::now() - t0).seconds();
        if(!std::equal(c, c + MxM, c_expected))
            std::cerr << "Error: c array does not match expected values" << std::endl;
    }
    
    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;
    
    delete[] a;
    delete[] b;
    delete[] c;
    delete[] c_expected;
    
    return 0;
}