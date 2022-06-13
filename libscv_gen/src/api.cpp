#include <systemc.h>
#include <api_core.h>
#include "api.h"

namespace sysvc {

using namespace debugger;

vc_module::vc_module(const char *name)
{
    cfg_.make_list(module_attr_total);
    cfg_[module_name].make_string(name);
    cfg_[module_childs].make_list(0);
    cfg_[module_in].make_list(0);
    cfg_[module_out].make_list(0);
    cfg_[module_localparam].make_list(0);
    cfg_[module_reg].make_list(0);
    cfg_[module_wire].make_list(0);
}

void vc_module::register_input(const char *name, int width) {
    AttributeType &i = cfg_[module_in].new_list_item();
    i.make_list(in_attr_total);
    i[in_name].make_string(name);
    i[in_width].make_int64(width);
}

void vc_module::register_output(const char *name, int width) {
    AttributeType &o = cfg_[module_out].new_list_item();
    o.make_list(out_attr_total);
    o[out_name].make_string(name);
    o[out_width].make_int64(width);
}

void vc_module::generate_sc(char *buf, size_t sz) {
    generate_sc_h(buf, sz);
    generate_sc_cpp(buf, sz);
}

void vc_module::generate_sc_h(char *buf, size_t sz) {
    size_t pos = 0;
    pos += RISCV_sprintf(&buf[pos], sz - pos, "%s",
"/*\n"
" *  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com\n"
" *\n"
" *  Licensed under the Apache License, Version 2.0 (the \"License\");\n"
" *  you may not use this file except in compliance with the License.\n"
" *  You may obtain a copy of the License at\n"
" *\n"
" *      http://www.apache.org/licenses/LICENSE-2.0\n"
" *\n"
" *  Unless required by applicable law or agreed to in writing, software\n"
" *  distributed under the License is distributed on an \"AS IS\" BASIS,\n"
" *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
" *  See the License for the specific language governing permissions and\n"
" *  limitations under the License.\n"
" */\n"
" \n"
"#pragma once\n");
}

void vc_module::generate_sc_h_include(char *buf, size_t sz, size_t pos) {
    pos += RISCV_sprintf(&buf[pos], sz - pos, "%s", "#include <systemc.h>\n");
    AttributeType &childs = cfg_[module_childs];
    for (unsigned i = 0; i < childs.size(); i++) {
    }
}

void vc_module::generate_sc_cpp(char *buf, size_t sz) {
}

}
