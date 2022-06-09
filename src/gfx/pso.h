#ifndef UHERO_PSO_H__
#define UHERO_PSO_H__ 1

#include "../core/core.h"

enum vattrib_e
{
	UHVATTRIB_NONE = 0,
	UHVATTRIB_VEC2 = 1,
	UHVATTRIB_VEC3 = 2,
	UHVATTRIB_VEC4 = 3,
};
typedef enum vattrib_e vattrib_t;

enum geom_e
{
	UHGEOM_TRIANGLES = 0,
	UHGEOM_LINES = 1,
};
typedef enum geom_e geom_t;

enum fillmode_e
{
	UHFILLMODE_FILL = 0,
};
typedef enum fillmode_e fillmode_t;

enum cullmode_e
{
	UHCULLMODE_CULL_BACK = 0,
	UHCULLMODE_CULL_FRONT = 1
};
typedef enum cullmode_e cullmode_t;

enum fbtype_e
{
	UHFRAMEBUFFER_DEFAULT = 0,
};
typedef enum fbtype_e fbtype_t;

/*
pipeline state object
contains:
	vertex array object
	shader program
	other states [geometry mode | blend | depth | ... ]
*/		
struct pso_s
{
	uint32_t vao;
	uint32_t program;

	geom_t _geometry;
	fillmode_t _fill;
	cullmode_t _cull;
};

API result_t pso_create(pso_t* out_pso,
	str_t* shaders,
	vattrib_t* vertex_attribs,
	geom_t geom_type, fillmode_t fillmode, cullmode_t cullmode
);

API result_t pso_new(pso_t* out_pso);
API void pso_delete(pso_t* pso);

API void pso_makeCurrent(pso_t* pso);

API void pso_setVertexBuffer(pso_t* pso, uint32_t index,
	buffer_t* buffer, uint32_t offset, uint32_t stride
);
API void pso_setIndexBuffer(pso_t* pso, buffer_t* buffer);

API void pso_drawArrays(pso_t* pso, uint32_t first, uint32_t count);

#endif