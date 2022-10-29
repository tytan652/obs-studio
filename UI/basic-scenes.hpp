#pragma once

#include <memory>

#include <QWidget>

class OBSBasic;

#include "ui_OBSBasicScenes.h"

class OBSBasicScenes : public QWidget {
	Q_OBJECT

	// XXX: OBSBasic still require access to the scenes tree
	friend class OBSBasic;

	std::unique_ptr<Ui::OBSBasicScenes> ui;

public:
	OBSBasicScenes(OBSBasic *main);
	inline ~OBSBasicScenes(){};

	inline void ToggleToolbarVisibility(bool visible)
	{
		ui->scenesToolbar->setVisible(visible);
	}
};
