#ifndef UHERO_PSO_H__
#define UHERO_PSO_H__ 1

#include "../core/core.h"

enum vattrib_e
{
	VEC2 = 0,
	VEC3 = 1,
	VEC4 = 2,
};
typedef enum vattrib_e vattrib_t;

enum geom_e
{
	TRIANGLES = 0,
	LINES = 1,
};
typedef enum geom_e geom_t;

enum fillmode_e
{
	FILL = 0,
};
typedef enum fillmode_e fillmode_t;

enum cullmode_e
{
	CULL_BACK = 0,
	CULL_FRONT = 1
};
typedef enum cullmode_e cullmode_t;

/*
pipeline state object
contains:
	vertex array object
	shader program
	framebuffer
	other states [geometry mode | blend | depth | ... ]
*/		
struct pso_s
{
	uint32_t vao;
	uint32_t program;
	uint32_t framebuffer;
};

API result_t pso_create(pso_t* out_pso,
	str_t* shaders,
	vattrib_t* vertex_attribs,
	geom_t geom_type, fillmode_t fillmode, cullmode_t cullmode
);

API result_t pso_new(pso_t* out_pso);
API void pso_delete(pso_t* pso);

API void pso_makeCurrent(pso_t* pso);

#endif