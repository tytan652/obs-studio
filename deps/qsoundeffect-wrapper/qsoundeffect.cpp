#include <QUrl>
#include <QSoundEffect>

#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
EXPORT QSoundEffect *q_sound_effect_new(QObject *parent)
{
	return new QSoundEffect(parent);
}

EXPORT void q_sound_effect_delete(QSoundEffect *self)
{
	if (self)
		delete self;
}

EXPORT void q_sound_effect_setSource(QSoundEffect *self, const QUrl &url)
{
	self->setSource(url);
}

EXPORT void q_sound_effect_play(QSoundEffect *self)
{
	self->play();
}
}
