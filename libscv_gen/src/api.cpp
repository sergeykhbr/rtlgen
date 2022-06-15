#include <systemc.h>
#include <api_core.h>
#include "api.h"
#include "systemc_gen.h"

namespace sysvc {

GenObject::GenObject(int id, const char *name) {
    id_ = id;
    name_.make_string(name);
    next_ = 0;
}

void GenObject::add_to_end(GenObject *p) {
    if (next_ == 0) {
        next_ = p;
    } else {
        GenObject *t = next_;
        while (t->next_) {
            t = t->next_;
        }
        t->next_ = p;
    }
}

CfgParamObject::CfgParamObject(ModuleObject *parent,
                               const char *name,
                               int type,
                               int width,
                               uint64_t value,
                               const char *comment)
    : GenObject(ID_CFG_PARAM, name) {
    type_ = type;
    value_ = value;
    comment_.make_string(comment);
}

ModuleObject::ModuleObject(ModuleObject *parent, const char *name) :
    GenObject(ID_MODULE, name) {
    parent_ = parent;
    child_ = 0;
    io_ = 0;
}

void ModuleObject::add_child(ModuleObject *child) {
    if (child_) {
        child_->add_child(child);
    } else {
        child_ = child;
    }
}

void ModuleObject::add_io(IoObject *io) {
    if (io_) {
        io_->add_to_end(io);
    } else {
        io_ = io;
    }
}

IoObject::IoObject(ModuleObject *parent,
                     int id,
                     const char *name,
                     int width,
                     const char *comment) : GenObject(id, name) {
    width_int_ = width;
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", width);
    width_str_.make_string(tstr);
    comment_.make_string(comment);
    parent->add_io(this);
}


int SCV_generate_systemc(AttributeType *cfg, ModuleObject *m) {
    SystemCGenerator gen(*cfg);
    gen.generate(m);
    return 0;
}

}
