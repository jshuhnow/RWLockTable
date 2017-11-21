//
// Created by iris on 10/19/17.
//

#ifndef PROJECT2_LINKEDLIST_H
#define PROJECT2_LINKEDLIST_H
#include "LinkedList.h"
#include "LLNode.h"
class LinkedList {
public:
    LinkedList();
    ~LinkedList();
    LLNode* push(long tid, int rid, bool isShared);

    bool tryLock(long tid, int rid, bool isShared);
    bool tryUnlock(long tid, int rid, bool isShared);
    bool eraseActiveLock(long tid, int rid, bool isShared);

    void print();
private:
    void erase(LLNode*);
    LLNode *head, *tail;
};

#endif //PROJECT2_LINKEDLIST_H
