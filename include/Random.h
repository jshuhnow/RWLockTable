//
// Created by iris on 10/21/17.
//

#ifndef PROJECT2_RANDOM_H
#define PROJECT2_RANDOM_H
#include <random>
#include <tuple>
#include <mutex>
class Random {
public:
    Random(int min, int max);
    std::tuple<int, int, int> getDistinctThree();
private:
    std::mt19937 gen; //Standard mersenne_twister_engine seeded with std::random_device
    std::uniform_int_distribution<> dis;
    std::mutex lk;
    int n, min, max;
};


#endif
