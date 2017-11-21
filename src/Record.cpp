//
// Created by iris on 10/20/17.
//
#include "Record.h"

Record::Record() : val(100) {} // 100 is inital value
long long Record::read() {
    return val;
}
long long Record::write(long long val) {
    return (this->val = val);
}
