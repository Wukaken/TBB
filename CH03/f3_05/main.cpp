#include <iostream>
#include <tbb/tbb.h>
#include "../../utils.h"

void fig_3_5()
{
    tbb::flow::graph g;

    tbb::flow::function_node<int, std::string> my_node(
        g, tbb::flow::unlimited,
        [](const int& in) -> std::string {
            std::cout << "received: " << in << std::endl;
            return std::to_string(in);
        }
    );
    tbb::flow::function_node<int, double> my_other_node(
        g, tbb::flow::unlimited,
        [](const int& in) -> double {
            std::cout << "other received: " << in << std::endl;
            return double(in);
        }
    );

    tbb::flow::join_node<std::tuple<std::string, double>, 
                         tbb::flow::queueing> my_join_node(g);
    tbb::flow::function_node<std::tuple<std::string, double>, 
                             int> my_final_node(
        g, tbb::flow::unlimited,
        [](const std::tuple<std::string, double>& in) -> int {
            std::cout << "final: " << std::get<0>(in)
                      << " and " << std::get<1>(in) << std::endl;
            return 0;
        }
    );

    make_edge(my_node, tbb::flow::input_port<0>(my_join_node));
    make_edge(my_other_node, tbb::flow::input_port<1>(my_join_node));
    make_edge(my_join_node, my_final_node);

    my_node.try_put(1);
    my_other_node.try_put(2);
    g.wait_for_all();
}

int main(int argc, char *argv[])
{
    warmupTBB();
    
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_3_5();
    double parallel_time = (tbb::tick_count::now() - t0).seconds();

    std::cout << "parallel_time == " << parallel_time << " seconds" << std::endl;
    return 0;
}
