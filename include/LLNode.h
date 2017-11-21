//
// Created by iris on 10/19/17.
//

#ifndef PROJECT2_LLNODE_H
#define PROJECT2_LLNODE_H
#include <condition_variable>

/* class LLNode does not have any private methods and varaible
to avoid code being too long */
class LLNode {
public:

    /*LLNode* getPrv();
    void setPrv(LLNode *prv);

    LLNode* getNxt();
    void setNxt(LLNode *nxt);*/
    LLNode(); //for head
    LLNode(long tid, int rid, bool isShared);
    LLNode(LLNode *other);
    void print();
//private:
    LLNode *prv, *nxt;
    long tid; //thread id (starts 0)
    int rid; //record id
    bool isShared; // false - eXclusive lock(write)
    bool isAcquired;
    bool operator=(const LLNode &other);
    long long bfsOrder; // please see bfsGlobalOrder
    std::condition_variable cv;
};
#endif //PROJECT2_LLNODE_H
