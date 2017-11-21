//
// Created by iris on 10/21/17.
//

#include "Random.h"
#include <tuple>
#include <utility>
#include <cassert>
#include <random>
#include <algorithm> //random_shuffle
#include <mutex>

Random::Random(int min, int max) :
    gen(std::random_device()()),
    dis(0, max-min+1), // uniform distribution [0, max-min+1] (inclusive)
    n(max-min+1), max(max), min(min) {
        assert(n >= 3);
    }

// O(N) - it is safer to use this algorithm whose time complexity is BOUNDED.
std::tuple<int, int, int> Random::getDistinctThree() {
    std::unique_lock<std::mutex> lock(lk);

    std::vector<int> v(n);
    for (size_t i=0; i<n; ++i) {
        v[i] = i+min;
    }
    std::random_shuffle(v.begin(), v.end());
    assert(v[0] != v[1] && v[1] != v[2] && v[2] != v[0]);
    return std::make_tuple(v[0], v[1], v[2]);
}
