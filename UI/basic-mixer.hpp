#pragma once

#include <memory>

#include <QWidget>

class OBSBasic;
class VolControl;

#include "ui_OBSBasicMixer.h"

class OBSBasicMixer : public QWidget {
	Q_OBJECT

	OBSBasic *main;
	std::unique_ptr<Ui::OBSBasicMixer> ui;

	bool vertical = false;

public:
	OBSBasicMixer(OBSBasic *main);
	inline ~OBSBasicMixer(){};

	void ToggleMixerLayout(bool vertical);

	void AddVolumeControl(VolControl *vol);

private slots:
	void on_actionMixerToolbarMenu_triggered();
};
