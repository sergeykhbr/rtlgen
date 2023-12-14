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
#include "const.h"
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
void TEXT(const char *comment=NO_COMMENT);

/** Generate constant all 0:
    sysc: 0
    sv:   '0
    vhdl: (others => '0')
 */
Operation &ALLZEROS(const char *comment=NO_COMMENT);

/** Generate constant all 1:
    sysc: ~0
    sv:   '1
    vhdl: (others => '1')
 */
Operation &ALLONES(const char *comment=NO_COMMENT);

/** Bit selection;
    sysc: a[7], a[param]
    sv:   a[7], a[param]
    vhdl: a(1), a(param)
 */
Operation &BIT(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);
Operation &BIT(GenObject &a, const char *b, const char *comment=NO_COMMENT);
Operation &BIT(GenObject &a, int b, const char *comment=NO_COMMENT);

/** Bits selection by range;
    sysc: a(7, 3), a(param-1, 0)
    sv:   a[7: 3], a[param-1: 0]
    vhdl: a(7 downto 3), a(param-1 downto 0)
 */
Operation &BITS(GenObject &a, GenObject &h, GenObject &l, const char *comment=NO_COMMENT);
Operation &BITS(GenObject &a, int h, int l, const char *comment=NO_COMMENT);

/** Bits selection by width;
    sysc: a(2*n + 8 - 1, 2*n)
    sv:   a[2*n +: 8]
    vhdl: a(2*n + 8 - 1 downto 2*n)
 */
Operation &BITSW(GenObject &a, GenObject &start, GenObject &width, const char *comment="");


/** Simple number in different format (including allzeros/allones)
    sysc: 1234, 0xABC, true, 3.2, "str", '1, '0
    sv:   1234
    vhdl: 1234
 */
GenObject &CONST(const char *val);
GenObject &CONST(const char *val, const char *width);
GenObject &CONST(const char *val, int width);

/** Convert to biguint<> (systemc only):
    sysc: biguint<sz>(a)
    sv:   a
    vhdl: a
 */
Operation &TO_BIG(size_t sz, GenObject &a);


/** Logical bitwise inversion:
    sysc: (~a)
    sv:   (~a)
    vhdl: (not a)
 */
Operation &INV_L(GenObject &a, const char *comment=NO_COMMENT);

/** Boolean inversion:
    sysc: (!a)
    sv:   (~a)
    vhdl: (not a)
 */
Operation &INV(GenObject &a, const char *comment=NO_COMMENT);

/**  OR all bits
    sysc: a.or_reduce()
    sv:   (|a)
    vhdl: or_reduce(a)
 */
Operation &OR_REDUCE(GenObject &a, const char *comment=NO_COMMENT);

/**  AND all bits
    sysc: a.and_reduce()
    sv:   (&a)
    vhdl: and_reduce(a)
 */
Operation &AND_REDUCE(GenObject &a, const char *comment=NO_COMMENT);

/**  Decrement
    sysc: (a - 1)
    sv:   (a - 1)
    vhdl: (a - 1)
 */
Operation &DEC(GenObject &a, const char *comment=NO_COMMENT);

/**  Increment
    sysc: (a + 1)
    sv:   (a + 1)
    vhdl: (a + 1)
 */
Operation &INC(GenObject &a, const char *comment=NO_COMMENT);


/** Arithemic adder:
    sysc: (a + b)
    sv:   (a + b)
    vhdl: (a + b)
 */
