#include <utility>
#include <map>
#include <vector>
#include <iostream>

bool isPrime(int n){
    bool prime = true;
    for(int i = 2; i * i < n; i++){
        if(n % i == 0){
            prime = false;
            break;
        }
    }
    return prime;
}

using PrimesValue = std::pair<int, bool>;
struct PrimesTreeElement{
    using Ptr = std::shared_ptr<PrimesTreeElement>;
    PrimesValue v;
    Ptr left;
    Ptr right;
    PrimesTreeElement(const PrimesValue& _v) : left(), right(){
        v.first = _v.first;
        v.second = _v.second;
    };
};

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
    int level, const IntVector &vec,
    IntVector::const_iterator i)
{
    if(level && i != vec.end()){
        PrimesTreeElement::Ptr e = std::make_shared<PrimesTreeElement>(PrimesValue(*i, false));
        if(level - 1){
            e->left = makePrimesTreeElem(level - 1, vec, ++i);
            e->right = makePrimesTreeElem(level - 1, vec, ++i);
        }
        return e;
    }
    else
        return nullptr;
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
        if (!validatePrimesElem(e->left, m) || 
            !validatePrimesElem(e->right, m))
            return false;
    }
    return true;
}