#include "obs-internal.h"

const struct obs_protocol_info *find_protocol(const char *id)
{
	size_t i;
	for (i = 0; i < obs->protocol_types.num; i++)
		if (strcmp(obs->protocol_types.array[i].id, id) == 0)
			return obs->protocol_types.array + i;

	return NULL;
}

const char *obs_protocol_get_display_name(const char *id)
{
	const struct obs_protocol_info *info = find_protocol(id);
	return (info != NULL) ? info->get_name(info->type_data) : NULL;
}

obs_protocol_t *obs_protocol_create(const char *id, const char *name)
{
	const struct obs_protocol_info *info = find_protocol(id);
	struct obs_protocol *protocol;

	protocol = bzalloc(sizeof(struct obs_protocol));

	if (!info) {
		blog(LOG_ERROR, "Protocol ID '%s' not found", id);

		protocol->info.id = bstrdup(id);
		protocol->owns_info_id = true;
	} else {
		protocol->info = *info;
	}

	protocol->control = bzalloc(sizeof(obs_weak_protocol_t));
	protocol->control->protocol = protocol;

	obs_context_data_insert(&protocol->context, &obs->data.protocols_mutex,
				&obs->data.first_protocol);

	if (info)
		protocol->context.data =
			info->create(protocol->context.settings, protocol);
	if (!protocol->context.data)
		blog(LOG_ERROR, "Failed to create protocol '%s'!", name);

	blog(LOG_DEBUG, "protocol '%s' (%s) created", name, id);
	return protocol;
}

void obs_protocol_destroy(obs_protocol_t *protocol)
{
	if (protocol) {
		obs_context_data_remove(&protocol->context);

		blog(LOG_DEBUG, "protocol '%s' destroyed",
		     protocol->context.name);

		if (protocol->context.data)
			protocol->info.destroy(protocol->context.data);

		if (protocol->owns_info_id)
			bfree((void *)protocol->info.id);
		bfree(protocol);
	}
}

const char *obs_protocol_get_name(const obs_protocol_t *protocol)
{
	return obs_protocol_valid(protocol, "obs_protocol_get_name")
		       ? protocol->context.name
		       : NULL;
}

const char *obs_protocol_get_type(const obs_protocol_t *protocol)
{
	return obs_protocol_valid(protocol, "obs_protocol_get_type")
		       ? protocol->info.id
		       : NULL;
}

void obs_protocol_addref(obs_protocol_t *protocol)
{
	if (!protocol)
		return;

	obs_ref_addref(&protocol->control->ref);
}

void obs_protocol_release(obs_protocol_t *protocol)
{
	if (!protocol)
		return;

	obs_weak_protocol_t *control = protocol->control;
	if (obs_ref_release(&control->ref)) {
		// The order of operations is important here since
		// get_context_by_name in obs.c relies on weak refs
		// being alive while the context is listed
		obs_protocol_destroy(protocol);
		obs_weak_protocol_release(control);
	}
}

void obs_weak_protocol_addref(obs_weak_protocol_t *weak)
{
	if (!weak)
		return;

	obs_weak_ref_addref(&weak->ref);
}

void obs_weak_protocol_release(obs_weak_protocol_t *weak)
{
	if (!weak)
		return;

	if (obs_weak_ref_release(&weak->ref))
		bfree(weak);
}

obs_protocol_t *obs_protocol_get_ref(obs_protocol_t *protocol)
{
	if (!protocol)
		return NULL;

	return obs_weak_protocol_get_protocol(protocol->control);
}

obs_weak_protocol_t *obs_protocol_get_weak_protocol(obs_protocol_t *protocol)
{
	if (!protocol)
		return NULL;

	obs_weak_protocol_t *weak = protocol->control;
	obs_weak_protocol_addref(weak);
	return weak;
}

obs_protocol_t *obs_weak_protocol_get_protocol(obs_weak_protocol_t *weak)
{
	if (!weak)
		return NULL;

	if (obs_weak_ref_get_ref(&weak->ref))
		return weak->protocol;

	return NULL;
}

bool obs_weak_protocol_references_protocol(obs_weak_protocol_t *weak,
					   obs_protocol_t *protocol)
{
	return weak && protocol && weak->protocol == protocol;
}

void *obs_protocol_get_type_data(obs_protocol_t *protocol)
{
	return obs_protocol_valid(protocol, "obs_protocol_get_type_data")
		       ? protocol->info.type_data
		       : NULL;
}

const char *obs_protocol_get_id(const obs_protocol_t *protocol)
{
	return obs_protocol_valid(protocol, "obs_protocol_get_id")
		       ? protocol->info.id
		       : NULL;
}

const char *obs_protocol_get_url_prefix(const obs_protocol_t *protocol)
{
	return obs_protocol_valid(protocol, "obs_protocol_get_url_prefix")
		       ? protocol->info.url_prefix
		       : NULL;
}

const char *obs_get_protocol_url_prefix(const char *id)
{
	const struct obs_protocol_info *info = find_protocol(id);
	return info ? info->url_prefix : NULL;
}

const char *obs_protocol_get_recommended_output(const obs_protocol_t *protocol)
{
	return obs_protocol_valid(protocol,
				  "obs_protocol_get_recommended_output")
		       ? protocol->info.recommended_output
		       : NULL;
}

const char *obs_get_protocol_recommended_output(const char *id)
{
	const struct obs_protocol_info *info = find_protocol(id);
	return info ? info->recommended_output : NULL;
}
