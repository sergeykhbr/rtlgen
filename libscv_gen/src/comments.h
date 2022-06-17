#pragma once

#include "genobjects.h"
#include <iostream>

namespace sysvc {

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

}  // namespace sysvc
