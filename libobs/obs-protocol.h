#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct obs_protocol_info {
	const char *id;

	const char *(*get_name)(void *type_data);

	void *(*create)(obs_data_t *settings, obs_protocol_t *protocol);
	void (*destroy)(void *data);

	void *type_data;
	void (*free_type_data)(void *type_data);
};

EXPORT void obs_register_protocol_s(const struct obs_protocol_info *info,
				    size_t size);

#define obs_register_protocol(info) \
	obs_register_protocol_s(info, sizeof(struct obs_protocol_info))

#ifdef __cplusplus
}
#endif
