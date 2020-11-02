#pragma once

#define NOMINMAX

#include <algorithm>
#include <cfloat>
#include <iostream>
#include <random>
#include <vector>
#include <tbb/tbb.h>

struct DataItem{
    int id;
    double value;
    DataItem(int i, double v) : id(i), value(v) {}
};

using QSVector = std::vector<DataItem>;

void serialQuicksort(QSVector::iterator b, QSVector::iterator e)
{
    if(b >= e)
        return;

    double pivot_value = b->value;
    QSVector::iterator i = b, j = e - 1;
    while(i != j){
        while(i != j && pivot_value < j->value) --j;
        while(i != j && i->value <= pivot_value) ++i;
        std::iter_swap(i, j);
    }
    std::iter_swap(b, i);

    serialQuicksort(b, i);
    serialQuicksort(i + 1, e);
}

void parallelQuicksort(QSVector::iterator b, QSVector::iterator e)
{
    const int cutoff = 100;
    if(e - b < cutoff){
        serialQuicksort(b, e);
    }else{
        double pivot_value = b->value;
        QSVector::iterator i = b, j = e - 1;
        while(i != j){
            while(i != j && pivot_value < j->value) --j;
            while(i != j && i->value <= pivot_value) ++i;
            std::iter_swap(i, j);
        }
        std::iter_swap(b, i);

        tbb::parallel_invoke(
            [=]() { parallelQuicksort(b, i); },
            [=]() { parallelQuicksort(i + 1, e); }
        );
    }
}

static QSVector makeQSData(int N)
{
    QSVector v;
    std::default_random_engine g;
    std::uniform_real_distribution<double> d(0.0, 1.0);
    for(int i = 0; i < N; i++)
        v.push_back(DataItem(i, d(g)));

    return v;
}

static bool checkIsSorted(const QSVector &v)
{
    double max_value = std::numeric_limits<double>::min();
    for(auto e : v){
        if(e.value < max_value){
            std::cerr << "Sort Failed" << std::endl;
            return false;
        }
        max_value = e.value;
    }
    return true;
}

static void warmupTBB()
{
    tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(),
                      [](int){
                          tbb::tick_count t0 = tbb::tick_count::now();
                          while((tbb::tick_count::now() - t0).seconds() < 0.01);
                      });
}

static std::vector<double> initForwardSubstitution(
    std::vector<double> &x, std::vector<double> &a,
    std::vector<double> &b)
{
    const int N = x.size();
    for(int i = 0; i < N; i++){
        x[i] = 0;
        b[i] = i * i;
        for(int j = 0; j <= i; j++)
            a[j + i * N] = 1 + i * j;
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
