#include "river_cfg.h"

river_cfg::river_cfg(GenObject *parent) :
    FileObject(parent, "river_cfg"),
    _1_(this, ""),
    _2_(this, "2**Number of CPU slots in the clusters. Some of them could be unavailable"),
    _3_(this, ""),
    CFG_LOG2_CPU_MAX(this, "CFG_LOG2_CPU_MAX", new I32D(2)),
    CFG_CPU_MAX(this, "CFG_CPU_MAX", new I32D("POW2(1,CFG_LOG2_CPU_MAX)")),
    _4_(this),
    CFG_VENDOR_ID(this, "CFG_VENDOR_ID", new UI64H("0x000000F1")),
    CFG_IMPLEMENTATION_ID(this, "CFG_IMPLEMENTATION_ID", new UI64H("0x20191123")),
    CFG_HW_FPU_ENABLE(this, "CFG_HW_FPU_ENABLE", new BOOL(true)),
    _5_(this),
    RISCV_ARCH(this, "RISCV_ARCH", new I32D(64)),
    _6_(this),
    CFG_CPU_ADDR_BITS(this, "CFG_CPU_ADDR_BITS", new I32D("CFG_BUS_ADDR_WIDTH")),
    CFG_CPU_ID_BITS(this, "CFG_CPU_ID_BITS", new I32D(1)),
    CFG_CPU_USER_BITS(this, "CFG_CPU_USER_BITS", new I32D(1)),
    _7_(this),
    _8_(this, ""),
    _9_(this, "Branch Predictor Branch Target Buffer (BTB) size"),
    _10_(this, ""),
    CFG_BTB_SIZE(this, "CFG_BTB_SIZE", new I32D(8)),
    _11_(this, "Branch predictor depth. It is better when it is equal to the pipeline depth excluding fetcher."),
    _12_(this, "Let it be equal to the decoder's history depth"),
    CFG_BP_DEPTH(this, "CFG_BP_DEPTH", new I32D(5)),
    _13_(this),
    _14_(this, ""),
    _15_(this, "Decoded instructions history buffer size in Decoder"),
    _16_(this, ""),
    CFG_DEC_DEPTH(this, "CFG_DEC_DEPTH", new I32D("SUB(CFG_BP_DEPTH,3)"), "requested, fetching, fetched"),
    _17_(this),
    _18_(this, "Power-on start address can be free changed"),
    CFG_RESET_VECTOR(this, "CFG_RESET_VECTOR", new UI64H(0x10000)),
    _19_(this),
    _20_(this, "Valid size 0..16"),
    CFG_PROGBUF_REG_TOTAL(this, "CFG_PROGBUF_REG_TOTAL", new I32D(16)),
    _21_(this, "Must be at least 2 to support RV64I"),
    CFG_DATA_REG_TOTAL(this, "CFG_DATA_REG_TOTAL", new I32D(4)),
    _22_(this, "Total number of dscratch registers"),
    CFG_DSCRATCH_REG_TOTAL(this, "CFG_DSCRATCH_REG_TOTAL", new I32D(2)),
    _23_(this, "Number of elements each 2*CFG_ADDR_WIDTH in stack trace buffer:"),
    CFG_LOG2_STACK_TRACE_ADDR(this, "CFG_LOG2_STACK_TRACE_ADDR", new I32D(5)),
    STACK_TRACE_BUF_SIZE(this, "STACK_TRACE_BUF_SIZE", new I32D("POW2(1,CFG_LOG2_STACK_TRACE_ADDR)")),
    _24_(this),
    _25_(this, ""), 
    _26_(this, "ICacheLru config (16 KB by default)"),
    _27_(this, ""),
    CFG_ILOG2_BYTES_PER_LINE(this, "CFG_ILOG2_BYTES_PER_LINE", new I32D(5), "[4:0] 32 Bytes = 4x8 B log2(Bytes per line)"),
    CFG_ILOG2_LINES_PER_WAY(this, "CFG_ILOG2_LINES_PER_WAY", new I32D(7)),
    CFG_ILOG2_NWAYS(this, "CFG_ILOG2_NWAYS", new I32D(2)),
    _28_(this),
    _29_(this, "Derivatives I$ constants:"),
    ICACHE_BYTES_PER_LINE(this, "ICACHE_BYTES_PER_LINE", new I32D("POW2(1,CFG_ILOG2_BYTES_PER_LINE)")),
    ICACHE_LINES_PER_WAY(this, "ICACHE_LINES_PER_WAY", new I32D("POW2(1,CFG_ILOG2_LINES_PER_WAY)")),
    ICACHE_WAYS(this, "ICACHE_WAYS", new I32D("POW2(1,CFG_ILOG2_NWAYS)")),
    ICACHE_LINE_BITS(this, "ICACHE_LINE_BITS", new I32D("MUL(8,ICACHE_BYTES_PER_LINE)")),
    _30_(this),
    _31_(this, "Information: To define the CACHE SIZE in Bytes use the following:"),
    ICACHE_SIZE_BYTES(this, "ICACHE_SIZE_BYTES", new I32D("MUL(ICACHE_WAYS,MUL(ICACHE_LINES_PER_WAY,ICACHE_BYTES_PER_LINE))")),
    _32_(this),
    ITAG_FL_TOTAL(this, "ITAG_FL_TOTAL", new I32D(1)),
    _33_(this),
    _34_(this),
    _35_(this, ""),
    _36_(this, "DCacheLru config (16 KB by default)"),
    _37_(this, ""),
    CFG_DLOG2_BYTES_PER_LINE(this, "CFG_DLOG2_BYTES_PER_LINE", new I32D(5), "[4:0] 32 Bytes = 4x8 B log2(Bytes per line)"),
    CFG_DLOG2_LINES_PER_WAY(this, "CFG_DLOG2_LINES_PER_WAY", new I32D(7)),
    CFG_DLOG2_NWAYS(this, "CFG_DLOG2_NWAYS", new I32D(2)),
    _38_(this),
    _39_(this, "Derivatives D$ constants:"),
    DCACHE_BYTES_PER_LINE(this, "DCACHE_BYTES_PER_LINE", new I32D("POW2(1,CFG_DLOG2_BYTES_PER_LINE)")),
    DCACHE_LINES_PER_WAY(this, "DCACHE_LINES_PER_WAY", new I32D("POW2(1,CFG_DLOG2_LINES_PER_WAY)")),
    DCACHE_WAYS(this, "DCACHE_WAYS", new I32D("POW2(1,CFG_DLOG2_NWAYS)")),
    _40_(this),
    DCACHE_LINE_BITS(this, "DCACHE_LINE_BITS", new I32D("MUL(8,DCACHE_BYTES_PER_LINE)")),
    _41_(this),
    _42_(this, "Information: To define the CACHE SIZE in Bytes use the following:"),
    DCACHE_SIZE_BYTES(this, "DCACHE_SIZE_BYTES", new I32D("MUL(DCACHE_WAYS,MUL(DCACHE_LINES_PER_WAY,DCACHE_BYTES_PER_LINE))")),
    _43_(this),
    TAG_FL_VALID(this, "TAG_FL_VALID", new I32D(0), "always 0"),
    DTAG_FL_DIRTY(this, "DTAG_FL_DIRTY", new I32D(1)),
    DTAG_FL_SHARED(this, "DTAG_FL_SHARED", new I32D(2)),
    DTAG_FL_RESERVED(this, "DTAG_FL_RESERVED", new I32D(3)),
    DTAG_FL_TOTAL(this, "DTAG_FL_TOTAL", new I32D(4)),
    _44_(this),
    _45_(this),
    _46_(this, ""),
    _47_(this, "L1 cache common parameters (suppose I$ and D$ have the same size)"),
    _48_(this, ""),
    L1CACHE_BYTES_PER_LINE(this, "L1CACHE_BYTES_PER_LINE", new I32D("DCACHE_BYTES_PER_LINE")),
    L1CACHE_LINE_BITS(this, "L1CACHE_LINE_BITS", new I32D("MUL(8,DCACHE_BYTES_PER_LINE)")),
    _49_(this),
    REQ_MEM_TYPE_WRITE(this, "REQ_MEM_TYPE_WRITE", new I32D(0)),
    REQ_MEM_TYPE_CACHED(this, "REQ_MEM_TYPE_CACHED", new I32D(1)),
    REQ_MEM_TYPE_UNIQUE(this, "REQ_MEM_TYPE_UNIQUE", new I32D(2)),
    REQ_MEM_TYPE_BITS(this, "REQ_MEM_TYPE_BITS", new I32D(3)),
    _50_(this),
    SNOOP_REQ_TYPE_READDATA(this, "SNOOP_REQ_TYPE_READDATA", new I32D(0), "0=check flags; 1=data transfer"),
    SNOOP_REQ_TYPE_READCLEAN(this, "SNOOP_REQ_TYPE_READCLEAN", new I32D(1), "0=do nothing; 1=read and invalidate line"),
    SNOOP_REQ_TYPE_BITS(this, "SNOOP_REQ_TYPE_BITS", new I32D(2)),
    _51_(this),
    _n_(this)


{
}