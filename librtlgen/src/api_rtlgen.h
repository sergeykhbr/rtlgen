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

namespace sysvc {

/** Generate text line:
    sysc: // text
    sv:   // text
    vhdl: -- text
 */
void TEXT(const char *comment=NO_COMMENT);
/** The same but out of comb process in System Verilog
 */
void TEXT_ASSIGN(const char *comment=NO_COMMENT);

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

/** Create unnamed number in different format (including allzeros/allones)
    sysc: 1234, 0xABC, true, 3.2, "str", '1, '0
    sv:   1234
    vhdl: 1234
 */
GenObject &CONST(const char *val);

/** Create unnamed logic value using string width argument
    sysc: 1234, 0x0ABC
    sv:   16'd1234, 16'h0ABC
    vhdl: "1234", X"0ABC"
 */
GenObject &CONST(const char *val, const char *width);

/** Create unnamed logic value using int width argument
    sysc: 1234, 0x0ABC
    sv:   16'd1234, 16'h0ABC
    vhdl: "1234", X"0ABC"
 */
GenObject &CONST(const char *val, int width);

/** explicit conversion to int
    sysc: a.to_int()
    sv:   int'(a)
    vhdl: conv_std_logic_to_int(a)
*/
Operation &TO_INT(GenObject &a);

/** explicit conversion to uint32 (sysc only)
    sysc: a.to_uint64()
    sv:   a
    vhdl: a
*/
Operation &TO_U32(GenObject &a);

/** explicit conversion to uint64 (sysc only)
    sysc: a.to_uint64()
    sv:   a
    vhdl: a
*/
Operation &TO_U64(GenObject &a);

/** explicit conversion to const char* (sysc only)
    sysc: a.c_str()
    sv:   a
    vhdl: a
*/
Operation &TO_CSTR(GenObject &a);

/** explicit conversion of biguint to uint64 (sysc only)
    sysc: a.to_uint64()
    sv:   a
    vhdl: a
*/
Operation &BIG_TO_U64(GenObject &a);

/** Convert to biguint<> (systemc only):
    sysc: biguint<sz>(a)
    sv:   a
    vhdl: a
 */
Operation &TO_BIG(int sz, GenObject &a);


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

/** Bitwise OR:
    sysc: (a | b)
    sv:   (a | b)
    vhdl: (a or b)
 */
Operation &OR2_L(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Logical OR:
    sysc: (a || b)
    sv:   (a || b)
    vhdl: (a or b)
 */
Operation &OR2(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** XOR:
    sysc: (a ^ b)
    sv:   (a ^ b)
    vhdl: (a xor b)
 */
Operation &XOR2(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Bitwise AND (output in one line):
    sysc: (a & b & c)
    sv:   (a & b & c)
    vhdl: (a and b and c)
 */
Operation &AND3_L(GenObject &a, GenObject &b, GenObject &c, const char *comment=NO_COMMENT);

/** Logical AND (output in one line):
    sysc: (a && b && c)
    sv:   (a && b && d)
    vhdl: (a and b and c)
 */
Operation &AND3(GenObject &a, GenObject &b, GenObject &c, const char *comment=NO_COMMENT);

/** Logical OR (output in one line):
    sysc: (a || b || c)
    sv:   (a || b || d)
    vhdl: (a or b or c)
 */
Operation &OR3(GenObject &a, GenObject &b, GenObject &c, const char *comment=NO_COMMENT);

/** Logical AND (output in one line):
    sysc: (a && b && c && d)
    sv:   (a && b && c && d)
    vhdl: (a and b and c and d)
 */
Operation &AND4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment=NO_COMMENT);

/** Logical OR (output in one line):
    sysc: (a || b || c || d)
    sv:   (a || b || c || d)
    vhdl: (a or b or c or d)
 */
Operation &OR4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment=NO_COMMENT);

/** Sum of N-elements (multiline output)
    sysc: (a + b + c + ..)
    sv:   (a + b + c + ..)
    vhdl: (a + b + c + ..)
 */
Operation &ADDx(size_t cnt, ...);

/** Bitwise And of N-elements (multiline output)
    sysc: (a & b & c & ..)
    sv:   (a & b & c & ..)
    vhdl: (a and b and c and ..)
 */
Operation &ANDx_L(size_t cnt, ...);

/** And of N-elements (multiline output)
    sysc: (a && b && c && ..)
    sv:   (a && b && c && ..)
    vhdl: (a and b and c and ..)
 */
Operation &ANDx(size_t cnt, ...);

/** Bitwise OR of N-elements (multiline output)
    sysc: (a | b | c | ..)
    sv:   (a | b | c | ..)
    vhdl: (a or b or c or ..)
 */
Operation &ORx_L(size_t cnt, ...);

/** OR of N-elements (multiline output)
    sysc: (a || b || c || ..)
    sv:   (a || b || c || ..)
    vhdl: (a or b or c or ..)
 */
Operation &ORx(size_t cnt, ...);

/** XOR of N-elements (multiline output)
    sysc: (a ^ b ^ c ^ ..)
    sv:   (a ^ b ^ c ^ ..)
    vhdl: (a xor b xor c xor ..)
 */
Operation &XORx(size_t cnt, ...);

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
Operation &SPLx_ASSIGN(GenObject &a, size_t cnt, ...);     // cplit concatated bus



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

/** Set value b into a:
    sysc: a = b;
    sv:   a = b;
    vhdl: a := b;
 */
Operation &SETVAL(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Set value b into a with delay:
    sysc: a.write(b, T * SC_NS);
    sv:   #T a = b;         (full equivalent a = #T b; for blocking assignment)
    vhdl: a := b after T ns;
 */
Operation &SETVAL_DELAY(GenObject &a, GenObject &b, GenObject &T, const char *comment=NO_COMMENT);

/** Set non-blocking value b into a:
    sysc: a = b;
    sv:   a <= b;
    vhdl: a <= b;
 */
Operation &SETVAL_NB(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

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

/** Set to a specific bit of logic:
    sysc: a[b] = val;
    sv:   a[b] = val;
    vhdl: a(b) := val;
 */
Operation &SETBIT(GenObject &a, GenObject &b, GenObject &val, const char *comment=NO_COMMENT);

/** Set to a specific bit of logic (int b):
    sysc: a[b] = val;
    sv:   a[b] = val;
    vhdl: a(b) := val;
 */
Operation &SETBIT(GenObject &a, int b, GenObject &val, const char *comment=NO_COMMENT);

/** Set a specific bit of logic to HIGH:
    sysc: a[b] = 1;
    sv:   a[b] = 1'b1;
    vhdl: a(b) := '1';
 */
Operation &SETBITONE(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Set a specific bit of logic to HIGH (variant 2):
    sysc: a[b] = 1;
    sv:   a[b] = 1'b1;
    vhdl: a(b) := '1';
 */
Operation &SETBITONE(GenObject &a, const char *b, const char *comment=NO_COMMENT);

/** Set a specific bit of logic to HIGH (variant 3):
    sysc: a[b] = 1;
    sv:   a[b] = 1'b1;
    vhdl: a(b) := '1';
 */
Operation &SETBITONE(GenObject &a, int b, const char *comment=NO_COMMENT);

/** Set a specific bit of logic to LOW:
    sysc: a[b] = 0;
    sv:   a[b] = 1'b0;
    vhdl: a(b) := '0';
 */
Operation &SETBITZERO(GenObject &a, GenObject &b, const char *comment=NO_COMMENT);

/** Set a specific bit of logic to LOW (variant 2):
    sysc: a[b] = 0;
    sv:   a[b] = 1'b0;
    vhdl: a(b) := '0';
 */
Operation &SETBITZERO(GenObject &a, const char *b, const char *comment=NO_COMMENT);

/** Set a specific bit of logic to LOW (variant 3):
    sysc: a[b] = 0;
    sv:   a[b] = 1'b0;
    vhdl: a(b) := '0';
 */
Operation &SETBITZERO(GenObject &a, int b, const char *comment=NO_COMMENT);

/** Set a specific bits of logic:
    sysc: a(h, l) = val;
    sv:   a[h: l] = val;
    vhdl: a(h downto l) := val;
 */
Operation &SETBITS(GenObject &a, GenObject &h, GenObject &l, GenObject &val, const char *comment=NO_COMMENT);

/** Set a specific bits of logic (variant 2):
    sysc: a(h, l) = val;
    sv:   a[h: l] = val;
    vhdl: a(h downto l) := val;
 */
Operation &SETBITS(GenObject &a, int h, int l, GenObject &val, const char *comment=NO_COMMENT);

/** Set a specific bits of logic using width argument:
    sysc: a(start + width - 1, start) = val;
    sv:   a[start +: width] = val;
    vhdl: a(start + width - 1 downto start) := val;
 */
Operation &SETBITSW(GenObject &a, GenObject &start, GenObject &width, GenObject &val, const char *comment=NO_COMMENT);


/** Write array item value:
    sysc: arr[idx].item = val;
    sv:   arr[idx].item = val;
    vhdl: arr(idx).item := val;
 */
Operation &SETARRITEM(GenObject &arr,
                      GenObject &idx,
                      GenObject &item,
                      GenObject &val,
                      const char *comment=NO_COMMENT);

/** Non-bocking write array item value:
    sysc: arr[idx].item = val;
    sv:   arr[idx].item <= val;
    vhdl: arr(idx).item <= val;
 */
Operation &SETARRITEM_NB(GenObject &arr,
                         GenObject &idx,
                         GenObject &item,
                         GenObject &val,
                         const char *comment=NO_COMMENT);

/** Write array item value (another format):
    sysc: arr[idx].item = val;
    sv:   arr[idx].item = val;
    vhdl: arr(idx).item := val;
 */
Operation &SETARRITEM(GenObject &arr, int idx, GenObject &val);

/** Assign array item value:
    sysc: arr[idx].item = val;
    sv:   assign arr[idx].item = val; (only out of process)
    vhdl: arr(idx).item <= val;
 */
Operation &ASSIGNARRITEM(GenObject &arr, GenObject &idx,
                         GenObject &item,
                         GenObject &val,
                         const char *comment=NO_COMMENT);

/** Assign array item valu (another format of idx argument):
    sysc: arr[idx].item = val;
    sv:   assign arr[idx].item = val; (only out of process)
    vhdl: arr(idx).item <= val;
 */
Operation &ASSIGNARRITEM(GenObject &arr, int idx, GenObject &val);


/** Write array item bit value:
    sysc: arr[idx].item[bitidx] = val;
    sv:   arr[idx].item[bitidx] = val;
    vhdl: arr(idx).item(bitidx) := val;
 */
Operation &SETARRITEMBIT(GenObject &arr, GenObject &idx,
                         GenObject &item,  GenObject &bitidx,
                         GenObject &val,
                         const char *comment=NO_COMMENT);

/** Write array item bits using width argument:
    sysc: arr[idx].item(start+width-1, width) = val;
    sv:   arr[idx].item[start +: width] = val;
    vhdl: arr(idx).item(start+width-1 downto width) := val;
 */
Operation &SETARRITEMBITSW(GenObject &arr, GenObject &idx,
                           GenObject &item, GenObject &start, GenObject &width,
                           GenObject &val,
                           const char *comment=NO_COMMENT);


/** Auaxilirary function used in multilevel cycles with multilevel structures.
    Index is cleared after operation is generated.

    Example: We would like to form variable names "struct1[i].struct2[j].variable*":
        
        for (int i = 0; i < i_end; i++) {
            for (int j = 0; j <ji_end; j++) {
                SETARRIDX(struct1, i);
                ARRITEM(struct1.struct2, j, struct1.struct2.variable1);

                SETARRIDX(struct1, i);
                ARRITEM(struct1.struct2, j, struct1.struct2.variable2);
            }
        }
    sysc: -
    sv:   -
    vhdl: -
 */
Operation &SETARRIDX(GenObject &arr, GenObject &idx);

/** Increment value:
    sysc: res += inc;
    sv:   res += inc;
    vhdl: res = res + inc;
 */
Operation &INCVAL(GenObject &res, GenObject &inc, const char *comment=NO_COMMENT);


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
