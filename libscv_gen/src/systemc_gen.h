#pragma once

#include <attribute.h>
#include "api.h"

namespace sysvc {

class SystemCGenerator
{
 public:
    SystemCGenerator(AttributeType &cfg);
    virtual ~SystemCGenerator();

    void generate(ModuleObject *m);

 protected:
    void generate_h_module(ModuleObject *m);

    void add_string(const char *str);
    void add_dec(int v);
    void write_file(const char *fname);

 protected:
    AttributeType cfg_;
    AttributeType ns_;
    size_t pos_;
    size_t sz_;
    char *out_;
};

}  // namespace sysvc

