#include "qsoundeffect-wrapper.hpp"

#ifdef _WIN32
#include <util/util.hpp>
#include <util/platform.h>

static void *libhandle = nullptr;

static OBSSoundEffect *(*q_sound_effect_new)(QObject *parent) = nullptr;
static void (*q_sound_effect_delete)(OBSSoundEffect *self) = nullptr;
static void (*q_sound_effect_setSource)(OBSSoundEffect *self,
					const QUrl &url) = nullptr;
static void (*q_sound_effect_play)(OBSSoundEffect *self) = nullptr;

#define DLSYM_CHECK(sym_name)                                             \
	if (!sym_name) {                                                  \
		blog(LOG_ERROR, "qsoundeffect-wrapper: "                  \
				"Function " #sym_name " not found, "      \
				"qsoundeffect-wrapper will be unloaded"); \
		unload_qsoundeffect();                                    \
	}

static inline void unload_qsoundeffect()
{
	os_dlclose(libhandle);
	libhandle = nullptr;

	blog(LOG_DEBUG, "qsoundeffect-wrapper unloaded");
}

static void load_qsoundeffect()
{
	libhandle = os_dlopen("qsoundeffect-wrapper.dll");

	if (!libhandle) {
		blog(LOG_DEBUG, "qsoundeffect-wrapper not loaded");
		return;
	}

	blog(LOG_DEBUG, "qsoundeffect-wrapper loaded");

	q_sound_effect_new =
		(void *(*)(QObject *))os_dlsym(libhandle, "q_sound_effect_new");
	DLSYM_CHECK(q_sound_effect_new);
	q_sound_effect_delete =
		(void (*)(void *))os_dlsym(libhandle, "q_sound_effect_delete");
	DLSYM_CHECK(q_sound_effect_delete);
	q_sound_effect_setSource = (void (*)(void *, const QUrl &url))os_dlsym(
		libhandle, "q_sound_effect_setSource");
	DLSYM_CHECK(q_sound_effect_setSource);
	q_sound_effect_play =
		(void (*)(void *))os_dlsym(libhandle, "q_sound_effect_play");
	DLSYM_CHECK(q_sound_effect_play);
}

#endif

bool obs_sound_effect_available()
{
#ifndef _WIN32
	return true;
#else
	if (!libhandle)
		load_qsoundeffect();

	return !!libhandle;
#endif
}

OBSSoundEffect *obs_sound_effect_new(QObject *parent)
{
#ifndef _WIN32
	return new QSoundEffect(parent);
#else
	if (!libhandle)
		load_qsoundeffect();

	if (!libhandle)
		return nullptr;

	return q_sound_effect_new(parent);
#endif
}

void obs_sound_effect_delete(OBSSoundEffect *self)
{
	if (!self)
		return;

#ifndef _WIN32
	delete self;
#else
	if (!libhandle)
		return;

	q_sound_effect_delete(self);
#endif
}

void obs_sound_effect_set_source(OBSSoundEffect *self, const QUrl &url)
{
	if (!self)
		return;

#ifndef _WIN32
	self->setSource(url);
#else
	if (!libhandle)
		return;

	q_sound_effect_setSource(self, url);
#endif
}

void obs_sound_effect_play(OBSSoundEffect *self)
{
	if (!self)
		return;

#ifndef _WIN32
	self->play();
#else
	if (!libhandle)
		return;

	q_sound_effect_play(self);
#endif
}
