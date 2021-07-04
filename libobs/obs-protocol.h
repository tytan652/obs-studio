#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct obs_protocol_codec {
	char *name;
};

struct obs_protocol_info {
	const char *id;
	const char *url_prefix;

	const char *(*get_name)(void *type_data);

	void *(*create)(obs_data_t *settings, obs_protocol_t *output);
	void (*destroy)(void *data);

	void *type_data;
	void (*free_type_data)(void *type_data);

	bool video_codec_agnostic;
	bool audio_codec_agnostic;

	void (*get_supported_video_codecs)(void *data,
					   struct obs_protocol_codec **codecs,
					   size_t *count);
	void (*get_supported_audio_codecs)(void *data,
					   struct obs_protocol_codec **codecs,
					   size_t *count);

	const char *recommended_output;
};

EXPORT void obs_register_protocol_s(const struct obs_protocol_info *info,
				    size_t size);

#define obs_register_protocol(info) \
	obs_register_protocol_s(info, sizeof(struct obs_protocol_info))

#ifdef __cplusplus
}
#endif
