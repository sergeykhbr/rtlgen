// 
//  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
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

#define CONFIG_RISCV_VHDL               1
#define CONFIG_GPU3D                    0

#if CONFIG_RISCV_VHDL

    #define GENCFG_IMPLEMENTATION_ID        "0x20220813"
    #define GENCFG_HW_FPU_ENABLE            "true"
    #define GENCFG_SOC_HW_ID                "0x20220903"

    #define GENCFG_XDMA_ENABLE              1       // enable AXI master DMA slot and potentially DMA itself
    #define GENCFG_SD_CTRL_ENABLE           1
    #define GENCFG_PCIE_ENABLE              0
    #define GENCFG_HDMI_ENABLE              0

    #define GENCFG_CPU_NUM                  1
    #define GENCFG_ILOG2_LINES_PER_WAY      7
    #define GENCFG_ILOG2_NWAYS              2
    #define GENCFG_DLOG2_LINES_PER_WAY      7
    #define GENCFG_DLOG2_NWAYS              2
    #define GENCFG_L2CACHE_ENA              1
    #define GENCFG_L2_LOG2_NWAYS            4
    #define GENCFG_L2_LOG2_LINES_PER_WAY    9
    #define GENCFG_BOOTROM_LOG2_SIZE        16
    #define GENCFG_BOOTROM_FILE_HEX         "../../../../examples/bootrom_tests/linuxbuild/bin/bootrom_tests"
    #define GENCFG_SRAM_LOG2_SIZE           21

#elif CONFIG_GPU3D

    #define GENCFG_IMPLEMENTATION_ID        "0x20241103"
    #define GENCFG_HW_FPU_ENABLE            "false"
    #define GENCFG_SOC_HW_ID                "0x20241103"

    #define GENCFG_XDMA_ENABLE              0       // enable AXI master DMA slot and potentially DMA itself
    #define GENCFG_SD_CTRL_ENABLE           0
    #define GENCFG_PCIE_ENABLE              1
    #define GENCFG_HDMI_ENABLE              1

    #define GENCFG_CPU_NUM                  1
    #define GENCFG_ILOG2_LINES_PER_WAY      2
    #define GENCFG_ILOG2_NWAYS              2
    #define GENCFG_DLOG2_LINES_PER_WAY      2
    #define GENCFG_DLOG2_NWAYS              2
    #define GENCFG_L2CACHE_ENA              0
    #define GENCFG_L2_LOG2_NWAYS            4
    #define GENCFG_L2_LOG2_LINES_PER_WAY    9
    #define GENCFG_BOOTROM_LOG2_SIZE        15
    #define GENCFG_BOOTROM_FILE_HEX         "../../../software/bootrom/bin/bootrom"
    #define GENCFG_SRAM_LOG2_SIZE           16

#else
#endif