Operation &ADD2(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Arithemic subtruct:
    sysc: (a - b)
    sv:   (a - b)
    vhdl: (a - b)
 */
Operation &SUB2(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Multiplication subtruct:
    sysc: (a * b)
    sv:   (a * b)
    vhdl: (a * b)
 */
Operation &MUL2(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Division subtruct:
    sysc: (a / b)
    sv:   (a / b)
    vhdl: (a / b)
 */
Operation &DIV2(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Bitwise AND:
    sysc: (a & b)
    sv:   (a & b)
    vhdl: (a and b)
 */
Operation &AND2_L(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Logical AND:
    sysc: (a && b)
    sv:   (a && b)
    vhdl: (a and b)
 */
Operation &AND2(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Equal:
    sysc: (a == b)
    sv:   (a == b)
    vhdl: (a = b)
 */
Operation &EQ(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Not equal:
    sysc: (a != b)
    sv:   (a != b)
    vhdl: (a /= b)
 */
Operation &NE(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Greater:
    sysc: (a > b)
    sv:   (a > b)
    vhdl: (a > b)
 */
Operation &GT(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Greater-Equal:
    sysc: (a >= b)
    sv:   (a >= b)
    vhdl: (a >= b)
 */
Operation &GE(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Less:
    sysc: (a < b)
    sv:   (a < b)
    vhdl: (a < b)
 */
Operation &LS(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Less-Equal:
    sysc: (a <= b)
    sv:   (a <= b)
    vhdl: (a <= b)
 */
Operation &LE(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Equal Zero:
    sysc: (a.or_reduce() == 0)
    sv:   ((|a) == 1'b0)
    vhdl: (or_reduce(a) = '0')
 */
Operation &EZ(GenObject &a, const char *comment=NO_COMMENT);

/** Non Zero:
    sysc: (a.or_reduce() != 0)
    sv:   ((|a) == 1'b1)
    vhdl: (or_reduce(a) = '1')
 */
Operation &NZ(GenObject &a, const char *comment=NO_COMMENT);

/** Concatation
    sysc: (a, b, c, ..)  depends of zero value
    sv:   {a, b, c, ..)
    vhdl: (a & b & c &...)
 */
Operation &CC2(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);
Operation &CC3(GenObject &a, GenObject &b, GenObject &c, const char *comment=NO_COMMENT);
Operation &CC4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment=NO_COMMENT);
Operation &CCx(size_t cnt, ...);

/** Calculate total width of the bus after concatation
    sysc: (a, b, c, ..) compute bitwidth
    sv:   {a, b, c, ..) compute bitwidth
    vhdl: (a & b & c &...) compute bitwidth
    
 */
Operation &CALCWIDTHx(size_t cnt, ...);

/** Split bus on a set of signals:
    sysc: x1 = a(7,0); x2=a[8]; x3=a(10,9) etc
    sv:   {x1, x2, x3, ..} = a
    vhdl: (x1 & x2 & x3 &...) := a
    
 */
Operation &SPLx(GenObject &a, size_t cnt, ...);     // cplit concatated bus



/** Left shift:
    sysc: (a << sz)
    sv:   {a[], {sz{1'b0}}
    vhdl: (a sll sz)
 */
Operation &LSH(GenObject &a, GenObject &sz, const char *comment=NO_COMMENT);
Operation &LSH(GenObject &a, int sz, const char *comment=NO_COMMENT);

/** Power of 2, Special case of LSH:
    sysc: (1 << sz)
    sv:   (2**sz)
    vhdl: (2**sz)
 */
Operation &POW2(GenObject &sz, const char *comment=NO_COMMENT);

/** Right shift:
    sysc: (a >> sz)
    sv:   {a[], {sz{1'b0}}
    vhdl: (a srl sz)
 */
Operation &RSH(GenObject &a, GenObject &sz, const char *comment=NO_COMMENT);
Operation &RSH(GenObject &a, int sz, const char *comment=NO_COMMENT);

/** Write 0 into variable:
    sysc: a = 0;
    sv:   a = 1'b0;     or '0
    vhdl: a := '0';  or (others => '0')
 */
Operation &SETZERO(GenObject &a, const char *comment=NO_COMMENT);

/** Write 1 into variable:
    sysc: a = 1;
    sv:   a = 1'b1;  or n'd1
    vhdl: a := '1';  or "00...01"
 */
Operation &SETONE(GenObject &a, const char *comment=NO_COMMENT);


/** Instantitate new module:
    sysc: inst0 = new ModuleTpe("name", ...);
    sv:   ModuleType #(..) inst0
    vhdl: inst0 : ModuleType generic map (..);
 */
void NEW(GenObject &m, const char *name, GenObject *idx=0, const char *comment=NO_COMMENT);

/** Connect module IOs:
    sysc: inst0->io(w_signal);
    sv:   .io(w_signal)
    vhdl: io -> signal
 */
void CONNECT(GenObject &m, GenObject *idx, GenObject &port, GenObject &s, const char *comment=NO_COMMENT);

/** End of module instantitaion. Empty string is generated. */
void ENDNEW(const char *comment=NO_COMMENT);

/**
    sysc: a = 0;
    sv:   assign a = '0;
    vhdl: a <= (others => '0');
 */
Operation &ASSIGNZERO(GenObject &a, const char *comment=NO_COMMENT);

/**
    sysc: a = b;
    sv:   assign a = b;
    vhdl: a <= b;
 */
Operation &ASSIGN(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

};
