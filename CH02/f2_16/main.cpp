#include <tbb/tbb.h>
#include "../flag.h"
#include "../ch02.h"

void fig_2_16(const std::vector<double> &altitude, std::vector<bool> &is_visible, double dx){
    const int N = altitude.size();

    double max_angle = std::atan2(dx, altitude[0] - altitude[1]);
    double final_max_angle = tbb::parallel_scan(
        tbb::blocked_range<int>(1, N),
        0.0,
        [&altitude, &is_visible, dx](const tbb::blocked_range<int>& r,
                                     double max_angle, bool is_final_scan) -> double {
            for(int i = r.begin(); i != r.end(); i++){
                double my_angle = atan2(i * dx, altitude[0] - altitude[i]);
                if(my_angle >= max_angle)
                    max_angle = my_angle;
                else
                    is_visible[i] = false;
            }                                         
            return max_angle;
        },
        [](double a, double b) -> double {
            return std::max(a, b);
        }
    );
}

int main(int argc, char* argv[])
{
    const int N = 100000000;
    const double dx = 1.0;

    auto input_types = { ALT_FLAT, ALT_DECREASING, ALT_WAVE, ALT_PLATEAU };
    std::vector<bool> is_visible(N, true);

    double parallel_time = 0.0;
    tbb::tick_count t0;
    for(auto i : input_types){
        std::vector<double> altitude = makeAltitudeVector(N, i);
        std::fill(is_visible.begin(), is_visible.end(), true);
        t0 = tbb::tick_count::now();
        fig_2_16(altitude, is_visible, dx);
        parallel_time = (tbb::tick_count::now() - t0).seconds();
        dumpLOS(is_visible);
        if(checkVisibility(i, is_visible) != true)
            std::cerr << "Error: visibility is not as expected!" << std::endl;

        std::cout << "parallel_time for " << LOSInputNames[i] << " == " << parallel_time << " seconds" << std::endl;
    }

    return 0;
}