#include "api.h"

namespace sysvc {

GenObject::GenObject(int id, const char *name) {
    id_ = id;
    name_ = std::string(name);
    next_ = 0;
}

void GenObject::add_to_end(GenObject *p) {
    if (next_ == 0) {
        next_ = p;
    } else {
        GenObject *t = next_;
        while (t->next_) {
            t = t->next_;
        }
        t->next_ = p;
    }
}

}
