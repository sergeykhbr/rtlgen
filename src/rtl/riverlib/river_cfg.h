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
    Int32Param CFG_LOG2_CPU_MAX;
    Int32Param CFG_CPU_MAX;

    Int32Param REQ_MEM_TYPE_BITS;
};


