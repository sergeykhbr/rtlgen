#include "projects.h"
#include "utils.h"

namespace sysvc {

ProjectObject::ProjectObject(const char *name,
                         const char *rootpath,
                         const char *comment)
    : GenObject(0, ID_PROJECT, name) {
    rootpath_ = std::string(rootpath);
    comment_ = std::string(comment);
}


std::string ProjectObject::generate(EGenerateType v) {
    if (!SCV_is_dir_exists(rootpath_.c_str())) {
        SCV_create_dir(rootpath_.c_str());
    }

    GenObject *p = getChilds();
    while (p) {
        p->generate(v);
        p = p->getChilds();
    }

    return GenObject::generate(v);
}

}  // namespace sysvc

