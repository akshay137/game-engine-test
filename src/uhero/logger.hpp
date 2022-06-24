#ifndef UHERO_LOGGER_H__
#define UHERO_LOGGER_H__ 1

#include "common.hpp"

// temp
#include <cstdio>

#define UH_LOG(file, fmt, ...) fprintf(file, fmt, ##__VA_ARGS__)
#define UH_INFO(fmt, ...) UH_LOG(stdout, "[INFO] " fmt, ##__VA_ARGS__)
#define UH_WARN(fmt, ...) UH_LOG(stdout, "[WARN] " fmt, ##__VA_ARGS__)
#define UH_ERROR(fmt, ...) UH_LOG(stderr, "[ERROR] " fmt, ##__VA_ARGS__)
#define UH_VERB(fmt, ...) UH_LOG(stdout, "[VERB] " fmt, ##__VA_ARGS__)

// convinience macro for SDL error reporting
#define UHSDL_ERROR(fn) UH_ERROR(#fn ": %s\n", SDL_GetError())

#define DUMPVAR(fs, id, ...) UH_VERB(#id " = " fs " {%s:%d}\n", ##__VA_ARGS__)
#define DUMPI(i) DUMPVAR("%d", i, i, __FILE__, __LINE__)
#define DUMPU(u) DUMPVAR("%u", u, u, __FILE__, __LINE__)
#define DUMPX(x) DUMPVAR("%x", x, x, __FILE__, __LINE__)
#define DUMPF(f) DUMPVAR("%f", f, f, __FILE__, __LINE__)
#define DUMPPTR(p) DUMPVAR("%p", p, p, __FILE__, __LINE__)

namespace uhero
{}

#endif