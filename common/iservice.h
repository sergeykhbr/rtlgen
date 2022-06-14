#pragma once

#include <iface.h>

namespace sysvc {

const char *IFACE_SERVICE = "IService";

class IService : public IFace {
 public:
    IService(const char name) : IFace(IFACE_SERVICE) {}

    virtual const char *getObjName() { return "stub"; }
};

extern "C" IFace *RISCV_get_service(const char *name) { return 0; }

}  // namespace sysvc