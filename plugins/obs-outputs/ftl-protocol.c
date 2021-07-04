#include <util/threading.h>
#include <obs-module.h>

struct ftl_codecs {
	DARRAY(struct obs_protocol_codec) video_codecs;
	DARRAY(struct obs_protocol_codec) audio_codecs;
};

static const char *ftl_protocol_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Faster Than Light (FTL) Protocol";
}

static void *ftl_protocol_create(obs_data_t *settings, obs_protocol_t *protocol)
{
	UNUSED_PARAMETER(settings);
	UNUSED_PARAMETER(protocol);

	struct ftl_codecs *data = bzalloc(sizeof(struct ftl_codecs));

	da_init(data->video_codecs);
	da_init(data->audio_codecs);

	const char *video_codecs[] = {"H.264"};
	const char *audio_codecs[] = {"opus"};

	for (size_t i = 0; i < sizeof(video_codecs) / sizeof(video_codecs[0]);
	     i++) {
		struct obs_protocol_codec codec;
		codec.name = strdup(video_codecs[i]);
		da_push_back(data->video_codecs, &codec);
	}

	for (size_t i = 0; i < sizeof(audio_codecs) / sizeof(audio_codecs[0]);
	     i++) {
		struct obs_protocol_codec codec;
		codec.name = strdup(audio_codecs[i]);
		da_push_back(data->audio_codecs, &codec);
	}

	return data;
}

static void ftl_protocol_destroy(void *data)
{
	struct ftl_codecs *codecs = data;

	da_free(codecs->video_codecs);
	da_free(codecs->audio_codecs);

	bfree(codecs);
}

static void ftl_protocol_get_video_codec(void *data,
					 struct obs_protocol_codec **codec,
					 size_t *count)
{
	struct ftl_codecs *av_codecs = data;
	DARRAY(struct obs_protocol_codec) video_codecs;
	da_init(video_codecs);
	da_copy(video_codecs, av_codecs->video_codecs);

	*count = video_codecs.num;
	*codec = video_codecs.array;
}

static void ftl_protocol_get_audio_codec(void *data,
					 struct obs_protocol_codec **codec,
					 size_t *count)
{
	struct ftl_codecs *av_codecs = data;
	DARRAY(struct obs_protocol_codec) audio_codecs;
	da_init(audio_codecs);
	da_copy(audio_codecs, av_codecs->audio_codecs);

	*count = audio_codecs.num;
	*codec = audio_codecs.array;
}

struct obs_protocol_info ftl_protocol_info = {
	.id = "FTL",
	.url_prefix = "ftl://",
	.get_name = ftl_protocol_getname,
	.create = ftl_protocol_create,
	.destroy = ftl_protocol_destroy,
	.video_codec_agnostic = false,
	.audio_codec_agnostic = false,
	.get_supported_video_codecs = ftl_protocol_get_video_codec,
	.get_supported_audio_codecs = ftl_protocol_get_audio_codec,
	.recommended_output = "ftl_output",
};
