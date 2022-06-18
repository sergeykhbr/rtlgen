#pragma once

#include "genobjects.h"
#include <iostream>

namespace sysvc {

/**
 * Commentary insertion
 */

class TextLine : public GenObject {
 public:
    TextLine(GenObject *parent, const char *comment);
    TextLine(GenObject *parent);

    virtual std::string generate(EGenerateType) override;
 protected:
    bool empty_;
};

class CommentLicense : public GenObject {
 public:
    CommentLicense();

    virtual std::string generate(EGenerateType) override;
 protected:
    TextLine str01;
    TextLine str02;
    TextLine str03;
    TextLine str04;
    TextLine str05;
    TextLine str06;
    TextLine str07;
    TextLine str08;
    TextLine str09;
    TextLine str10;
    TextLine str11;
    TextLine str12;
    TextLine str13;
    TextLine str14;
    TextLine str15;
};

}  // namespace sysvc
