// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "pcie_cfg.h"

pcie_cfg *pcie_cfg_ = 0;

pcie_cfg::pcie_cfg(GenObject *parent) :
    FileObject(parent, "pcie_cfg"),
    _text0_(this, "PCIE end-point config:"),
    CFG_PCIE_DATA_WIDTH(this, "CFG_PCIE_DATA_WIDTH", "64"),
    CFG_PCIE_DMAFIFO_DEPTH(this, "CFG_PCIE_DMAFIFO_DEPTH", "3", "FIFO depth in PCIE DMA engine"),
    _text1_(this, ""),
    _text2_(this, "Completions packets {Fmt[1:0], Type[4:0]} for non-posted TLPs:"),
    PCIE_CPL_NODATA(this, "PCIE_CPL_NODATA", "7", "0x0A", "Completion without data (IOWr, CfgWr1, CfgWr2)"),
    PCIE_CPL_DATA(this, "PCIE_CPL_DATA", "7", "0x4A", "Completion with data (MemRd, IORd, CfgRd1, CfgRd2)"),
    PCIE_CPL_LOCKED_READ_NODATA(this, "PCIE_CPL_LOCKED_READ_NODATA", "7", "0x0B", "Completion for Locked Memory Read without Data (only when error)"),
    PCIE_CPL_LOCKED_READ(this, "PCIE_CPL_LOCKED_READ", "7", "0x4B", "Completion for locked memory read"),
    _n_(this)
{
    pcie_cfg_ = this;
}

