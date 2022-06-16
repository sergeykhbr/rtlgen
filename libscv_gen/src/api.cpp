#include "api.h"
#include <stdlib.h>
#include <dirent.h>

namespace sysvc {

int SCV_is_dir_exists(const char *path) {
#ifdef _WIN32
    wchar_t wpath[4096];
    mbstowcs(wpath, path, sizeof(wpath));
    DWORD dwAttr = GetFileAttributesW(wpath);
    return (dwAttr != INVALID_FILE_ATTRIBUTES && 
         (dwAttr & FILE_ATTRIBUTE_DIRECTORY)) ? 1: 0;
#else
    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
        return 1;
    }
    return 0;
#endif
}

void SCV_create_dir(const char *path) {
    if (SCV_is_dir_exists(path)) {
        return;
    }
#if defined(_WIN32) || defined(__CYGWIN__)
    wchar_t wfulldir[4096];
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
    mbstowcs(wfulldir, path, 4096);
    _wmkdir(wfulldir);
#else
    mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

void SCV_write_file(const char *fname, const char *buf, size_t sz) {
    FILE *f = fopen(fname, "wb");
    if (f) {
        fwrite(buf, 1, sz, f);
        fclose(f);
    } else {
        RISCV_printf("error: cannot open file %s\n", fname);
    }
}


}
