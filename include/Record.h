//
// Created by iris on 10/19/17.
//

#ifndef PROJECT2_RECORD_H
#define PROJECT2_RECORD_H
class Record {
public:
    Record();
    long long read(); //getter of val - implicit Shared Lock required
    long long write(long long); //setter of idx - implicit Exclusive Lock required

private:
    long long val;

};


#endif //PROJECT2_RECORD_H
