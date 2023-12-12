// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
// 
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
// 

#include "comments.h"
#include "utils.h"

namespace sysvc {

TextLine::TextLine(GenObject *parent, const char *comment)
    : GenObject(parent, comment) {
    empty_ = false;
}

TextLine::TextLine(GenObject *parent)
    : TextLine(parent, NO_COMMENT) {
    empty_ = true;
}

std::string TextLine::getStrValue() {
    std::string ret = "";
    if (!empty_) {
        if (SCV_is_vhdl()) {
            ret += "-- ";
        } else {
            ret += "// ";
        }
        ret += getComment();
    }
    return ret;
}

std::string TextLine::generate() {
    std::string ret = "";
    if (!empty_) {
        ret += addspaces();
        ret += getStrValue();
    }
    ret += "\n";
    return ret;
}


CommentLicense::CommentLicense() : GenObject(0, NO_COMMENT),
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

std::string CommentLicense::generate() {
    std::string ret = "";
    for (auto &p: getEntries()) {
        ret += p->generate();
    }
    return ret;
}

}
