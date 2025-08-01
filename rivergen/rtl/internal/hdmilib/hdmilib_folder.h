// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#pragma once

#include <api_rtlgen.h>
#include "video_sync.h"
#include "framebuf.h"
#include "rgb2ycbcr.h"
#include "hdmi_top.h"

class hdmilib_folder : public FolderObject {
 public:
    hdmilib_folder(GenObject *parent) :
        FolderObject(parent, "hdmilib"),
        video_sync_file_(this),
        framebuf_file_(this),
        rgb2ycbcr_file_(this),
        hdmi_top_file_(this) {}

 protected:
    video_sync_file video_sync_file_;
    framebuf_file framebuf_file_;
    rgb2ycbcr_file rgb2ycbcr_file_;
    hdmi_top_file hdmi_top_file_;
};
