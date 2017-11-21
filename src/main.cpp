//
// Created by iris on 10/19/17.
//

#include "TransactionManager.h"
#include <iostream>
#include <stdlib.h> // atoi()
#include <vector>
#include <functional>

int main(int argc, const char *argv[]) {
    if (argc != 4) {
        std::cout << "Usage: num_thread num_record max_EX_order\n";
        return 1;
    }
    // # of threads
    int N = atoi(argv[1]);

    // # of records
    int R = atoi(argv[2]);

    // # of commits
    int E = atoi(argv[3]);

    // start running
    TransactionManager tm(N, R, E);

    // wait
    tm.join();

    return 0;
}
