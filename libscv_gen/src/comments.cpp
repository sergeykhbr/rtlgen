#include "comments.h"

namespace sysvc {

EmptyLine::EmptyLine(GenObject *parent)
    : GenObject(parent, ID_EMPTYLINE, "") {
}

std::string EmptyLine::generate(EGenerateType v) {
    std::string ret = "\n";
    return ret;
}


CommentObject::CommentObject(GenObject *parent, const char *comment)
    : GenObject(parent, ID_COMMENT, comment) {
}

std::string CommentObject::generate(EGenerateType v) {
    std::string ret = "// " + getName() + "\n";
    return ret;
}


CommentLicense::CommentLicense()
    : CommentObject(0, ""),
    str01(this, ""),
    str02(this, " Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com"),
    str03(this, ""),
    str04(this, " Licensed under the Apache License, Version 2.0 (the \"License\");"),
    str05(this, " you may not use this file except in compliance with the License."),
    str06(this, " You may obtain a copy of the License at"),
    str07(this, ""),
    str08(this, "     http://www.apache.org/licenses/LICENSE-2.0"),
    str09(this, ""),
    str10(this, " Unless required by applicable law or agreed to in writing, software"),
    str11(this, " distributed under the License is distributed on an \"AS IS\" BASIS,"),
    str12(this, " WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied."),
    str13(this, " See the License for the specific language governing permissions and"),
    str14(this, " limitations under the License."),
    str15(this, "") {
}

std::string CommentLicense::generate(EGenerateType v) {
    std::string ret = "";
    for (auto &p: entries_) {
        ret += "// " + p->getName() + "\n";
    }
    return ret;
}

}
