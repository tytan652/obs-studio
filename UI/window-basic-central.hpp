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

	enum ContextBarSize {
		ContextBarSize_Minimized,
		ContextBarSize_Reduced,
		ContextBarSize_Normal
	};

	OBSBasic *main;

	std::unique_ptr<Ui::OBSBasicCentral> ui;

	ContextBarSize contextBarSize = ContextBarSize_Normal;

public:
	OBSBasicCentral(OBSBasic *main);
	inline ~OBSBasicCentral() {}

	void EnablePreviewDisplay(bool enable);

	void InitContextContainerVisibility(bool visible);

	obs_hotkey_pair_id CreateContextBarHotkeyPair();

	void ClearContextBar();
	void UpdateContextBarVisibility();

	void ResetUI();

public slots:
	void UpdateContextBar(bool force = false);

	void UpdateContextContainerVisibility(bool visible);
};
