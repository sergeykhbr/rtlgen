#include "river_cfg.h"

river_cfg::river_cfg(GenObject *parent) :
    HeadFileObject(parent, "river_cfg", "CPU top level configuration"),
    cmt1_(this, ""),
    cmt2_(this, "2**Number of CPU slots in the clusters. Some of them could be unavailable"),
    cmt3_(this, ""),
    CFG_LOG2_CPU_MAX(this, "CFG_LOG2_CPU_MAX", new I32D(2), ""),
    CFG_CPU_MAX(this, "CFG_CPU_MAX", new I32D("POW2(1,CFG_LOG2_CPU_MAX)"), ""),
    emt1_(this),
    CFG_VENDOR_ID(this, "CFG_VENDOR_ID", new UI64H("0x000000F1"), ""),
    CFG_IMPLEMENTATION_ID(this, "CFG_IMPLEMENTATION_ID", new UI64H("0x20191123"), ""),

    REQ_MEM_TYPE_BITS(this, "REQ_MEM_TYPE_BITS", new I32D(3), ""),
    emtn_(this)
{
}
