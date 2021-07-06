#include <util/threading.h>
#include <obs-module.h>

struct hls_codecs {
	DARRAY(struct obs_protocol_codec) video_codecs;
	DARRAY(struct obs_protocol_codec) audio_codecs;
};

static const char *hls_protocol_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "HTTP Live Streaming (HLS)";
}

static void *hls_protocol_create(obs_data_t *settings, obs_protocol_t *protocol)
{
	UNUSED_PARAMETER(settings);
	UNUSED_PARAMETER(protocol);

	struct hls_codecs *data = bzalloc(sizeof(struct hls_codecs));

	da_init(data->video_codecs);
	da_init(data->audio_codecs);

	const char *video_codecs[] = {"H.264"};
	const char *audio_codecs[] = {"AAC", "HE-AAC", "MP3", "AC-3", "E-AC-3"};

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

static void hls_protocol_destroy(void *data)
{
	struct hls_codecs *codecs = data;

	da_free(codecs->video_codecs);
	da_free(codecs->audio_codecs);

	bfree(codecs);
}

static void hls_protocol_get_video_codecs(void *data,
					  struct obs_protocol_codec **codecs,
					  size_t *count)
{
	struct hls_codecs *av_codecs = data;
	DARRAY(struct obs_protocol_codec) video_codecs;
	da_init(video_codecs);
	da_copy(video_codecs, av_codecs->video_codecs);

	*count = video_codecs.num;
	*codecs = video_codecs.array;
}

static void hls_protocol_get_audio_codecs(void *data,
					  struct obs_protocol_codec **codecs,
					  size_t *count)
{
	struct hls_codecs *av_codecs = data;
	DARRAY(struct obs_protocol_codec) audio_codecs;
	da_init(audio_codecs);
	da_copy(audio_codecs, av_codecs->audio_codecs);

	*count = audio_codecs.num;
	*codecs = audio_codecs.array;
}

struct obs_protocol_info hls_protocol_info = {
	.id = "HLS",
	.get_name = hls_protocol_getname,
	.create = hls_protocol_create,
	.destroy = hls_protocol_destroy,
	.video_codec_agnostic = false,
	.audio_codec_agnostic = false,
	.get_supported_video_codecs = hls_protocol_get_video_codecs,
	.get_supported_audio_codecs = hls_protocol_get_audio_codecs,
	.recommended_output = "ffmpeg_hls_muxer",
};
