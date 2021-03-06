#include <iostream>
#include <vector>
#include <string>
#include <pstl/algorithm>
#include <pstl/execution>

int main()
{
    std::vector<std::string> v = {" Hello ", " Parallel STL! "};
    std::for_each(pstl::execution::par, v.begin(), v.end(),
        [](std::string &s) { std::cout << s << std::endl; }
    );
    return 0;
}