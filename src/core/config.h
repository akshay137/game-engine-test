#ifndef UHERO_CONFIG_H__
#define UHERO_CONFIG_H__ 1

#include "core.h"
#include "version.h"
#include "../ds/str.h"

struct config_s
{
	version_t app_version;
	str_t app_name;
	int32_t width;
	int32_t height;
	int32_t display_index;

	size_t max_sprites;
};

// read config from the `filename`
// if reading fails, default values will be written to config
API config_t config_read(const char* filename);
API result_t config_write(const config_t* config, const char* filename);

#endif