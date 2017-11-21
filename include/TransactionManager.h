//
// Created by iris on 10/19/17.
//

#ifndef PROJECT2_TRANSACTIONMANAGER_H
#define PROJECT2_TRANSACTIONMANAGER_H
#include "Record.h"
#include "LockTable.h"
#include "Random.h"
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
class TransactionManager {
public:
    void join();
    TransactionManager(int N, int R, int E);
    ~TransactionManager();
private:
    std::vector<Record> records;

    int N, R, E;
    std::vector<std::thread> threads;
    
    std::vector<std::ofstream> ofs;
    long long global_execution_order;


    LockTable lkTbl;
    bool transaction(int tid);
    void run(int tid);
    Random random;

};
#endif //PROJECT2_TRANSACTIONMANAGER_H
