#pragma once

#include <QWidget>

class OBSBasic;

#include "ui_OBSBasicTransitions.h"

class OBSBasicTransitions : public QWidget {
	Q_OBJECT

	// XXX: OBSBasic still requires access to transitions combobox
	friend class OBSBasic;

	std::unique_ptr<Ui::OBSBasicTransitions> ui;

public:
	OBSBasicTransitions(OBSBasic *main);
	inline ~OBSBasicTransitions() {}

private slots:
	void TransitionChanged(bool has_duration, bool has_config);
};
