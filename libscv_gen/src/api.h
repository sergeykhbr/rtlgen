#pragma once

#include <iostream>
#include "genobjects.h"
#include "projects.h"
#include "values.h"
#include "params.h"
#include "ports.h"

namespace sysvc {

class FolderObject : public GenObject {
 public:
    FolderObject(GenObject *parent,
                 const char *name);

    virtual std::string getFullPath() override;
    virtual std::string generate(EGenerateType) override;

 protected:
};

class FileObject : public GenObject {
 public:
    FileObject(GenObject *parent,
                 const char *name);

    virtual std::string getFullPath() override;
    virtual std::string generate(EGenerateType) override;

 protected:
};



/**
 * Commentary insertion
 */

class EmptyLine : public GenObject {
 public:
    EmptyLine(GenObject *parent);

    virtual std::string generate(EGenerateType) override;
};

class CommentObject : public GenObject {
 public:
    CommentObject(GenObject *parent, const char *comment);

    virtual std::string generate(EGenerateType) override;
};

class CommentLicense : public CommentObject {
 public:
    CommentLicense();

    virtual std::string generate(EGenerateType) override;
 protected:
    CommentObject str01;
    CommentObject str02;
    CommentObject str03;
    CommentObject str04;
    CommentObject str05;
    CommentObject str06;
    CommentObject str07;
    CommentObject str08;
    CommentObject str09;
    CommentObject str10;
    CommentObject str11;
    CommentObject str12;
    CommentObject str13;
    CommentObject str14;
    CommentObject str15;
};


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


};
