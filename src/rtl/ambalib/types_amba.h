#pragma once

#include <api.h>

using namespace sysvc;

class types_amba : public FileObject {
 public:
    types_amba(GenObject *parent);

 public:
    ParamI32 CFG_BUS_ADDR_WIDTH;
    EmptyLine emtn_;
};


