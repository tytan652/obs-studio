#pragma once

#include <memory>

#include <QWidget>
#include <QPointer>

class OBSBasic;
class RecordButton;

#include "ui_OBSBasicControls.h"

class OBSBasicControls : public QWidget {
	Q_OBJECT

	// XXX: OBSBasic still require access to buttons state
	friend class OBSBasic;

	OBSBasic *main;
	std::unique_ptr<Ui::OBSBasicControls> ui;

	QPointer<QMenu> streamButtonMenu;

	QScopedPointer<QPushButton> pauseButton;

	QPointer<QHBoxLayout> replayBufferLayout;
	QPointer<RecordButton> replayBufferButton;
	QScopedPointer<QPushButton> saveReplayButton;

	QPointer<QPushButton> vcamButton;

public:
	OBSBasicControls(OBSBasic *main);
	inline ~OBSBasicControls() {}

private:
	void SetupReplayBufferButton(bool remove);

private slots:
	void ResizePauseButton();
	void ResizeSaveReplayButton();

	void SetStreamingStoppedState(bool add_menu);
	void SetStartingStreamingState(bool broadcast_auto_start);
	void SetStoppingStreamingState();
	void SetStreamingStartedState(bool add_menu);

	void SetBroadcastStartedState(bool auto_stop);
	void SetBroadcastFlowEnabled(bool enabled, bool ready);

	void SetRecordingStoppedState();
	void SetStoppingRecordingState();
	void SetRecordingStartedState(bool add_pause_button);

	void PauseToUnpauseButton();
	void UnpauseToPauseButton();

	void SetReplayBufferStartedState();
	void SetReplayBufferStoppingState();
	void SetReplayBufferStoppedState();

	void EnableVCamButton();
	void SetVCamStoppedState();
	void SetVCamStartedState();
};
