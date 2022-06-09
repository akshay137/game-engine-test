#include "config.h"
#include "logger.h"

config_t config_read(const char* filename)
{
	// set it to default values
	config_t conf = {
		.app_version = version(0, 1, 0),
		.app_name = str("pong"),
		.width = 1280,
		.height = 720,
		.display_index = 0,
		.max_sprites = 1024
	};

	// TODO: read config file and update values which exists in file
	INFO("Reading config from: %s\n", filename);

	return conf;
}

result_t config_write(const config_t* config, const char* filename)
{
	INFO("Writing config at %p to %s\n", config, filename);
	return UH_SUCCESS;
}