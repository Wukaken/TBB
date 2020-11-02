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
    DataItem(int i, double v) : id{i}, value{v} {}
};

using QSVector = std::vector<DataItem>;

void serialQuicksort(QSVector::iterator b, QSVector::iterator e)
{
    if(b >= e)
        return;

    double pivot_value = b->value;
    QSVector::iterator i = b;
    QSVector::iterator j = e - 1;
    while(i != j){
        while(i != j && pivot_value < j->value) --j;
        while(i != j && i->value <= pivot_value) ++i;
        std::iter_swap(i, j);
    }
    std::iter_swap(b, i);

    serialQuicksort(b, i);
    serialQuicksort(i + 1, e);
}

static QSVector makeQSData(int N)
{
    QSVector v;
    std::default_random_engine g;
    std::uniform_real_distribution<double> d(0.0, 1.0);
    for(int i = 0; i < N; i++)
        v.push_back(DataItem{i, d(g)});
    
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

int main()
{
    const int N = 1000000;

    QSVector v = makeQSData(N);
    double serial_time = 0.0;
    {
        tbb::tick_count t0 = tbb::tick_count::now();
        serialQuicksort(v.begin(), v.end());
        serial_time = (tbb::tick_count::now() - t0).seconds();
        if(!checkIsSorted(v))
            std::cerr << "Error: serial sorted list out of order" << std::endl;
    }
    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;
    return 0;
}
