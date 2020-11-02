#include <iostream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include <string>
#include <tbb/tbb.h>
#include "../ch02.h"
#include "../stringText.h"

void fig_2_27(int num_tokens, std::ofstream& caseBeforeFile, std::ofstream& caseAfterFile)
{
    tbb::parallel_pipeline(
        num_tokens,
        // the get filter
        tbb::make_filter<void, CaseStringPtr>(
            // filter node
            tbb::filter::serial_in_order,
            // filter body
            [&](tbb::flow_control& fc) -> CaseStringPtr{
                CaseStringPtr s_ptr = getCaseString(caseBeforeFile);
                if(!s_ptr)
                    fc.stop();
                return s_ptr;
            }
        ) &     // concatenation operation, and
        tbb::make_filter<CaseStringPtr, CaseStringPtr>(
            tbb::filter::parallel,
            [](CaseStringPtr s_ptr) -> CaseStringPtr {
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
                return s_ptr;
            }
        ) &
        tbb::make_filter<CaseStringPtr, void>(
            tbb::filter::serial_in_order,
            [&](CaseStringPtr s_ptr) -> void {
                writeCaseString(caseAfterFile, s_ptr);
            }
        )
    );
}

int main(int argc, char* argv[])
{
    int num_tokens = tbb::task_scheduler_init::default_num_threads();
    int num_strings = 100;
    int string_len = 100000;
    int free_list_size = num_tokens;
    
    std::ofstream caseBeforeFile("fig_2_27_before.txt");
    std::ofstream caseAfterFile("fig_2_27_after.txt");
    initCaseChange(num_strings, string_len, free_list_size);

    warmupTBB();
    double parallel_time = 0.0;
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_2_27(num_tokens, caseBeforeFile, caseAfterFile);
    parallel_time = (tbb::tick_count::now() - t0).seconds();

    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    return 0;
}