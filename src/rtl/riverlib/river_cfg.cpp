#include "river_cfg.h"

river_cfg::river_cfg(GenObject *parent) :
    HeadFileObject(parent, "river_cfg", "CPU top level configuration"),
    cmt1_(this, ""),
    cmt2_(this, "2**Number of CPU slots in the clusters. Some of them could be unavailable"),
    cmt3_(this, ""),
    CFG_LOG2_CPU_MAX(this, "CFG_LOG2_CPU_MAX", 2, ""),
    CFG_CPU_MAX(this, "CFG_CPU_MAX", "1 << CFG_LOG2_CPU_MAX", 1 << CFG_LOG2_CPU_MAX.getValue(), ""),
    REQ_MEM_TYPE_BITS(this, "REQ_MEM_TYPE_BITS", 3, "")
{
}
