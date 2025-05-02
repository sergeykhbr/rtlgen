// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
// 
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
// 

#pragma once

#include "genobjects.h"

namespace sysvc {

class RefObject : public GenObject {
 public:
    RefObject(GenObject *parent, GenObject *ref, const char *comment)
        : GenObject(parent, comment), ref_(ref) {
    }

    virtual std::string getType() override { return ref_->getType(); }
    virtual std::string getName() override { return ref_->getName(); }
    virtual std::string getTypedef() override { return ref_->getTypedef(); }
    virtual std::string nameInModule(EPorts portid) override;

    virtual uint64_t getValue() { return ref_->getValue(); }
    virtual double getFloatValue() { return ref_->getFloatValue(); }
    virtual std::string getStrValue() { return ref_->getStrValue(); }
    virtual GenObject *getObjValue() { return ref_->getObjValue(); }

    virtual uint64_t getWidth() { return ref_->getWidth(); }
    virtual std::string getStrWidth() { return ref_->getStrWidth(); }
    virtual GenObject *getObjWidth() { return ref_->getObjWidth(); }
    
    virtual uint64_t getDepth() { return ref_->getDepth(); }
    virtual GenObject *getObjDepth() { return ref_->getObjDepth(); }
    virtual std::string getStrDepth() { return ref_->getStrDepth(); }

    virtual void setSelector(GenObject *sel) { ref_->setSelector(sel); }
    virtual GenObject *getSelector() { return ref_->getSelector(); }

    virtual bool is2Dim() override { return ref_->is2Dim(); }
 protected:
    GenObject *ref_;
};

/**
    Generate reset structure/function (*_r_reset)
    */
class RefResetObject : public RefObject {
    public:
    RefResetObject(GenObject *parent, GenObject *ref, const char *comment)
        : RefObject(parent, ref, comment) {
    }

    virtual std::string nameInModule(EPorts portid) override;
};


}  // namespace sysvc