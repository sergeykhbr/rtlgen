#include "river_top.h"

RiverTop::RiverTop(GenObject *parent,
                   river_cfg &cfg) :
    ModuleObject(parent, "RiverTop"),
    io_start_(this),
    i_clk(this, "i_clk", ConstParam(1), "CPU clock"),
    i_nrst(this, "i_nrst", ConstParam(1), "Reset: active LOW"),
    cmt1(this, "Memory interface:"),
    i_req_mem_ready(this, "i_req_mem_ready", ConstParam(1), "System Bus is ready to accept memory operation request"),
    o_req_mem_path(this, "o_req_mem_path", ConstParam(1), "0=ctrl; 1=data path"),
    o_req_mem_valid(this, "o_req_mem_valid", ConstParam(1), "AXI memory request is valid"),
    o_req_mem_type(this, "o_req_mem_type", cfg.REQ_MEM_TYPE_BITS, "AXI memory request type"),
    o_req_mem_size(this, "o_req_mem_size", ConstParam(3), "request size: 0=1 B;...; 7=128 B"),
    o_req_mem_addr(this, "o_req_mem_addr", cfg.REQ_MEM_TYPE_BITS, "AXI memory request address"),
    o_req_mem_strob(this, "o_req_mem_strob", cfg.REQ_MEM_TYPE_BITS, "Writing strob. 1 bit per Byte (uncached only)"),
    o_req_mem_data(this, "o_req_mem_data", cfg.REQ_MEM_TYPE_BITS, "Writing data"),
    i_resp_mem_valid(this, "i_resp_mem_valid", ConstParam(1), "AXI response is valid"),
    i_resp_mem_path(this, "i_resp_mem_path", ConstParam(1), "0=ctrl; 1=data path"),
    i_resp_mem_data(this, "i_resp_mem_data", cfg.REQ_MEM_TYPE_BITS, "Read data"),
    i_resp_mem_load_fault(this, "i_resp_mem_load_fault", ConstParam(1), "data load error"),
    i_resp_mem_store_fault(this, "i_resp_mem_store_fault", ConstParam(1), "data store error"),
    io_end_(this)
{
}
