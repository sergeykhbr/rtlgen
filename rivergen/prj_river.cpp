#include "prj_river.h"

RiverProject::RiverProject(const char *rootpath) :
    ProjectObject("river_v3", rootpath, "River v3.0 CPU generator"),
    folder_rtl_(this, "rtl"),
    folder_ambalib_(&folder_rtl_, "ambalib"),
    file_types_amba_(&folder_ambalib_),
    folder_riverlib_(&folder_rtl_, "riverlib"),
    file_river_cfg_(&folder_riverlib_),
    file_river_top_(&folder_riverlib_)
{
}