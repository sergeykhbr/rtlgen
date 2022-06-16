#include "api.h"

namespace sysvc {

CommentObject::CommentObject(GenObject *parent, const char *comment)
    : GenObject(ID_COMMENT, comment) {
    if (parent->getId() == ID_HEAD_FILE) {
        reinterpret_cast<HeadFileObject *>(parent)->add_entry(this);
    } else if (parent->getId() == ID_MODULE) {
        //reinterpret_cast<ModuleObject *>(parent)->add_entry(this);
    }
}

std::string CommentObject::generate_sysc() {
    std::string ret = "// " + getName() + "\n";
    return ret;
}

}
