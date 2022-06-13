#pragma once
#include <stdio.h>

#include <attribute.h>
#define RISCV_free free
#define RISCV_malloc malloc
#define RISCV_sprintf sprintf_s
#define RISCV_printf printf
#define LOG_ERROR 1

#ifdef _WIN32
    #define RV_PRI64 "I64"
#else
#endif

