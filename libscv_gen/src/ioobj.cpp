#include "api.h"

namespace sysvc {

IoObject::IoObject(ModuleObject *parent,
                     int id,
                     const char *name,
                     ParamObject &width,
                     const char *comment) : GenObject(id, name) {
    width_value_ = static_cast<int>(width.getValue());
    width_name_ = width.getName();
    comment_ = std::string(comment);
    parent->add_io(this);
}

}
