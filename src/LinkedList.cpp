//
// Created by iris on 10/20/17.
//
#include "LLNode.h"
#include "LinkedList.h"
#include <condition_variable>
#include <cassert>
#include <iostream>

/*
Structure of LinkedList
i) empty
[head == tail] -> nullptr
ii) K elements
[head] -> [element 1] -> [element 2] -> ... -> [element K == tail] -> nullptr
*/
LinkedList::LinkedList() {
    //tid and rid of head are -1 for safety.
    head = new LLNode(-1, -1, true);

    //Initially, head = tail
    tail = head;
}

/* Deallocate nodes */
// Time Complexity O(N)
LinkedList::~LinkedList() {
    LLNode *cur = head, *t;

    // iterate forward (front to back)
    while(cur) {
        t = cur->nxt;
        delete cur;
        cur = t;
    }
}

// Time Complexity O(1)
LLNode* LinkedList::push(long tid, int rid, bool isShared) {
    tail->nxt = new LLNode(tid, rid, isShared);
    tail->nxt->prv = tail;
    tail = tail->nxt;
    return tail;
}


void LinkedList::erase(LLNode *ptr) {
    // link prv to nxt
    ptr->prv->nxt = ptr->nxt;

    // link nxt to prv if exists
    if (ptr->nxt) {
        ptr->nxt->prv = ptr->prv;
    } else {
        // ptr should have been tail. now it should be the prvious one.
        tail = ptr->prv;
    }
    delete ptr;
}

// return true if found, false otherwise
bool LinkedList::eraseActiveLock(long tid, int rid, bool isShared) {
    LLNode *cur = head->nxt;
    while(cur) {
        if (cur->tid == tid && cur->rid == rid && cur->isShared == isShared) {
            erase(cur);
            return true;
        }
        cur = cur->nxt;
    }
    return false;
}

// Time Complexity O(N)
// unlock acquired lock (serially called by single thread after transaction is completed)
// return true if found, false otherwise
bool LinkedList::tryUnlock(long tid, int rid, bool isShared) {

    if (isShared == false) {
        // Exclusive Lock
        if (head->nxt &&
            head->nxt->isAcquired &&
            head->nxt->isShared == false) {
            erase(head->nxt);
            return true;
        }
        else {
            return false;
        }
    } else {
        // Shared Lock
        LLNode *cur = head->nxt;
        while(cur) {

            // It's okay even if there are preceding shared locks.
            if (cur->isShared == false) return false;

            if (cur->tid == tid &&
                cur->rid == rid &&
                cur->isShared == isShared) {
                // Found one
                erase(cur);
                return true;
            }
            cur = cur->nxt;
        }
    }
    // not found
    return false;
}

// Acquire pending lock
// Time Complexity O(N)
// return true if found, false otherwise
bool LinkedList::tryLock(long tid, int rid, bool isShared) {
    LLNode *cur = head->nxt;
    if (isShared) {
        // Shared Lock
        while(cur) {
            if (cur->isShared) {
                if (cur->tid == tid &&
                cur->rid == rid) {
                    return (cur->isAcquired = true);
                }
            } else {
                // If there exists a preceding eXclusive lock, discard.
                return false;
            }
            cur = cur->nxt;
        }
        // Not Found
        return false;
    } else {
        // eXclusive Lock
        // Check only the first lock()'front')
        if (cur &&
            cur->tid == tid &&
            cur->rid == rid &&
            cur->isShared == isShared) {

            return (cur->isAcquired = true);
        } else {
            // Not Found
            return false;
        }
    }
}
// for debugging
// Time Complexity O(N)
void LinkedList::print() {
    LLNode *cur = head->nxt;
    while(cur) {
        cur->print(); std::cout << "->";
        cur = cur->nxt;
    }
}
