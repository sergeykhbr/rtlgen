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

    bool is_root();
    std::string getFullPath();

    virtual std::string generate_sysc() override;

 protected:
};

class HeadFileObject : public GenObject {
 public:
    HeadFileObject(GenObject *parent,
               const char *name,
               const char *comment);

    bool is_root();
    std::string getFullPath();

    virtual std::string generate_sysc() override;

 protected:
    std::string comment_;
};




/*
 * Simple wires
 */



/**
 * Commentary insertion
 */

class EmptyLine : public GenObject {
 public:
    EmptyLine(GenObject *parent);

    virtual std::string generate_sysc() override;
};

class CommentObject : public GenObject {
 public:
    CommentObject(GenObject *parent, const char *comment);

    virtual std::string generate_sysc() override;
};

class CommentLicense : public CommentObject {
 public:
    CommentLicense();

    virtual std::string generate_sysc() override;
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

    virtual std::string generate_sysc() override;
 protected:
    void generate_sysc_h();

 protected:
    std::string fullpath_;
};


};
