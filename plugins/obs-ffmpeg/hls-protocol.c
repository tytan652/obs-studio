#include <util/threading.h>
#include <obs-module.h>

static const char *hls_protocol_getname(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return "HTTP Live Streaming (HLS)";
}

static void *hls_protocol_create(obs_data_t *settings, obs_protocol_t *protocol)
{
	UNUSED_PARAMETER(settings);
	UNUSED_PARAMETER(protocol);

	return NULL;
}

static void hls_protocol_destroy(void *data)
{
	UNUSED_PARAMETER(data);
}

struct obs_protocol_info hls_protocol_info = {
	.id = "HLS",
	.get_name = hls_protocol_getname,
	.create = hls_protocol_create,
	.destroy = hls_protocol_destroy,
};
