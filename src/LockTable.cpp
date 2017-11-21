//
// Created by iris on 10/20/17.
//
#include "LockTable.h"
#include "LLNode.h"
#include "LinkedList.h"
#include "Constants.h"
#include <vector>
#include <cassert>
#include <queue>
#include <iostream>

LockTable::LockTable(int N, int R) : N(N), R(R), bfsGlobalOrder(0), isGrowing(true), size(0) {
    related_locks.resize(N);
    lls.resize(R);
}
std::vector<LLNode*> LockTable::getRequestedLock(long tid) {
    return related_locks[tid];
}
bool LockTable::acquire(int tid, int rid, bool isShared) {
    return lls[rid].tryLock(tid, rid, isShared);
}
bool LockTable::tryLock(int tid, int rid, bool isShared) {
    std::unique_lock<std::mutex> uqlk(lk);

#ifdef MYDEBUG
    std::cout << "tryLock: " "[" << rid << "] " <<
    (isShared? "S": "X") << tid << std::endl;
#endif

    LLNode *ptr = lls[rid].push(tid, rid, isShared);
    related_locks[(ptr->tid)].push_back(ptr);
    size++;

    if (isGrowing == false || checkDeadLock(ptr)) {
        //printLinkedList();
        return false;
    }

    //note that conditional statement is critical section
    while(isGrowing && !acquire(tid, rid, isShared)) {
#ifdef MYDEBUG
        std::cout << "sleep! tid=" << tid<< std::endl;
#endif

        //spin while isGrowing = true
        //release lock
        cv.wait(uqlk);
        //acquire lock

#ifdef MYDEBUG
        std::cout << "wakeup! tid=" << tid<< std::endl;
#endif

    }
#ifdef MYDEBUG
    std::cout<< (isGrowing?"":"FAILED to ") << "lock: [" << rid <<  "] " <<
    (isShared? "S" : "X") <<tid << std::endl;
#endif

    return isGrowing;
}
// return - false if Deadlock detected
bool LockTable::trySharedLock(int tid, int rid) {
    return tryLock(tid, rid, true);
}

// return - false if Deadlock detected
bool LockTable::tryExclusiveLock(int tid, int rid) {
    return tryLock(tid, rid, false);
}

bool LockTable::releaseAndWait(int tid, int i, int j, int k) {
    std::unique_lock<std::mutex> lock(lk);

#ifdef MYDEBUG
    std::cout<< "releaseAndWait tid=" << tid << ", i=" << i << ", j=" << j << ", k=" << k << std::endl;
    std::cout << "size= " << size << std::endl;
#endif
    size -= related_locks[tid].size();
    related_locks[tid].clear();

#ifdef MYDEBUG
    std::cout << "size= " << size << std::endl;
#endif

    bool r = lls[i].eraseActiveLock(tid, i, true) &&
        (j==-1 || lls[j].eraseActiveLock(tid, j, false)) &&
        (k==-1 || lls[k].eraseActiveLock(tid, k, false));

    if (size == 0) {
        cv.notify_all();
        // Now the locktable grows!
        isGrowing = true;
    } else {
        //wait until 'size' == 0
        cv.wait(lock, [tid, this]()->bool {
#ifdef MYDEBUG
            std::cout << "checking size of tid " << tid << " = " << this->size << std::endl;
#endif
            return this->size == 0; });
    }

    return r;
}
bool LockTable::release(int tid, int i, int j, int k) {
    std::unique_lock<std::mutex> lock(lk);

#ifdef MYDEBUG
    std::cout<< "release tid=" << tid << ", i=" << i << ", j=" << j << ", k=" << k << std::endl;
#endif

    size -= related_locks[tid].size();
    related_locks[tid].clear();

    // unlock and wakeup next
    // -1 is default value for parameter 'j' and 'k'. Refer the header file.
    bool r=  lls[i].tryUnlock(tid, i, true) &&
    (j==-1 || lls[j].tryUnlock(tid, j, false)) &&
    (k==-1 || lls[k].tryUnlock(tid, k, false));
    if (r) {
        cv.notify_all();
    } else {
        //for debugging
#ifdef MYDEBUG
        printLinkedList(true);
#endif
    }
    return r;
}

// lock should be acquired before
// Time Complexity O(NR)
// return true if the graph is cyclic, false if acyclic(=not cyclic)
bool LockTable::checkDeadLock(LLNode *ptr) {
    assert(isGrowing);
    //bfsGlobalOrder is monotonically increased for every check.
    long long g = bfsGlobalOrder++;
    std::queue<LLNode*> q;

    // enqueue all the locks requested by this thread (at most the maximum number of locks per thread)
    for (LLNode *e : getRequestedLock(ptr->tid)) {
        e->bfsOrder = g;
        q.push(e);
    }

    //TODO :: check BFS order
    while(!q.empty()) {
        auto cur = q.front(); q.pop();

        // prvious lock
        if (cur->prv) {
            if (cur->prv->tid == ptr->tid) {
                //Cycle Detected
                isGrowing = false;
#ifdef MYDEBUG
                std::cout<<"Deadlock detected" << std::endl;
#endif
                cv.notify_all();
                return true;
            }
            if (cur->prv->bfsOrder < g && // not visited in this check
                cur->prv->prv) { // not head
                cur->prv->bfsOrder = g;
                q.push(cur->prv);
            }
        }

        // related lock (=def) locks requested by this thread
        for (LLNode* e : getRequestedLock(cur->tid)) {
            if (e == cur || e->bfsOrder >= g) continue;
            e->bfsOrder = g;
            q.push(e);
        }
    }
    // acyclic
    return false;
}

// no needs to wrap up with MYDEBUG
void LockTable::printLinkedList(bool lockAcquired) {
    if (lockAcquired) {
        std::cout << "===========================================\n";
        for (int i=0; i<R; i++) {
            std::cout << "Record " << i << " ";
            lls[i].print();
            std::cout << std::endl;
        }
        std::cout << "===========================================\n";
    }
    else {
        std::lock_guard<std::mutex> lock(lk);
        std::cout << "===========================================\n";
        for (int i=0; i<R; i++) {
            std::cout << "Record " << i << " ";
            lls[i].print();
            std::cout << std::endl;
        }
        std::cout << "===========================================\n";
    }
}
