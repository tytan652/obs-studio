#pragma once

#include <memory>

#include <QWidget>
#include <QPointer>

#include "volume-control.hpp"

class OBSBasic;

#include "ui_OBSBasicMixer.h"

class OBSBasicMixer : public QWidget {
	Q_OBJECT

	std::unique_ptr<Ui::OBSBasicMixer> ui;

	bool verticalLayout = false;

public:
	OBSBasicMixer(OBSBasic *main);
	inline ~OBSBasicMixer() {}

	void ToggleMixerLayout(bool vertical);

	void AddVolumeControl(VolControl *vol);
};
