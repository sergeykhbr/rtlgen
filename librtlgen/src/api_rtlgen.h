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

#pragma once

#include <iostream>
#include "genobjects.h"
#include "projects.h"
#include "folders.h"
#include "files.h"
#include "values.h"
#include "params.h"
#include "defparams.h"
#include "modules.h"
#include "ports.h"
#include "comments.h"
#include "funcs.h"
#include "proc.h"
#include "logic.h"
#include "operations.h"
#include "enums.h"
#include "signals.h"
#include "regs.h"
#include "structs.h"
#include "array.h"
#include "clock.h"

namespace sysvc {

/** Generate text line:
    sysc: // text
    sv:   // text
    vhdl: -- text
 */
void TEXT(const char *comment="");

/** Generate constant all 0:
    sysc: 0
    sv:   '0
    vhdl: (others => '0')
 */
Operation &ALLZEROS(const char *comment="");

/** Generate constant all 1:
    sysc: ~0
    sv:   '1
    vhdl: (others => '1')
 */
Operation &ALLONES(const char *comment="");

/** Write 0 into variable:
    sysc: a = 0;
    sv:   a = 1'b0;     or '0
    vhdl: a := '0';  or (others => '0')
 */
Operation &SETZERO(GenObject &a, const char *comment="");

/** Write 1 into variable:
    sysc: a = 1;
    sv:   a = 1'b1;  or n'd1
    vhdl: a := '1';  or "00...01"
 */
Operation &SETONE(GenObject &a, const char *comment="");


/** Instantitate new module:
    sysc: inst0 = new ModuleTpe("name", ...);
    sv:   ModuleType #(..) inst0
    vhdl: inst0 : ModuleType generic map (..);
 */
void NEW(GenObject &m, const char *name, GenObject *idx=0, const char *comment="");

/** Connect module IOs:
    sysc: inst0->io(w_signal);
    sv:   .io(w_signal)
    vhdl: io -> signal
 */
void CONNECT(GenObject &m, GenObject *idx, GenObject &port, GenObject &s, const char *comment="");

/** End of module instantitaion. Empty string is generated. */
void ENDNEW(const char *comment="");

/**
    sysc: a = 0;
    sv:   assign a = '0;
    vhdl: a <= (others => '0');
 */
Operation &ASSIGNZERO(GenObject &a, const char *comment="");

/**
    sysc: a = b;
    sv:   assign a = b;
    vhdl: a <= b;
 */
Operation &ASSIGN(GenObject &a, GenObject &b, const char *comment="");

};
