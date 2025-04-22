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

#include "api_rtlgen.h"
#include <string.h>

namespace sysvc {

void TEXT(const char *comment) {
    new TextOperation(comment);
}

Operation &ALLZEROS(const char *comment) {
    return *new AllConstOperation(0, comment);
}

Operation &ALLONES(const char *comment) {
    return *new AllConstOperation(1, comment);
}

// BIT
Operation &BIT(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new BitOperation(&a, &b, comment);
    return *p;
}

Operation &BIT(GenObject &a, const char *b, const char *comment) {
    GenObject *idx = SCV_parse_to_obj(0, b);
    Operation *p = new BitOperation(&a, idx, comment);
    return *p;
}

Operation &BIT(GenObject &a, int b, const char *comment) {
    GenObject *idx = new DecConst(b);
    Operation *p = new BitOperation(&a, idx, comment);
    return *p;
}

// BITS
Operation &BITS(GenObject &a, GenObject &h, GenObject &l, const char *comment) {
    Operation *p = new BitsOperation(&a, &h, &l, comment);
    return *p;
}

Operation &BITS(GenObject &a, int h, int l, const char *comment) {
    Operation *p = new BitsOperation(&a, new DecConst(h), new DecConst(l), comment);
    return *p;
}

// BITSW
Operation &BITSW(GenObject &a, GenObject &start, GenObject &width, const char *comment) {
    Operation *p = new BitswOperation(&a, &start, &width, comment);
    return *p;
}


// CONST number
GenObject &CONST(const char *val) {
    GenObject *p = SCV_parse_to_obj(SCV_get_local_module(), val);
    return *p;
}

// CONST logic
GenObject &CONST(const char *val, const char *width) {
    GenObject *w = SCV_parse_to_obj(SCV_get_local_module(), width);
    GenObject *p;
    if (val[0] == '0' && val[1] == 'x') {
        p = new HexLogicConst(w,
            SCV_parse_to_obj(SCV_get_local_module(), val)->getValue());
    } else {
        p = new DecLogicConst(w,
            SCV_parse_to_obj(SCV_get_local_module(), val)->getValue());
    }
    return *p;
}

// CONST logic
GenObject &CONST(const char *val, int width) {
    GenObject *w = new DecConst(width);
    GenObject *p;
    if (val[0] == '0' && val[1] == 'x') {
        p = new HexLogicConst(w,
            SCV_parse_to_obj(SCV_get_local_module(), val)->getValue());
    } else {
        p = new DecLogicConst(w,
            SCV_parse_to_obj(SCV_get_local_module(), val)->getValue());
    }
    return *p;
}

// TO_INT
Operation &TO_INT(GenObject &a) {
    Operation *p = new ToIntOperation(&a, NO_COMMENT);
    return *p;
}

// TO_U32
Operation &TO_U32(GenObject &a) {
    Operation *p = new ToU32Operation(&a, NO_COMMENT);
    return *p;
}

// TO_U64
Operation &TO_U64(GenObject &a) {
    Operation *p = new ToU64Operation(&a, NO_COMMENT);
    return *p;
}

// TO_CSTR
Operation &TO_CSTR(GenObject &a) {
    Operation *p = new ToCStrOperation(&a, NO_COMMENT);
    return *p;
}

// BIG_TO_U64
Operation &BIG_TO_U64(GenObject &a) {
    Operation *p = new BigToU64Operation(&a, NO_COMMENT);
    return *p;
}

// TO_BIG
Operation &TO_BIG(int sz, GenObject &a) {
    Operation *p = new ToBigOperation(&a, sz, NO_COMMENT);
    return *p;
}

// INV (arithemtic, logical)
Operation &INV_L(GenObject &a, const char *comment) {
    Operation *p = new InvOperation(&a, true, comment);
    return *p;
}

Operation &INV(GenObject &a, const char *comment) {
    Operation *p = new InvOperation(&a, false, comment);
    return *p;
}

// DEC
Operation &DEC(GenObject &a, const char *comment) {
    GenObject *p1;
    if (a.isLogic()) {
        p1 = new DecLogicConstOne(a.getObjWidth(), 1);
    } else {
        p1 = new DecConst(1);
    }
    Operation *p = new Sub2Operation(&a, p1, comment);
    return *p;
}

// INC
Operation &INC(GenObject &a, const char *comment) {
    GenObject *p1;
    if (a.isLogic()) {
        p1 = new DecLogicConstOne(a.getObjWidth(), 1);
    } else {
        p1 = new DecConst(1);
    }
    Operation *p = new Add2Operation(&a, p1, comment);
    return *p;
}

// AND_REDUCE
Operation &AND_REDUCE(GenObject &a, const char *comment) {
    Operation *p = new AndReduceOperation(&a, comment);
    return *p;
}

// OR_REDUCE
Operation &OR_REDUCE(GenObject &a, const char *comment) {
    Operation *p = new OrReduceOperation(&a, comment);
    return *p;
}

// Equal Zero
Operation &EZ(GenObject &a, const char *comment) {
    Operation *p = new EzOperation(&a, comment);
    return *p;
}

// Non Zero
Operation &NZ(GenObject &a, const char *comment) {
    Operation *p = new NzOperation(&a, comment);
    return *p;
}

// Equal (==)
Operation &EQ(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new EqOperation(&a, &b, comment);
    return *p;
}

// Not Equal (!=)
Operation &NE(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new NeOperation(&a, &b, comment);
    return *p;
}

// Greater (>)
Operation &GT(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new GtOperation(&a, &b, comment);
    return *p;
}

// Greater-Equal (>=)
Operation &GE(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new GeOperation(&a, &b, comment);
    return *p;
}

// Less (<)
Operation &LS(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new LsOperation(&a, &b, comment);
    return *p;
}

// Less-Equal (<)
Operation &LE(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new LeOperation(&a, &b, comment);
    return *p;
}

// ADD2
Operation &ADD2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Add2Operation(&a, &b, comment);
    return *p;
}

