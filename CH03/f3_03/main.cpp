#include <iostream>
#include <tbb/tbb.h>
#include "../../utils.h"

void fig_3_3()
{
    tbb::flow::graph g;

    tbb::flow::function_node<int, std::string> my_first_node(
        g, tbb::flow::unlimited,
        [](const int& in) -> std::string {
            std::cout << "first node received: " << in << std::endl;
            return std::to_string(in);
        }
    );
    tbb::flow::function_node<std::string> my_second_node(
        g, tbb::flow::unlimited,
        [](const std::string& in) {
            std::cout << "second node received: " << in << std::endl;
        }
    );
    tbb::flow::make_edge(my_first_node, my_second_node);
    my_first_node.try_put(10);
    g.wait_for_all();
}

int main(int argc, char *argv[])
{
    warmupTBB();
    
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_3_3();
    double parallel_time = (tbb::tick_count::now() - t0).seconds();

    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    return 0;
}