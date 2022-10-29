#pragma once

#include <memory>

#include <QWidget>

class OBSBasic;

#include "ui_OBSBasicSources.h"

class OBSBasicSources : public QWidget {
	Q_OBJECT

	// XXX: Those still require access to the sources tree
	friend class OBSBasic;
	friend class OBSBasicPreview;

	std::unique_ptr<Ui::OBSBasicSources> ui;

public:
	OBSBasicSources(OBSBasic *main);
	inline ~OBSBasicSources(){};

	inline void ToggleToolbarVisibility(bool visible)
	{
		ui->sourcesToolbar->setVisible(visible);
	}
};
