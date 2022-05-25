#pragma once

#include <QUrl>

#ifndef _WIN32
#include <QSoundEffect>

typedef QSoundEffect OBSSoundEffect;
#else
typedef void OBSSoundEffect;
#endif

bool obs_sound_effect_available();

OBSSoundEffect *obs_sound_effect_new(QObject *parent);

void obs_sound_effect_delete(OBSSoundEffect *self);

void obs_sound_effect_set_source(OBSSoundEffect *self, const QUrl &url);

void obs_sound_effect_play(OBSSoundEffect *self);
