#include <memory>
#include <tbb/tbb.h>
#include <fstream>

using CaseStringPtr = std::shared_ptr<std::string>;
CaseStringPtr getCaseString(std::ofstream& f);
void writeCaseString(std::ofstream& f, CaseStringPtr s);

static tbb::concurrent_queue<CaseStringPtr> caseFreeList;
static int numCaseInputs = 0;

void initCaseChange(int num_strings, int string_len, int free_list_size)
{
    numCaseInputs = num_strings;
    caseFreeList.clear();
    for(int i = 0; i < free_list_size; i++)
        caseFreeList.push(std::make_shared<std::string>(string_len, ' '));
}

CaseStringPtr getCaseString(std::ofstream& f)
{
    std::shared_ptr<std::string> s;
    if(numCaseInputs > 0){
        if(!caseFreeList.try_pop(s) || !s){
            std::cerr << "Error: Ran out of elements in free list!" << std::endl;
            return CaseStringPtr{};
        }
        int ascii_range = 'z' - 'A' + 2;
        for(int i = 0; i < s->size(); i++){
            int offset = i % ascii_range;
            if(offset)
                (*s)[i] = 'A' + offset - 1;
            else
                (*s)[i] = '\n';
        }
        if(f.good())
            f << *s;
        --numCaseInputs;
    }
    return s;
}

void writeCaseString(std::ofstream& f, CaseStringPtr s)
{
    if(f.good())
        f << *s;
    caseFreeList.push(s);
}