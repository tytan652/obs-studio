#include <util/threading.h>
#include <obs-module.h>

static const char *srt_protocol_getname(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return "Secure Reliable Transport (SRT) Protocol";
}

static void *srt_protocol_create(obs_data_t *settings, obs_protocol_t *protocol)
{
	UNUSED_PARAMETER(settings);
	UNUSED_PARAMETER(protocol);

	return NULL;
}

static void srt_protocol_destroy(void *data)
{
	UNUSED_PARAMETER(data);
}

struct obs_protocol_info srt_protocol_info = {
	.id = "SRT",
	.url_prefix = "srt://",
	.get_name = srt_protocol_getname,
	.create = srt_protocol_create,
	.destroy = srt_protocol_destroy,
};
