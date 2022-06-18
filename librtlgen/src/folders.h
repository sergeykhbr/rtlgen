#pragma once

#include "genobjects.h"
#include <iostream>

namespace sysvc {

class FolderObject : public GenObject {
 public:
    FolderObject(GenObject *parent,
                 const char *name);

    virtual std::string getFullPath() override;
    virtual std::string generate(EGenerateType) override;

 protected:
};

}  // namespace sysvc
