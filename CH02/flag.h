#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

enum LOSInputType { ALT_FLAT = 0, ALT_DECREASING, ALT_WAVE, ALT_PLATEAU };
static std::string LOSInputNames[] = {"flat", "decreasing", "wave", "plateau"};

static std::vector<double> makeAltitudeVector(int N, LOSInputType t)
{
    std::vector<double> v(N);
    const int saw_period = N / 10;
    double previous_value = N;
    double delta = -1;
    for(int i = 0; i < N; i++){
        switch(t){
            case ALT_FLAT:
                if(i == 0)
                    v[i] = N;
                else
                    v[i] = N + 10;
                break;
            case ALT_DECREASING:
                if(i == 0){
                    v[i] = N;
                    previous_value = -1;
                } else
                    v[i] = ++previous_value;
                break;
            case ALT_WAVE:
                if(i == 0){
                    v[i] = previous_value = N;
                    delta = -1;
                }else{
                    v[i] = (previous_value += delta);
                    if(i % saw_period == 0)
                        delta *= -1;
                }
                break;
            case ALT_PLATEAU:
                if(i == 0)
                    v[i] = N;
                else if(i < saw_period)
                    v[i] = N / 2;
                else
                    v[i] = N / 4;
                break;
        }
    }
    return v;
}

static void printVisibility(bool b, int f, int l)
{
    std::cerr << f << " - " << l << " : ";
    if(b)
        std::cerr << "visible" << std::endl;
    else
        std::cerr << "not visible" << std::endl;
}

static bool checkVisibility(LOSInputType t, std::vector<bool> &is_visible)
{
    const int N = is_visible.size();
    const int saw_period = N / 10;
    int i = 0;
    switch (t){
        case ALT_FLAT:
            if(!is_visible[0] || !is_visible[1]) 
                return false;
            for(i = 2; i < N; i++){
                if(is_visible[i] == true)
                    return false;
            }
            return true;
            break;
        case ALT_DECREASING:
            for(i = 0; i < N; i++){
                if(is_visible[i] == false)
                    return false;
            }
            return true;
            break;
        case ALT_WAVE:
            for(i = 0; i < 2 * saw_period; i++){
                if(is_visible[i] == false)
                    return false;
            }
            for(i = 2 * saw_period; i < N; i += 2 * saw_period){
                if(is_visible[i] == false)
                    return false;
            }
            return true;
            break;
        case ALT_PLATEAU:
            for(i = 0; i < saw_period; i++){
                if(is_visible[i] == false)
                    return false;
            }
            while(is_visible[i] == false && i < N)
                i++;
            if(i <= saw_period || i == N){

                std::cout << "current: i : " << i << std::endl;
                return false;
            }
            for(; i < N; i++){
                if(is_visible[i] == false)
                    return false;
            }
            return true;
            break;
        default:
            return false;
    }
}

static void dumpLOS(std::vector<bool> &is_visible)
{
    bool can_see_it = true;
    const int N = (int)is_visible.size();
    int first_index = 0;
    for(int i = 0; i < N; i++){
        bool b = is_visible[i];
        if(can_see_it != b){
            printVisibility(can_see_it, first_index,  i - 1);
            can_see_it = b;
            first_index = i;
        }
    }
    printVisibility(is_visible[N - 1], first_index, N - 1);
}