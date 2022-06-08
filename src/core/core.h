#ifndef UHERO_COMMON_H__
#define UHERO_COMMON_H__ 1

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <assert.h>

#define UHERO_CONFIG_FILE "uhero.conf"

#define API extern
#define API_INLINE static inline

enum result_e
{
	UH_SUCCESS = 0,
	UH_FAILURE = -1,
	UH_EXTERNERL_LIB_ERROR = -2,
	UH_IO_ERROR = -3,
};
typedef enum result_e result_t;

#define UH_TDEF_STRUCT(s) typedef struct s##_s s##_t
#define UH_TDEF_UNION(u) typedef union u##_u u##_t

// common structs, can be used all over the code base
UH_TDEF_STRUCT(version);
UH_TDEF_STRUCT(config);
UH_TDEF_STRUCT(window);

UH_TDEF_STRUCT(gfx);
UH_TDEF_STRUCT(pso);
UH_TDEF_STRUCT(buffer);
UH_TDEF_STRUCT(texture);

UH_TDEF_STRUCT(vertex2d);
UH_TDEF_STRUCT(vertex3d);

UH_TDEF_STRUCT(file);

UH_TDEF_STRUCT(str);

API result_t uhero_init(void);
API void uhero_shutdown(void);

#endif