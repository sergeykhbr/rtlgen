// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "refobj.h"
#include "values.h"
#include <iostream>
#include <list>

namespace sysvc {

class StructObject : public GenValue {
 public:
    // Declare structure type and create on instance
    StructObject(GenObject *parent,
                 GenObject *clk,
                 EClockEdge edge,
                 GenObject *nrst,
                 EResetActive active,
                 const char *type,
                 const char *name,
                 const char *rstval,
                 const char *comment);
    StructObject(GenObject *parent,
                 const char *type,
                 const char *name,
                 const char *comment);

    /** GenObject generic methods */
    virtual std::string getName() override { return name_; }
    virtual std::string getType() override;
    virtual bool isStruct() override { return true; }
    virtual bool isTypedef() override;
    virtual bool isInterface() override;
    virtual std::string getStrValue() override;
    virtual bool is2Dim() override;

    virtual std::string generate() override;

 protected:
    virtual std::string generate_interface();
    virtual std::string generate_vector_type();
    virtual std::string generate_param();
    virtual std::string generate_interface_constructor();
    virtual std::string generate_interface_constructor_init();
    virtual std::string generate_interface_op_equal();      // operator ==
    virtual std::string generate_interface_op_assign();     // operator =
    virtual std::string generate_interface_op_stream();     // operator <<
    virtual std::string generate_interface_op_bracket();    // operator [] for vector only
    virtual std::string generate_interface_sc_trace();      // sc_trace

protected:
    std::string type_;
    std::string name_;
};

/**
    Generate inside of module structure of registers
 */
class RegTypedefStruct : public StructObject {
 public:
    RegTypedefStruct(GenObject *parent,
                        GenObject *clk,
                        EClockEdge edge,
                        GenObject *rst,
                        EResetActive active,
                        const char *suffix,
                        const char *type,
                        const char *rstval);

    std::string nameInModule(EPorts portid) override {
        if (portid == PORT_OUT) {
            return r_->getName();
        } 
        return v_->getName();
    }

    virtual void add_entry(GenObject *obj) override ;

    virtual GenObject *rst_instance() { return rst_; }
    virtual GenObject *v_instance() { return v_; }
    virtual GenObject *rin_instance() { return rin_; }
    virtual GenObject *r_instance() { return r_; }

    static std::string reg_suffix(GenObject *p, int unique_idx);

 protected:
    /**
        Generate rin, r, v signal instances:
     */
    class RegSignalInstance : public StructObject {
     public:
        RegSignalInstance(GenObject *parent,
                        RegTypedefStruct *p,
                        const char *name,
                        const char *rstval);
     protected:
        RegTypedefStruct *rstruct_;
    };

    class RegResetStruct : public RegSignalInstance {
     public:
        RegResetStruct(GenObject *parent,
                        RegTypedefStruct *p,
                        const char *name);

        //virtual bool isParam() override { return true; }
        virtual bool isConst() override { return true; }
    };

 protected:
    RegResetStruct *rst_;
    RegSignalInstance *v_;
    RegSignalInstance *rin_;
    RegSignalInstance *r_;
};


}  // namespace sysvc
