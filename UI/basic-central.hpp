#pragma once

#include <memory>

#include <QWidget>
#include <QPointer>

class OBSBasic;

#include "ui_OBSBasicCentral.h"

class OBSBasicCentral : public QWidget {
	Q_OBJECT

	friend class OBSBasicPreview;
	// XXX: OBSBasic is deeply tied to the central widget
	friend class OBSBasic;

	OBSBasic *main;

	std::unique_ptr<Ui::OBSBasicCentral> ui;

public:
	OBSBasicCentral(OBSBasic *main);
	inline ~OBSBasicCentral() {}

	void EnablePreviewDisplay(bool enable);

	inline bool IsContextContainerVisible()
	{
		return ui->contextContainer->isVisible();
	}

	void ClearContextBar();

	void ResetUI();

public slots:
	void ScaleWindowAction();
	void ScaleCanvasAction();
	void ScaleOutputAction();
};
