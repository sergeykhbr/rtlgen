#pragma once

#define RISCV_free free
#define RISCV_malloc malloc
#define RISCV_sprintf sprintf_s
#define RISCV_printf printf
#define LOG_ERROR 1

#ifdef _WIN32
    #define RV_PRI64 "I64"
#else
#endif


int SCV_is_dir_exists(const char *path);
void SCV_create_dir(const char *path);
void SCV_write_file(const char *fname, const char *buf, size_t sz);

