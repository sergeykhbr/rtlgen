#include "prj_river.h"

RiverProject::RiverProject(const char *rootpath) :
    ProjectObject("river_v3", rootpath, "River v3.0 CPU generator"),
    folder_rtl_(this, "rtl"),
    folder_riverlib_(&folder_rtl_, "riverlib"),
    file_river_cfg_(&folder_riverlib_),
    module_RiverTop_(&folder_riverlib_, file_river_cfg_)
{
}