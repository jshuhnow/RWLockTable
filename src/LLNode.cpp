//
// Created by iris on 10/20/17.
//
#include "LLNode.h"
#include <iostream>
LLNode::LLNode() :
    prv(nullptr), nxt(nullptr), bfsOrder(0), isAcquired(false){}
LLNode::LLNode(long tid, int rid, bool isShared) :
    tid(tid), rid(rid), isShared(isShared),
    prv(nullptr), nxt(nullptr), bfsOrder(0), isAcquired(false) {}
LLNode::LLNode(LLNode *other) :
    prv(other->prv),
    nxt(other->nxt),
    tid(other->tid),
    rid(other->rid),
    isShared(other->isShared),
    bfsOrder(0) {}

// for debugging
// Time Complexity O(1)
void LLNode::print() {
    std::cout << "[" <<tid <<", "
    << (isShared?"S":"X") << ", "
    << (isAcquired?"A":"W") <<"]";
}
//compare only tid, rid, isShared
bool LLNode::operator=(const LLNode &other) {
    return tid == other.tid && rid == other.rid && isShared == other.isShared;
}
