#include "values.h"
#include "utils.h"

std::string Int32Const::generate_sysc() {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d",
                    static_cast<int32_t>(getValue()));
    return std::string(tstr);
}

