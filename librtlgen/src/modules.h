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
#include <iostream>
#include <list>

namespace sysvc {

/**
 * Parent module class definition
 */
class ModuleObject : public GenObject {
 public:
    ModuleObject(GenObject *parent, const char *type, const char *name="");

    virtual std::string getType() { return type_; }
    virtual std::string generate_sysc_h();
    virtual std::string generate_sysc_cpp();
    virtual std::string generate_sv_pkg();
    virtual std::string generate_sv_mod();

    // Check registers in current module and all sub-modules to create
    // async_reset logic and always (ff) process
    virtual bool isCombProcess();
    virtual bool isRegProcess();
    virtual bool isNRegProcess();
    virtual bool is2DimReg();
    virtual bool isSubModules();
    virtual bool isFileValue();
    virtual void getTmplParamList(std::list<GenObject *> &genlist);
    virtual void changeTmplParameter(const char *name, const char *val);
    virtual void getParamList(std::list<GenObject *> &genlist);
    virtual void getIoList(std::list<GenObject *> &genlist);
 protected:
    std::string generate_sv_pkg_localparam();
    std::string generate_sv_pkg_reg_struct(bool negedge);
    std::string generate_sv_pkg_struct();
    std::string generate_sv_mod_genparam();     // generic parameters
    std::string generate_sv_mod_param_strings();
    std::string generate_sv_mod_func(GenObject *func);
    std::string generate_sv_mod_signals();
    std::string generate_sv_mod_proc(GenObject *proc);
    std::string generate_sv_mod_proc_registers();
    std::string generate_sv_mod_always_ff_rst(bool clkpos=true);
    std::string generate_sv_mod_always_ops();   // additional operation in always process (if defined)

    std::string generate_sysc_reg_struct(bool negedge=false);
    std::string generate_sysc_proc_registers(bool clkpos=true);
    std::string generate_sysc_param_strings();
    std::string generate_sysc_constructor();
    std::string generate_sysc_submodule_nullify();
    std::string generate_sysc_destructor();
    std::string generate_sysc_vcd();
    std::string generate_sysc_func(GenObject *func);
    std::string generate_sysc_proc(GenObject *proc);
    std::string generate_sysc_template_param(GenObject *p);
    std::string generate_sysc_template_f_name(const char *rettype="void");
    std::string generate_sysc_sensitivity(std::string prefix, std::string name, GenObject *obj);
    std::string generate_sysc_vcd_entries(std::string name1, std::string name2, GenObject *obj);

 protected:
};

}  // namespace sysvc
