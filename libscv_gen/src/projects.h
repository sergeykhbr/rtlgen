#pragma once

#include "genobjects.h"
#include <iostream>

namespace sysvc {

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
    std::string rootpath_;
    std::string comment_;
};

}  // namespace sysvc
