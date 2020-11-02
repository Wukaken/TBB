#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <utility>
#include <vector>
#include <tbb/tbb.h>
#include "../ch02.h"

using PrimesValue = std::pair<int, bool>;
using PrimesList = std::list<PrimesValue>;

bool isPrime(int n);

void fig_2_17(PrimesList& values)
{
    tbb::parallel_do(values, 
        [](PrimesList::reference v){
            if(isPrime(v.first))
                v.second = true;
        }
    );
}

void serialImpl(PrimesList &values)
{
    for(PrimesList::reference v : values){
        if(isPrime(v.first))
            v.second = true;
    }
}

bool isPrime(int n)
{
    int e = std::sqrt(n);
    std::vector<bool> p(e+1, true);
    for(int i = 2; i <= e; i++){
        if(p[i]){
            if(n % i){
                for(int j = 2 * i; j <= e; j += i){
                    p[j] = false;
                }
            }
            else 
                return false;
        }
    }
    return true;
}

using PrimesMap = std::map<int, bool>;
using IntVector = std::vector<int>;

static IntVector makePrimesValues(int n, PrimesMap &m)
{
    std::default_random_engine gen;
    std::uniform_int_distribution<int> dist;
    IntVector vec;
    for(int i = 0; i < n; i++){
        int v = dist(gen);
        vec.push_back(v);
        m[v] = isPrime(v);
    }
    return vec;
}

static PrimesList makePrimesList(const IntVector& vec)
{
    PrimesList l;
    for(auto &v : vec)
        l.push_back(PrimesValue(v, false));

    return l;
}

int main(int argc, char* argv[])
{
    const int levels = 14;
    const int N = std::pow(2, levels) - 1;
    PrimesMap m;

    auto vec = makePrimesValues(N, m);
    PrimesList sList = makePrimesList(vec);
    PrimesList pList = makePrimesList(vec);

    double serial_time = 0.0;
    double parallel_time = 0.0;

    tbb::tick_count t0 = tbb::tick_count::now();
    serialImpl(sList);
    serial_time = (tbb::tick_count::now() - t0).seconds();
    for(auto p : sList){
        if(p.second != m[p.first])
            std::cerr << "Error: serial results are incorrect!" << std::endl;
    }

    warmupTBB();
    t0 = tbb::tick_count::now();
    fig_2_17(pList);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
    for(auto p : pList){
        if(p.second != m[p.first])
            std::cerr << "Error: parallel results are incorrect!" << std::endl;
    }

    if(sList != pList)
        std::cerr << "Error: serial and parallel implementations do not agree!" << std::endl;

    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;
    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    std::cout << "speedup == " << serial_time / parallel_time << std::endl;

    return 0;
}