// SUB2
Operation &SUB2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Sub2Operation(&a, &b, comment);
    return *p;
}

// MUL2

Operation &MUL2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Mul2Operation(&a, &b, comment);
    return *p;
}

// DIV2
Operation &DIV2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Div2Operation(&a, &b, comment);
    return *p;
}


// AND2_L
Operation &AND2_L(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new And2Operation(&a, &b, true, comment);
    return *p;
}

// AND2
Operation &AND2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new And2Operation(&a, &b, false, comment);
    return *p;
}

// OR2_L
Operation &OR2_L(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Or2Operation(&a, &b, true, comment);
    return *p;
}

// OR2
Operation &OR2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Or2Operation(&a, &b, false, comment);
    return *p;
}

// XOR2
Operation &XOR2(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new Xor2Operation(&a, &b, comment);
    return *p;
}

// AND3_L
Operation &AND3_L(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new And3Operation(&a, &b, &c, true, comment);
    return *p;
}

// AND3
Operation &AND3(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new And3Operation(&a, &b, &c, false, comment);
    return *p;
}

// OR3
Operation &OR3(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    Operation *p = new Or3Operation(&a, &b, &c, false, comment);
    return *p;
}

// AND4
Operation &AND4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment) {
    Operation *p = new And4Operation(&a, &b, &c, &d, comment);
    return *p;
}

// OR4
Operation &OR4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment) {
    Operation *p = new Or4Operation(&a, &b, &c, &d, comment);
    return *p;
}

