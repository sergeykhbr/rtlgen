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

#include "types_dma.h"

types_dma* glob_types_dma_ = 0;

types_dma::types_dma(GenObject *parent) :
    FileObject(parent, "types_dma"),
    _dma0_(this),
    _dma1_(this),
    pcie_dma64_out_def(this, "pcie_dma64_out_type", NO_COMMENT),
    _dma2_(this),
    _dma3_(this, "@brief   DMA output empty values."),
    pcie_dma64_out_none(this, "pcie_dma64_out_none", NO_COMMENT),
    _dma4_(this),
    _dma5_(this, "@brief DMA device input signals."),
    pcie_dma64_in_type_def(this, "pcie_dma64_in_type", NO_COMMENT),
    _dma6_(this),
    pcie_dma64_in_none(this, "pcie_dma64_in_none", NO_COMMENT),
    _n_(this)
{
    glob_types_dma_ = this;
}

