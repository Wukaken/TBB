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

using PrimesValue = std::pair<int, bool>;
struct PrimesTreeElement{
    using Ptr = std::shared_ptr<PrimesTreeElement>;
    PrimesValue v;
    Ptr left;
    Ptr right;
    PrimesTreeElement(const PrimesValue& _v) : left(), right(){
        v.first = _v.first;
        v.second = _v.second;
    }
};

void serialCheckPrimesElem(PrimesTreeElement::Ptr e){
    if(e){
        if(isPrime(e->v.first))
            e->v.second = true;
        if(e->left) 
            serialCheckPrimesElem(e->left);
        if(e->right) 
            serialCheckPrimesElem(e->right);
    }
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

static PrimesTreeElement::Ptr makePrimesTreeElem(
    int level, const IntVector &vec, IntVector::const_iterator i)
{
    PrimesTreeElement::Ptr e = NULL;
    if(level && i != vec.cend()){
        e = std::make_shared<PrimesTreeElement>(PrimesValue(*i, false));
        if(level - 1){
            e->left = makePrimesTreeElem(level - 1, vec, ++i);
            e->right = makePrimesTreeElem(level - 1, vec, ++i);
        }
    }
    return e;
}

static PrimesTreeElement::Ptr makePrimesTree(int level, IntVector &vec)
{
    return makePrimesTreeElem(level, vec, vec.cbegin());
}

static bool validatePrimesElem(PrimesTreeElement::Ptr e, PrimesMap &m)
{
    if(e){
        if(m[e->v.first] != e->v.second)
            return false;
        if(!validatePrimesElem(e->left, m) || !validatePrimesElem(e->right, m))
            return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    const int levels = 14;
    const int N = std::pow(2, levels) - 1;
    PrimesMap m;

    auto vec = makePrimesValues(N, m);
    auto root = makePrimesTree(levels, vec);

    double serial_time = 0.0;

    tbb::tick_count t0 = tbb::tick_count::now();
    serialCheckPrimesElem(root);
    serial_time = (tbb::tick_count::now() - t0).seconds();
    if(!validatePrimesElem(root, m))
        std::cerr << "Error: incorrect results!" << std::endl;
    
    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;

    return 0;
}