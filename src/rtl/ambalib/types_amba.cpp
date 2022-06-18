#include "types_amba.h"

types_amba::types_amba(GenObject *parent) :
    FileObject(parent, "types_amba"),
    CFG_BUS_ADDR_WIDTH(this, "CFG_BUS_ADDR_WIDTH", new I32D(64), ""),
    _1_(this)
{
}
