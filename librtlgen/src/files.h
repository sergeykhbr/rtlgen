#pragma once

#include "genobjects.h"
#include <iostream>

namespace sysvc {

class FileObject : public GenObject {
 public:
    FileObject(GenObject *parent,
                 const char *name);

    virtual std::string getFullPath() override;
    virtual std::string generate(EGenerateType) override;

 protected:
};

}  // namespace sysvc
