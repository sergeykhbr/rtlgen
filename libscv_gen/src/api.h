#pragma once

#include <attribute.h>

namespace sysvc {

enum EModuleAttributes {
    module_name,
    module_childs,
    module_in,
    module_out,
    module_localparam,
    module_reg,
    module_wire,
    module_attr_total
};

enum EInputAttributes {
    in_name,
    in_width,
    in_attr_total
};

enum EOutputAttributes {
    out_name,
    out_width,
    out_attr_total
};


class vc_module
{
 public:
    vc_module(const char *name);

    void register_input(const char *name, int width);
    void register_output(const char *name, int width);

    void generate_sc(char *buf, size_t sz);
    void generate_sc_h(char *buf, size_t sz);
    void generate_sc_h_include(char *buf, size_t sz, size_t pos);
    void generate_sc_cpp(char *buf, size_t sz);

 protected:
    debugger::AttributeType cfg_;
};

};
