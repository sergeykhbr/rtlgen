#include <systemc.h>
#include <api_core.h>
#include "api.h"
#include "systemc_gen.h"

namespace sysvc {

GenObject *SCV_new_object(size_t sz) {
    GenObject *ret = reinterpret_cast<GenObject *>(malloc(sz));
    memset(ret, 0, sz);
    return ret;
}

void SCV_add_to_end(GenObject *parent, GenObject *child) {
    while (parent->next) {
        parent = parent->next;
    }
    parent->next = child;
}

ModuleObject *SCV_new_module(ModuleObject *parent,
                        const char *name) {

    ModuleObject *p = static_cast<ModuleObject *>(
                    SCV_new_object(sizeof(ModuleObject)));
    p->id = LIST_ID_MODULE;
    p->parent = parent;
    p->name.make_string(name);

    if (parent) {
        SCV_add_to_end(parent, p);
    }
    return p;
}

IoObject *SCV_new_module_io(ModuleObject *m,
                    int dir,
                    char *name,
                    int width,
                    const char *comment) {
    IoObject *p = static_cast<IoObject *>(
                    SCV_new_object(sizeof(IoObject)));

    p->id = LIST_ID_INPUT;
    p->dir = dir;
    p->name.make_string(name);
    p->width.make_int64(width);
    p->comment.make_string(comment);

    if (m->io == 0) {
        m->io = p;
    } else {
        SCV_add_to_end(m->io, p);
    }
    return p;
}

int SCV_generate_systemc(AttributeType *cfg, ModuleObject *m) {
    SystemCGenerator gen(*cfg);
    gen.generate(m);
    return 0;
}

}
