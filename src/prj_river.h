#pragma once

#include <api.h>
#include "rtl/riverlib/river_cfg.h"
#include "rtl/riverlib/river_top.h"

using namespace sysvc;

class RiverProject : public ProjectObject {
 public:
    RiverProject(const char *rootpath);

 protected:
    FolderObject folder_rtl_;
    FolderObject folder_riverlib_;
    river_cfg file_river_cfg_;
    RiverTop module_RiverTop_;
};


