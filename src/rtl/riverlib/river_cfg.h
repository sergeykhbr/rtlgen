#pragma once

#include <api.h>

using namespace sysvc;

class river_cfg : public HeadFileObject {
 public:
    river_cfg(GenObject *parent);

 public:
    CommentObject cmt1_;
    CommentObject cmt2_;
    CommentObject cmt3_;
    ParamI32 CFG_LOG2_CPU_MAX;
    ParamI32 CFG_CPU_MAX;
    EmptyLine emt1_;
    ParamUI64 CFG_VENDOR_ID;
    ParamUI64 CFG_IMPLEMENTATION_ID;


    ParamI32 REQ_MEM_TYPE_BITS;
    EmptyLine emtn_;
};


