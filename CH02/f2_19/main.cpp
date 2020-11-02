#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <utility>
#include <vector>
#include <tbb/tbb.h>
#include "../ch02.h"
#include "../primeUtils.h"

void fig_2_19(PrimesTreeElement::Ptr root){
    PrimesTreeElement::Ptr tree_array[] = {root};
    tbb::parallel_do(
        tree_array,
        [](PrimesTreeElement::Ptr e, tbb::parallel_do_feeder<PrimesTreeElement::Ptr> &feeder){
            if(e){
                if(e->left)
                    feeder.add(e->left);
                if(e->right)
                    feeder.add(e->right);
                if(isPrime(e->v.first))
                    e->v.second = true;
            }
        }
    );
}

int main(int argc, char* argv[])
{
    const int levels = 14;
    const int N = std::pow(2, levels) - 1;
    PrimesMap m;

    auto vec = makePrimesValues(N, m);
    auto root = makePrimesTree(levels, vec);

    double parallel_time = 0.0;
    warmupTBB();

    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_19(root);
    parallel_time = (tbb::tick_count::now() - t0).seconds();
    if(!validatePrimesElem(root, m))
        std::cerr << "Error: incorrect results!" << std::endl;
    
    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    return 0;
}