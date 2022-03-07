#pragma once

#include <memory>

#include <QWidget>
#include <QPointer>

class OBSBasic;

#include "ui_OBSBasicSources.h"

class OBSBasicSources : public QWidget {
	Q_OBJECT

	// XXX: OBSBasic still require access to the sources tree
	friend class OBSBasic;

	std::unique_ptr<Ui::OBSBasicSources> ui;

public:
	OBSBasicSources(OBSBasic *main);
	inline ~OBSBasicSources() {}

	inline void ToggleToolbarVisibility(bool visible)
	{
		ui->sourcesToolbar->setVisible(visible);
	}
};
