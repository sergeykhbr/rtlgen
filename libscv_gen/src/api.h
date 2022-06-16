#pragma once

#include <iostream>

namespace sysvc {

enum EIdType {
    ID_PROJECT,
    ID_FOLDER,
    ID_HEAD_FILE,
    ID_CONST_PARAM,
    ID_INT32_PARAM,
    ID_INT64_PARAM,
    ID_MODULE,
    ID_INPUT,
    ID_OUTPUT,
    ID_COMMENT,
};

class ModuleObject;

class GenObject {
 public:
    GenObject(int id, const char *name);

    void add_to_end(GenObject *p);

    unsigned getId() { return id_; }
    std::string getName() { return name_; }
    GenObject *getNext() { return next_; }

    virtual std::string generate_sysc() { return std::string(""); }

 protected:
    unsigned id_;
    std::string name_;
    GenObject *next_;
};

class ProjectObject : public GenObject {
 public:
    ProjectObject(const char *name,
                  const char *rootpath,
                  const char *comment);

    void add_item(GenObject *item);
    std::string getRootPath() { return rootpath_; }
    std::string getComment() { return comment_; }

    virtual std::string generate_sysc() override;

 protected:
    GenObject *items_;          // Folders and files
    std::string rootpath_;
    std::string comment_;
};

class FolderObject : public GenObject {
 public:
    FolderObject(GenObject *parent,
                 const char *name);

    bool is_root();
    void add_subfolder(GenObject *f);
    void add_file(GenObject *f);
    std::string getFullPath();

    virtual std::string generate_sysc() override;

 protected:
    GenObject *parent_;
    GenObject *files_;
};

class HeadFileObject : public GenObject {
 public:
    HeadFileObject(GenObject *parent,
               const char *name,
               const char *comment);

    bool is_root();
    void add_entry(GenObject *entry);
    std::string getFullPath();

    virtual std::string generate_sysc() override;

 protected:
    GenObject *parent_;
    GenObject *entries_;
    std::string comment_;
};


class ParamObject : public GenObject {
 public:
    ParamObject(GenObject *parent,
                const char *name,
                int type,
                const char *comment);

    virtual int64_t getValue() = 0;
    virtual std::string getValueStr() { return value_str_; }
    std::string getComment() { return comment_; }

    virtual std::string generate_sysc() override;

 protected:
    GenObject *parent_;
    int type_;
    std::string value_str_;
    std::string comment_;
};

class ConstParam : public ParamObject {
 public:
    ConstParam(int value);
    virtual int64_t getValue() { return value_; }
 protected:
    int value_;
};

class Int32Param : public ParamObject {
 public:
    Int32Param(GenObject *parent,
                const char *name,
                int value,
                const char *comment);
    Int32Param(GenObject *parent,
                const char *name,
                const char *value_str,
                int value,
                const char *comment);
    virtual int64_t getValue() { return value_; }
 protected:
    int32_t value_;
};


/*
 * Input/Output ports of the modules
 */
class IoObject : public GenObject {
 public:
    IoObject(ModuleObject *parent,
             int id,
             const char *name,
             ParamObject &width,
             const char *comment);

    int getWidthInt() { return width_value_; }
    std::string getWidthStr() { return width_name_; }
    std::string getComment() { return comment_; }

 protected:
    int width_value_;
    std::string width_name_;
    std::string comment_;
};

class InPort : public IoObject {
 public:
    InPort(ModuleObject *parent, const char *name,
             ParamObject &width, const char *comment)
        : IoObject(parent, ID_INPUT, name, width, comment) {}
};

class OutPort : public IoObject {
 public:
    OutPort(ModuleObject *parent, const char *name,
             ParamObject &width, const char *comment)
        : IoObject(parent, ID_OUTPUT, name, width, comment) {}
};


/**
 * Commentary insertion
 */
class CommentObject : public GenObject {
 public:
    CommentObject(GenObject *parent, const char *comment);

    virtual std::string generate_sysc() override;
};


/**
 * Parent module class definition
 */
class ModuleObject : public GenObject {
 public:
    ModuleObject(GenObject *parent, const char *name);

    void add_io(IoObject *io);
    void add_param(ParamObject *param);
    void add_submodule(ModuleObject *child);

    IoObject *getIo() { return io_; }

    virtual std::string generate_sysc() override;
 protected:
    void generate_sysc_h();

 protected:
    GenObject *parent_;
    IoObject *io_;
    ParamObject *parameters_;
    ModuleObject *submodules_;
    std::string fullpath_;
};

#define RISCV_free free
#define RISCV_malloc malloc
#define RISCV_sprintf sprintf_s
#define RISCV_printf printf
#define LOG_ERROR 1

#ifdef _WIN32
    #define RV_PRI64 "I64"
#else
#endif


int SCV_is_dir_exists(const char *path);
void SCV_create_dir(const char *path);
void SCV_write_file(const char *fname, const char *buf, size_t sz);


};
