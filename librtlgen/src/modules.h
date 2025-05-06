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
#include "logic.h"
#include "structs.h"
#include <iostream>
#include <list>
#include <map>

namespace sysvc {

/**
 * Parent module class definition
 */
class ModuleObject : public GenObject {
 public:
    ModuleObject(GenObject *parent, const char *type,
                 const char *name, const char *comment);

    virtual void postInit() override;
    virtual std::string getType() override { return type_; }
    virtual std::string getName() override { return name_; }
    virtual bool isAsyncResetParam() override;
    virtual void registerModuleReg(GenObject *); // not a GenObject

    virtual void configureGenerator(ECfgGenType) override;
    virtual std::string generate() override;

    virtual std::string generate_sysc_h();
    virtual std::string generate_sysc_cpp();
    virtual std::string generate_sv_pkg();
    virtual std::string generate_sv_mod(bool no_pkg);
    virtual std::string generate_vhdl_pkg();
    virtual std::string generate_vhdl_mod();

    // Check registers in current module and all sub-modules to create
    // async_reset logic and always (ff) process
    virtual bool isModule() override { return true; }
    virtual bool isTypedef() override { return getName() == getType(); }
    virtual void getCombProcess(std::list<GenObject *> &proclist);
    virtual bool isSubModules();
    virtual void getTmplParamList(std::list<GenObject *> &genlist);
    virtual void getParamList(std::list<GenObject *> &genlist);
    virtual void getIoList(std::list<GenObject *> &genlist);
 protected:

    /**
        module without package: no_gendep = false && only_gendep = false
        module package:         no_gendep = true && only_gendep = false
        module with package:    no_gendep = false && only_gendep = true
   
     */
    std::string generate_sv_localparam(bool no_gendep, bool only_gendep);
    std::string generate_all_struct();
    std::string generate_sv_mod_genparam();     // generic parameters
    std::string generate_sv_mod_func(GenObject *func);
    std::string generate_all_mod_variables();

//    std::string generate_sysc_proc_registers();
    std::string generate_sysc_param_strings();
    std::string generate_sysc_constructor();
    std::string generate_sysc_submodule_nullify();
    std::string generate_sysc_destructor();
    std::string generate_sysc_vcd();
    std::string generate_sysc_template_param(GenObject *p);
    std::string generate_sysc_template_f_name(const char *rettype, bool with_class_name=true);
    std::string generate_sysc_sensitivity(GenObject *obj);
    std::string generate_sysc_vcd_entries(GenObject *obj);

    std::string generate_vhdl_pkg_localparam();
    std::string generate_vhdl_pkg_reg_struct();
    std::string generate_vhdl_pkg_struct();
    std::string generate_vhdl_mod_genparam();     // generic parameters
    std::string generate_vhdl_mod_param_strings();
    std::string generate_vhdl_mod_func(GenObject *func);
    std::string generate_vhdl_mod_signals();
    
 protected:
    std::string type_;
    std::string name_;
    char loopidx_[2];

    std::list<RegTypedefStruct *> sorted_regs_;
};

}  // namespace sysvc
