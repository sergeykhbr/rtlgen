#pragma once

#include <inttypes.h>
#include <iostream>

class ParamObject;

class GenValue {
 public:
    virtual uint64_t getValue() = 0;
    virtual std::string generate_sysc() = 0;
};

class Int32Const : public GenValue {
 public:
    Int32Const(uint64_t v) : GenValue(), val_(v) {}

    virtual uint64_t getValue() { return val_; }
    virtual std::string generate_sysc();

 protected:
    uint64_t val_;
};

class Int32Pow2 : public GenValue {
 public:
    Int32Pow2(Int32Const *v) : GenValue(), val_(v) {}

    virtual uint64_t getValue() { return 1ull << val_->getValue(); }
    virtual std::string generate_sysc();

 protected:
    Int32Const *val_;
};

static Int32Const const_1(1);
static Int32Const const_2(2);
static Int32Const const_3(3);