// ADDx
Operation &ADDx(size_t cnt, ...) {
    ADDxOperation *p = new ADDxOperation(NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}

// ANDx_L
Operation &ANDx_L(size_t cnt, ...) {
    ANDxOperation *p = new ANDxOperation(true, NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}

// ANDx
Operation &ANDx(size_t cnt, ...) {
    ANDxOperation *p = new ANDxOperation(false, NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}

// ORx_L
Operation &ORx_L(size_t cnt, ...) {
    ORxOperation *p = new ORxOperation(true, NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}

// ORx
Operation &ORx(size_t cnt, ...) {
    ORxOperation *p = new ORxOperation(false, NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}

// XOR
Operation &XORx(size_t cnt, ...) {
    XORxOperation *p = new XORxOperation(NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}

// CC2
Operation &CC2(GenObject &a, GenObject &b, const char *comment) {
    CCxOperation *p = new CCxOperation(true, comment);      // oneline = true
    p->add_entry(&a);
    p->add_entry(&b);
    return *p;
}

// CC3
Operation &CC3(GenObject &a, GenObject &b, GenObject &c, const char *comment) {
    CCxOperation *p = new CCxOperation(true, comment);      // oneline = true
    p->add_entry(&a);
    p->add_entry(&b);
    p->add_entry(&c);
    return *p;
}

// CC4
Operation &CC4(GenObject &a, GenObject &b, GenObject &c, GenObject &d, const char *comment) {
    CCxOperation *p = new CCxOperation(true, comment);      // oneline = true
    p->add_entry(&a);
    p->add_entry(&b);
    p->add_entry(&c);
    p->add_entry(&d);
    return *p;
}

// CCx
Operation &CCx(size_t cnt, ...) {
    CCxOperation *p = new CCxOperation(false, "");          // false = oneline for each argument
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}

// SPLx
Operation &SPLx(GenObject &a, size_t cnt, ...) {
    Operation *p = new SplitOperation(&a, NO_COMMENT);
    GenObject *obj;
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}

// CALCWIDTHx
Operation &CALCWIDTHx(size_t cnt, ...) {
    GenObject *obj;
    Operation *p = new CalcWidthOperation(NO_COMMENT);
    va_list arg;
    va_start(arg, cnt);
    for (int i = 0; i < cnt; i++) {
        obj = va_arg(arg, GenObject *);
        p->add_entry(obj);
    }
    va_end(arg);
    return *p;
}


// LSH
Operation &LSH(GenObject &a, GenObject &sz, const char *comment) {
    Operation *p = new LshOperation(&a, &sz, comment);
    return *p;
}

Operation &LSH(GenObject &a, int sz, const char *comment) {
    Operation *p = new LshOperation(&a, new DecConst(sz), comment);
    return *p;
}

// LSH
Operation &POW2(GenObject &sz, const char *comment) {
    Operation *p = new Pow2Operation(&sz, comment);
    return *p;
}

// RSH
Operation &RSH(GenObject &a, GenObject &sz, const char *comment) {
    Operation *p = new RshOperation(&a, &sz, comment);
    return *p;
}

Operation &RSH(GenObject &a, int sz, const char *comment) {
    Operation *p = new RshOperation(&a, new DecConst(sz), comment);
    return *p;
}

Operation &SETVAL(GenObject &a, GenObject &b, const char *comment) {
    Operation *ret = 0;
#if 1
    if (a.getName() == "vb_def_mapinfo") {
        bool st = true;
    }
#endif
    if (a.getObjDepth()) {
        GenObject *i = new I32D(NO_PARENT, "i", new DecConst(0), NO_COMMENT);;
        ret = new ForOperation(Operation::top_obj(),
                                i,
                                new DecConst(0),
                                a.getObjDepth(),
                                new StringConst("++"),
                                0,
                                comment);

        SETARRITEM(a, *i, a, ARRITEM(b, *i, b));

        new EndForOperation(0, NO_COMMENT);
    /*} else if (a.isStruct() 
        && !b.isOperation() && !b.isParam() && !b.isConst()
        && b.getObjValue() == 0) {
        for (auto &p: a.getEntries()) {
            ret = &SETVAL(*p, *b.getChildByName(p->getName()));
        }*/
    } else {
        ret = new SetValueOperation(&a,
                                      0,    // [arridx]
                                      0,    // .item
                                      false,    // h as width
                                      0,    // [h
                                      0,    // :l]
                                      false,    // blocking
                                      &b,  // val
                                      comment);
    }
    return *ret;
}

Operation &SETVAL_NB(GenObject &a, GenObject &b, const char *comment) {
    Operation *ret = new SetValueOperation(&a,
                                    0,    // [arridx]
                                    0,    // .item
                                    false,    // h as width
                                    0,    // [h
                                    0,    // :l]
                                    true,    // non-blocking
                                    &b,  // val
                                    comment);
    return *ret;
}

// a = 0
Operation &SETZERO(GenObject &a, const char *comment) {
    GenObject *b;
    if (a.isLogic()) {
        b = new DecLogicConst(a.getObjWidth(), 0);
    } else {
        b = new DecConst(0);
    }
    return SETVAL(a, *b, comment);
}

// a = 1
Operation &SETONE(GenObject &a, const char *comment) {
    GenObject *b;
    if (a.isLogic()) {
        b = new DecLogicConst(a.getObjWidth(), 1);
    } else {
        b = new DecConst(1);
    }
    return SETVAL(a, *b, comment);
}

Operation &SETBIT(GenObject &a, GenObject &b, GenObject &val, const char *comment) {
    Operation *p = new SetValueOperation(&a,
                                         0,     // idx
                                         0,     // item
                                         false, // h_as_width
                                         &b,    // h
                                         0,     // l
                                         false, // =
                                         &val,  // val
                                         comment);
    return *p;
}

Operation &SETBIT(GenObject &a, int b, GenObject &val, const char *comment) {
    Operation *p = new SetValueOperation(&a,
                                         0,     // idx
                                         0,     // item
                                         false, // h_as_width
                                         new DecConst(b),    // h
                                         0,     // l
                                         false, // =
                                         &val,  // val
                                         comment);
    return *p;
}

/** Set a specific bit of logic to HIGH:
    sysc: a[b] = 1;
    sv:   a[b] = 1'b1;
    vhdl: a(b) := '1';
 */
Operation &SETBITONE(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new SetValueOperation(&a,
                                         0,     // idx
                                         0,     // item
                                         false, // h_as_width
                                         &b,    // h
                                         0,     // l
                                         false, // =
                                         new DecLogicConst(new DecConst(1), 1),  // val
                                         comment);
    return *p;
}

/** Set a specific bit of logic to HIGH (variant 2):
    sysc: a[b] = 1;
    sv:   a[b] = 1'b1;
    vhdl: a(b) := '1';
 */
Operation &SETBITONE(GenObject &a, const char *b, const char *comment) {
    return SETBITONE(a, *SCV_parse_to_obj(0, b), comment);
}

/** Set a specific bit of logic to HIGH (variant 3):
    sysc: a[b] = 1;
    sv:   a[b] = 1'b1;
    vhdl: a(b) := '1';
 */
Operation &SETBITONE(GenObject &a, int b, const char *comment) {
    return SETBITONE(a, *new DecConst(b), comment);
}

/** Set a specific bit of logic to LOW:
    sysc: a[b] = 0;
    sv:   a[b] = 1'b0;
    vhdl: a(b) := '0';
 */
Operation &SETBITZERO(GenObject &a, GenObject &b, const char *comment) {
    Operation *p = new SetValueOperation(&a,
                                         0,     // idx
                                         0,     // item
                                         false, // h_as_width
                                         &b,    // h
                                         0,     // l
                                         false, // =
                                         new DecLogicConst(new DecConst(1), 0),  // val
                                         comment);
    return *p;
}

/** Set a specific bit of logic to LOW (variant 2):
    sysc: a[b] = 0;
    sv:   a[b] = 1'b0;
    vhdl: a(b) := '0';
 */
Operation &SETBITZERO(GenObject &a, const char *b, const char *comment) {
    return SETBITZERO(a, *SCV_parse_to_obj(0, b), comment);
}

/** Set a specific bit of logic to LOW (variant 3):
    sysc: a[b] = 0;
    sv:   a[b] = 1'b0;
    vhdl: a(b) := '0';
 */
Operation &SETBITZERO(GenObject &a, int b, const char *comment) {
    return SETBITZERO(a, *new DecConst(b), comment);
}

/** Set a specific bits of logic:
    sysc: a(h, l) = val;
    sv:   a[h : l] = val;
    vhdl: a(h downto l) := val;
 */
Operation &SETBITS(GenObject &a, GenObject &h, GenObject &l, GenObject &val, const char *comment) {
    Operation *p = new SetValueOperation(&a,
                                         0,     // idx
                                         0,     // item
                                         false, // h_as_width
                                         &h,    // h
                                         &l,     // l
                                         false, // =
                                         &val,  // val
                                         comment);
    return *p;
}

/** Set a specific bits of logic (variant 2):
    sysc: a(h, l) = val;
    sv:   a[h : l] = val;
    vhdl: a(h downto l) := val;
 */
Operation &SETBITS(GenObject &a, int h, int l, GenObject &val, const char *comment) {
    return SETBITS(a, *new DecConst(h), *new DecConst(l), val, comment);
}

/** Set a specific bits of logic using width argument:
    sysc: a(start + width - 1, start) = val;
    sv:   a[start +: width] = val;
    vhdl: a(start + width - 1 downto start) := val;
 */
Operation &SETBITSW(GenObject &a, GenObject &start, GenObject &width, GenObject &val, const char *comment) {
    Operation *p = new SetValueOperation(&a,
                                         0,     // idx
                                         0,     // item
                                         true, // h_as_width
                                         &width,    // h
                                         &start,     // l
                                         false, // =
                                         &val,  // val
                                         comment);
    return *p;
}


/** Write array item value:
    sysc: arr[idx].item = val;
    sv:   arr[idx].item = val;
    vhdl: arr(idx).item := val;
 */
Operation &SETARRITEM(GenObject &arr,
                      GenObject &idx,
                      GenObject &item,
                      GenObject &val,
                      const char *comment) {
    return *new SetValueOperation(&arr,
                                  &idx,     // [arridx]
                                  &item,    // .item
                                  false,    // h as width
                                  0,        // [h
                                  0,        // :l]
                                  false, // =
                                  &val,     // val
                                  comment);
}

/** Non-blocking write array item value:
    sysc: arr[idx].item = val;
    sv:   arr[idx].item <= val;
    vhdl: arr(idx).item <= val;
 */
Operation &SETARRITEM_NB(GenObject &arr,
                         GenObject &idx,
                         GenObject &item,
                         GenObject &val,
                         const char *comment) {
    return *new SetValueOperation(&arr,
                                  &idx,     // [arridx]
                                  &item,    // .item
                                  false,    // h as width
                                  0,        // [h
                                  0,        // :l]
                                  true,     // <=
                                  &val,     // val
                                  comment);
}

/** Write array item value (another format):
    sysc: arr[idx].item = val;
    sv:   arr[idx].item = val;
    vhdl: arr(idx).item := val;
 */
Operation &SETARRITEM(GenObject &arr, int idx, GenObject &val) {
    return *new SetValueOperation(&arr,
                                  new DecConst(idx),     // [arridx]
                                  0,        // .item
                                  false,    // h as width
                                  0,        // [h
                                  0,        // :l]
                                  false, // =
                                  &val,     // val
                                  NO_COMMENT);
}

/** Assign array item value:
    sysc: arr[idx].item = val;
    sv:   assign arr[idx].item = val; (only out of process)
    vhdl: arr(idx).item <= val;
 */
Operation &ASSIGNARRITEM(GenObject &arr, GenObject &idx,
                         GenObject &item,
                         GenObject &val,
                         const char *comment) {
    return *new AssignValueOperation(&arr,
                                     &idx,     // [arridx]
                                     &item,    // .item
                                     false,    // h as width
                                     0,        // [h
                                     0,        // :l]
                                     &val,     // val
                                     comment);
}

/** Assign array item valu (another format of idx argument):
    sysc: arr[idx].item = val;
    sv:   assign arr[idx].item = val; (only out of process)
    vhdl: arr(idx).item <= val;
 */
Operation &ASSIGNARRITEM(GenObject &arr, int idx, GenObject &val) {
    return *new AssignValueOperation(&arr,
                                     new DecConst(idx),     // [arridx]
                                     0,    // .item
                                     false,    // h as width
                                     0,        // [h
                                     0,        // :l]
                                     &val,     // val
                                     NO_COMMENT);
}


/** Write array item bit value:
    sysc: arr[idx].item[bitidx] = val;
    sv:   arr[idx].item[bitidx] = val;
    vhdl: arr(idx).item(bitidx) := val;
 */
Operation &SETARRITEMBIT(GenObject &arr, GenObject &idx,
                         GenObject &item,  GenObject &bitidx,
                         GenObject &val,
                         const char *comment) {
    return *new SetValueOperation(&arr,
                                  &idx,     // [arridx]
                                  &item,    // .item
                                  false,    // h as width
                                  &bitidx,  // [h
                                  0,        // :l]
                                  false,    // =
                                  &val,     // val
                                  comment);
}

/** Write array item bits using width argument:
    sysc: arr[idx].item(start+width-1, width) = val;
    sv:   arr[idx].item[start +: width] = val;
    vhdl: arr(idx).item(start+width-1 downto width) := val;
 */
Operation &SETARRITEMBITSW(GenObject &arr, GenObject &idx,
                           GenObject &item, GenObject &start, GenObject &width,
                           GenObject &val,
                           const char *comment) {
    return *new SetValueOperation(&arr,
                                  &idx,     // [arridx]
                                  &item,    // .item
                                  true,     // interpret h (MSB) as width
                                  &width,   // [h
                                  &start,   // :l]
                                  false,    // =
                                  &val,     // val
                                  comment);
}


/** Auaxilirary function used in for() cycle. Index is cleared after operation is generated.
 */
Operation &SETARRIDX(GenObject &arr, GenObject &idx) {
    return *new SetArrayIndexOperation(&arr, &idx, NO_COMMENT);
}


/** Increment value:
    sysc: res += inc;
    sv:   res += inc;
    vhdl: res = res + inc;
 */
Operation &INCVAL(GenObject &res, GenObject &inc, const char *comment) {
    return *new IncrementValueOperation(&res,
                                  0,        // [arridx]
                                  0,        // .item
                                  false,    // interpret h (MSB) as width
                                  0,        // [h
                                  0,        // :l]
                                  &inc,     // val
                                  comment);
}

void NEW(GenObject &m, const char *name, GenObject *idx, const char *comment) {
    new NewOperation(m, name, idx, comment);
}

void CONNECT(GenObject &m, GenObject *idx, GenObject &port, GenObject &s, const char *comment) {
    new ConnectOperation(m, idx, port, s, comment);
}


void ENDNEW(const char *comment) {
    new EndNewOperation(comment);
}

Operation &ASSIGNZERO(GenObject &a, const char *comment) {
    return *new AssignOperation(a, comment);
}

Operation &ASSIGN(GenObject &a, GenObject &b, const char *comment) {
    return *new AssignOperation(a, b, comment);
}

GenObject *SCV_parse_to_obj(GenObject *owner, const char *val) {
    GenObject *ret = 0;
    const char *pstr = val;
    char opcode[256] = "";
    char strarg[256] = "";
    size_t cnt = 0;
    size_t pos = 0;
    int bracecnt = 0;

    if (val[0] == '\0') {
        return ret;
    }
    // Skip spaces:
    while (val[pos] == ' ') {
        pos++;
    }

    while (val[pos] && val[pos] != '(' && val[pos] != ')' 
        && val[pos] != '{' && val[pos] != '}'
        && val[pos] != ',' && val[pos] != ' ') {
        opcode[cnt++] = val[pos++];
        opcode[cnt] = 0;
    }
    // Skip spaces:
    while (val[pos] == ' ') {
        pos++;
    }

    if (val[pos] == '{') {
        // TODO: structure initialization
    } else if (val[pos] != '(') {
        // No operations:
        bool isfloat = false;
        GenObject *cfgobj;
        pstr = opcode;
        while (*pstr) {
            if (*pstr == '.' && !isfloat) {
                isfloat = true;
            } else if (*pstr < '0' || *pstr > '9') {
                // File path string contains '.' symbols
                isfloat = false;
                break;
            }
            pstr++;
        }

        if (isfloat) {
            ret = new FloatConst(strtod(opcode, NULL));
        } else if (val[0] == '0' && val[1] == 'x') {
            ret = new HexConst(static_cast<uint64_t>(strtoull(val, 0, 16)));
        } else if (val[0] == '\'' && val[1] == '0') {
            ret = &ALLZEROS();
        } else if (val[0] == '\'' && val[1] == '1') {
            ret = &ALLONES();
        } else if (val[0] == 't' && val[1] == 'r' && val[2] == 'u' && val[3] == 'e') {
            ret = new DecConst(1);
        } else if (val[0] == 'f' && val[1] == 'a' && val[2] == 'l' && val[3] == 's' && val[4] == 'e') {
            ret = new DecConst(0);
        } else if (val[0] >= '0' && val[0] <= '9') {
            ret = new DecConst(static_cast<uint64_t>(strtoull(val, 0, 10)));
        } else if (cfgobj = SCV_get_cfg_type(owner, val)) {
            ret = cfgobj;
        } else {
            ret = new StringConst(val);
        }

        if (val[pos]) {     // skip ')' or ',' symbols
            pos++;
        }
        return ret;
    }
    pos++;                  // skip '(' symbol

    // Dual operation op(a,b):
    GenObject *args[2];
    for (int i = 0; i < 2; i++) {
        cnt = 0;
        bracecnt = 0;
        // Skip spaces:
        while (val[pos] == ' ') {
            pos++;
        }
        while (val[pos] 
            && ((bracecnt == 0 && val[pos] != ',' && val[pos] != ')' && val[pos] != ' '))
                || bracecnt) {
            // Skip spaces:
            while (val[pos] == ' ') {
                pos++;
            }
            if (val[pos] == '(') {
                bracecnt++;
            } else if (val[pos] == ')') {
                bracecnt--;
            }
            strarg[cnt++] = val[pos++];
            strarg[cnt] = 0;
            // Skip spaces:
            while (val[pos] == ' ') {
                pos++;
            }
        }
        if ((i == 0 && val[pos] != ',')
            || (i == 1 && val[pos] != ')')) {
            SHOW_ERROR("parse error: argument%d", i);
            return ret;
        }
        args[i] = SCV_parse_to_obj(owner, strarg);
        pos++;
        // Skip spaces:
        while (val[pos] == ' ') {
            pos++;
        }
    }

    if (strcmp(opcode, "POW2") == 0) {
        ret = &POW2(*args[1]);
    } else if (strcmp(opcode, "ADD") == 0) {
        ret = &ADD2(*args[0], *args[1]);
    } else if (strcmp(opcode, "SUB") == 0) {
        ret = &SUB2(*args[0], *args[1]);
    } else if (strcmp(opcode, "MUL") == 0) {
        ret = &MUL2(*args[0], *args[1]);
    } else if (strcmp(opcode, "DIV") == 0) {
        ret = &DIV2(*args[0], *args[1]);
    } else if (strcmp(opcode, "GT") == 0) {
        ret = &GT(*args[0], *args[1]);
    } else {
        SHOW_ERROR("%s", "wrong parse format");
    }
    return ret;
}

};
