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

	int previewX = 0, previewY = 0;
	int previewCX = 0, previewCY = 0;
	float previewScale = 0.0f;

	int programX = 0, programY = 0;
	int programCX = 0, programCY = 0;
	float programScale = 0.0f;
	QPointer<OBSQTDisplay> program;

	QPointer<QWidget> programWidget;
	QPointer<QVBoxLayout> programLayout;
	QPointer<QLabel> programLabel;

	ContextBarSize contextBarSize = ContextBarSize_Normal;

public:
	OBSBasicCentral(OBSBasic *main);
	inline ~OBSBasicCentral() {}

private:
	void ResizePreview(uint32_t cx, uint32_t cy);
	void ResizeProgram(uint32_t cx, uint32_t cy);

	/* OBS Callbacks */
	static void RenderProgram(void *data, uint32_t cx, uint32_t cy);
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);

public:
	void OBSInit();

	void RemovePreviewDrawCallback();

	void CreateProgramDisplay();

	void AddProgramDisplay(QWidget *programOptions);
	void RemoveProgramDisplay();

	void ResizePreviewProgram(obs_video_info ovi);

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
