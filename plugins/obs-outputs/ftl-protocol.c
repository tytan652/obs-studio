#include <util/threading.h>
#include <obs-module.h>

static const char *ftl_protocol_getname(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return "Faster Than Light (FTL) Protocol";
}

static void *ftl_protocol_create(obs_data_t *settings, obs_protocol_t *protocol)
{
	UNUSED_PARAMETER(settings);
	UNUSED_PARAMETER(protocol);

	return NULL;
}

static void ftl_protocol_destroy(void *data)
{
	UNUSED_PARAMETER(data);
}

struct obs_protocol_info ftl_protocol_info = {
	.id = "FTL",
	.url_prefix = "ftl://",
	.get_name = ftl_protocol_getname,
	.create = ftl_protocol_create,
	.destroy = ftl_protocol_destroy,
};
