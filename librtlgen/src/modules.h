#pragma once

#include "genobjects.h"
#include <iostream>

namespace sysvc {

/**
 * Parent module class definition
 */
class ModuleObject : public GenObject {
 public:
    ModuleObject(GenObject *parent, const char *name);

    virtual std::string generate(EGenerateType v) override;
 protected:
    std::string generate_sysc_h();
};

}  // namespace sysvc
