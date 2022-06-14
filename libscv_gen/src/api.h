#pragma once

#include <attribute.h>

namespace sysvc {

enum EListIdType {
    LIST_ID_MODULE,
    LIST_ID_INPUT,
    LIST_ID_OUTPUT,
};

static const int IO_DIR_INPUT = 0x1;
static const int IO_DIR_OUTPUT = 0x2;


struct GenObject {
    unsigned id;
    GenObject *next;
};

struct IoObject : public GenObject {
    AttributeType name;
    int dir;
    AttributeType width;        // width integer or string
    AttributeType comment;
};

struct ModuleObject : public GenObject {
    AttributeType name;
    GenObject *parent;
    GenObject *childs;
    IoObject *io;
};


ModuleObject *SCV_new_module(ModuleObject *parent,
                            const char *name);

IoObject *SCV_new_module_io(ModuleObject *m,
                            int dir,
                            char *name,
                            int width,
                            const char *comment);

int SCV_generate_systemc(AttributeType *cfg,
                         ModuleObject *m);

};
