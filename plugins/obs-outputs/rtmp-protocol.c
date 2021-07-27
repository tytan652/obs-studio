#include <util/threading.h>
#include <obs-module.h>

static const char *rtmp_protocol_getname(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return "Real Time Messaging Protocol (RTMP)";
}

static const char *rtmps_protocol_getname(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return "Real Time Messaging Protocol Secure (RTMPS)";
}

static void *rtmp_protocol_create(obs_data_t *settings,
				  obs_protocol_t *protocol)
{
	UNUSED_PARAMETER(settings);
	UNUSED_PARAMETER(protocol);

	return NULL;
}

static void rtmp_protocol_destroy(void *data)
{
	UNUSED_PARAMETER(data);
}

struct obs_protocol_info rtmp_protocol_info = {
	.id = "RTMP",
	.url_prefix = "rtmp://",
	.get_name = rtmp_protocol_getname,
	.create = rtmp_protocol_create,
	.destroy = rtmp_protocol_destroy,
	.recommended_output = "rtmp_output",
};

struct obs_protocol_info rtmps_protocol_info = {
	.id = "RTMPS",
	.url_prefix = "rtmps://",
	.get_name = rtmps_protocol_getname,
	.create = rtmp_protocol_create,
	.destroy = rtmp_protocol_destroy,
	.recommended_output = "rtmps_output",
};
