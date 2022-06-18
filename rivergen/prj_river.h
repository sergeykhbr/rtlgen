#pragma once

#include <api.h>
#include "rtl/ambalib/types_amba.h"
#include "rtl/riverlib/river_cfg.h"
#include "rtl/riverlib/river_top.h"

using namespace sysvc;

class RiverProject : public ProjectObject {
 public:
    RiverProject(const char *rootpath);

 protected:
    FolderObject folder_rtl_;
    FolderObject folder_ambalib_;
    types_amba file_types_amba_;
    FolderObject folder_riverlib_;
    river_cfg file_river_cfg_;
    river_top file_river_top_;
};


