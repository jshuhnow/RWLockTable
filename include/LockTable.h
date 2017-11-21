//
// Created by iris on 10/19/17.
//

#ifndef PROJECT2_LOCKTABLE_H
#define PROJECT2_LOCKTABLE_H
#include <vector>
#include <mutex>
#include "LLNode.h"
#include "LinkedList.h"

class LockTable {
public:
    std::vector<LLNode*> getRequestedLock(long tid);

    LockTable(int N, int R);

    bool trySharedLock(int tid, int idx);
    bool tryExclusiveLock(int tid, int idx);
    bool releaseAndWait(int tid, int i, int j=-1, int k=-1);
    bool release(int tid, int i, int j=-1, int k=-1);
    void printLinkedList(bool lockAcquired=false);

private:
    bool tryLock(int tid, int idx, bool isShared);
    bool acquire(int tid, int idx, bool isShared);
    void clearLock(long tid);
    void pushLock(LLNode*);
    long long bfsGlobalOrder;

    //i-th element will store all the locks requested by thread i
    std::vector<std::vector<LLNode*>> related_locks;
    int N, R;
    std::vector<LinkedList> lls;
    bool checkDeadLock(LLNode*); //newly inserted Node provided
    bool acquire(LLNode*);
    bool isGrowing;
    int size;
    std::condition_variable cv;
    std::mutex lk;

};
#endif //PROJECT2_LOCKTABLE_H
