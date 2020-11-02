#include <iostream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include <string>
#include <tbb/tbb.h>
#include "../ch02.h"

using CaseStringPtr = std::shared_ptr<std::string>;
CaseStringPtr getCaseString(std::ofstream& f);
void writeCaseString(std::ofstream& f, CaseStringPtr s);

void fig_2_24(std::ofstream& caseBeforeFile, std::ofstream& caseAfterFile)
{
    while(CaseStringPtr s_ptr = getCaseString(caseBeforeFile)){
        std::transform(s_ptr->begin(), s_ptr->end(), s_ptr->begin(),
            [](char c) -> char {
                if(std::islower(c))
                    return std::toupper(c);
                else if(std::isupper(c))
                    return std::tolower(c);
                else
                    return c;
            }
        );
        writeCaseString(caseAfterFile, s_ptr);
    }
}

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

int main(int argc, char* argv[])
{
    int num_strings = 100;
    int string_len = 100000;
    int free_list_size = 1;
    
    std::ofstream caseBeforeFile("fig_2_24_before.txt");
    std::ofstream caseAfterFile("fig_2_24_after.txt");
    initCaseChange(num_strings, string_len, free_list_size);

    double serial_time = 0.0;
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_24(caseBeforeFile, caseAfterFile);
    serial_time = (tbb::tick_count::now() - t0).seconds();

    std::cout << "serial_time == " << serial_time << " seconds" << std::endl;
    return 0;
}