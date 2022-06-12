#ifndef UHERO_RES_FONTATLAS_H__
#define UHERO_RES_FONTATLAS_H__ 1

#include "../gfx/font.h"

#define UH_FONTATLAS_ID 0x00aabb00

API result_t font_loadAtlas(font_t* out_fnt, const char* filename);

#endif