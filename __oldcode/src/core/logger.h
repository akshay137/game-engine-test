#ifndef UHERO_LOGGER_H__
#define UHERO_LOGGER_H__ 1

#include "core.h"

// TODO: remove stdio from this header
#include <stdio.h>

#ifdef PLT_LINUX
#define SHC(c) "\x1B[" #c "m"
#define LC_NORM SHC(0)
#define LC_GRN SHC(32)
#define LC_BLU SHC(34)
#define LC_MAG SHC(35)
#define LC_YEL SHC(93)
#define LC_RED SHC(31)
#else
#define LC_NORM
#define LC_GRN
#define LC_BLU
#define LC_MAG
#define LC_YEL
#define LC_RED
#endif


#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define LOGCAT(cat, fmt, ...) LOG(cat " " fmt, ##__VA_ARGS__)

#define INFO(fmt, ...) LOGCAT(LC_GRN "[UH_INFO]" LC_NORM, fmt, ##__VA_ARGS__)
#define VERB(fmt, ...) LOGCAT(LC_MAG "[UH_VERB]" LC_NORM, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) LOGCAT(LC_YEL "[UH_WARN]" LC_NORM, fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) LOGCAT(LC_RED "[UH_ERR]" LC_NORM, fmt, ##__VA_ARGS__)

#define DUMPV(fs, id, ...) VERB(#id " => " fs " {%s@%d}\n", ##__VA_ARGS__)

#define DUMPI(i) DUMPV("%d", i, i, __FILE__, __LINE__)
#define DUMPU(u) DUMPV("%u", u, u, __FILE__, __LINE__)
#define DUMPX(x) DUMPV("%X", x, x, __FILE__, __LINE__)
#define DUMPPTR(p) DUMPV("%p", p, p, __FILE__, __LINE__)
#define DUMPCSTR(s) DUMPV("%s", s, s, __FILE__, __LINE__)

#define DUMPSTR(s) DUMPV("%.*s", s, (int)s._size, s.ptr, __FILE__, __LINE__)
#define DUMPVER(v) DUMPV("%u.%u.%u", v, v.major, v.minor, v.patch, __FILE__, __LINE__)

#endif