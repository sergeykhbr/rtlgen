#pragma once

#include <attribute.h>

namespace sysvc {

enum EIdType {
    ID_CFG_PARAM,
    ID_MODULE,
    ID_INPUT,
    ID_OUTPUT,
    ID_IO_COMMENT,  // comment string between port
};

class ModuleObject;

class GenObject {
 public:
    GenObject(int id, const char *name);

    void add_to_end(GenObject *p);

    unsigned getId() { return id_; }
    const char *getName() { return name_.to_string(); }
    GenObject *getNext() { return next_; }

 protected:
    unsigned id_;
    AttributeType name_;
    GenObject *next_;
};

class CfgParamObject : public GenObject {
 public:
    CfgParamObject(ModuleObject *parent,
                   const char *name,
                   int type,
                   int width,
                   uint64_t value,
                   const char *comment);

    int getWidth() { return width_; }
    uint64_t getValue() { return value_; }
    const char *getComment() { return comment_.to_string(); }

 protected:
    int type_;
    int width_;
    uint64_t value_;
    AttributeType comment_;
};

class CfgParamFileObject : public GenObject {
 public:
    CfgParamFileObject(ModuleObject *parent,
                   const char *name);

 protected:
    CfgParamObject *param_;
};

class IoObject : public GenObject {
 public:
    IoObject(ModuleObject *parent,
             int id,
             const char *name,
             int width,
             const char *comment);

    int getWidthInt() { return width_int_; }
    const char *getWidthStr() { return width_str_.to_string(); }
    const char *getComment() { return comment_.to_string(); }

 protected:
    int width_int_;             // width integer
    AttributeType width_str_;   // width string
    AttributeType comment_;
};

class InPort : public IoObject {
 public:
    InPort(ModuleObject *parent, const char *name,
             int width, const char *comment)
        : IoObject(parent, ID_INPUT, name, width, comment) {}
};

class OutPort : public IoObject {
 public:
    OutPort(ModuleObject *parent, const char *name,
             int width, const char *comment)
        : IoObject(parent, ID_OUTPUT, name, width, comment) {}
};

class CommentIo : public IoObject {
 public:
    CommentIo(ModuleObject *parent, const char *comment)
        : IoObject(parent, ID_IO_COMMENT, "", 0, comment) {}
};


class ModuleObject : public GenObject {
 public:
    ModuleObject(ModuleObject *parent, const char *name);

    void add_child(ModuleObject *child);
    void add_io(IoObject *io);

    IoObject *getIo() { return io_; }

 protected:
    ModuleObject *parent_;
    ModuleObject *child_;
    IoObject *io_;
};


int SCV_generate_systemc(AttributeType *cfg,
                         ModuleObject *m);

};
