//
// Created by iris on 10/20/17.
//

#include "TransactionManager.h"
#include "Random.h"
#include "Constants.h"

#include <functional>
#include <thread>
#include <vector>
#include <tuple>
#include <string>
#include <queue>
#include <iostream>
#include <cassert>

TransactionManager::TransactionManager(int N, int R, int E) : N(N), R(R), E(E), random(0, R-1), lkTbl(N, R) {
    threads.resize(N);
    records.resize(R);

    // ofstream for each threads
    ofs.resize(N);

    global_execution_order = 0;
    for (int i=0; i<N; i++) {
        ofs[i].open("thread" + std::to_string(i+1) + ".txt");
    }

    // start running
    for (int i=0; i<N; i++) {
        threads[i] = std::thread(&TransactionManager::run, this, i);
    }

}
TransactionManager::~TransactionManager() {
    for (int i=0; i<N; ++i) {
        ofs[i].close();
    }
}

void TransactionManager::run(int tid) {
    // until global_execution_order <= E
    while(transaction(tid)) {

#ifdef MYDEBUG
        std::cout << "Transaction tid="  << tid << " Finished\n";
        std::cout << "=======================================\n";
#endif

    }
}

/* returns true - global_execution_order <= E
        false - otherwise */
bool TransactionManager::transaction(int tid) {

    auto ijk = random.getDistinctThree();

    int i = std::get<0>(ijk), j = std::get<1>(ijk), k = std::get<2>(ijk);

#ifdef MYDEBUG
    std::cout << "\n===============================================================\n";
    std::cout << "Global Execution Order = " << global_execution_order << std::endl;
    std::cout << "Transaction tid= " << tid << ": " << i << " " << j << " " << k << std::endl;
    lkTbl.printLinkedList();
#endif

    if (lkTbl.trySharedLock(tid, i) == false) {
        assert(lkTbl.releaseAndWait(tid ,i));
        return true;
    }

    // fetch i-th
    long long valI = records[i].read();

    /* try{Shared,Exclusive}Lock returns false if cycle is detected
    release and releaseAndWait return true if suceeded and it should.
    Use release() if you have to wakeup the next lock of acquired locks.
    Use releaseAndWait() if the following locks shouldn't be woke up
    */
    if (lkTbl.tryExclusiveLock(tid, j) == false) {
        // Rollback
        assert(lkTbl.releaseAndWait(tid, i, j));
        return true;
    }

    // install j-th
    long long valJ = records[j].write(records[j].read() + valI + 1);

    if (lkTbl.tryExclusiveLock(tid, k) == false) {
        // Rollback
        records[j].write(records[j].read() - (valI+1));
        assert(lkTbl.releaseAndWait(tid, i, j, k));
        return true;
    }

    // install k-th
    long long valK = records[k].write(records[k].read() - valI);


    long long commitId = ++global_execution_order;
    if (commitId > E) {
        // Rollback!
        records[j].write(records[j].read() - (valI + 1));
        records[k].write(records[k].read() + valI);
        assert(lkTbl.release(tid, i,j,k) );

        return false;
    }
    assert(lkTbl.release(tid, i,j,k));

    //Commit - indices are converted to 1-base from 0-base
    ofs[tid] << commitId << " " << i+1 << " " << j+1 << " " <<  k+1 <<
    " " << valI << " " << valJ << " " << valK << "\n";

    return true;
}

void TransactionManager::join() {
    for (int i=0; i<N; i++) {
        threads[i].join();
    }
}
