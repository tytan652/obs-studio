#include "window-basic-controls.hpp"

#include "window-basic-main.hpp"

OBSBasicControls::OBSBasicControls(OBSBasic *main_)
	: QWidget(nullptr), main(main_), ui(new Ui::OBSBasicControls)
{
	ui->setupUi(this);

	// Stream button setup
	connect(ui->streamButton, &QPushButton::clicked, main,
		&OBSBasic::StreamActionTriggered);

	// Broadcast button setup
	ui->broadcastButton->setVisible(false);
	connect(ui->broadcastButton, &QPushButton::clicked, main,
		&OBSBasic::BroadcastButtonClicked);
	connect(main, &OBSBasic::BroadcastReady, this,
		[this]() { this->ui->broadcastButton->setChecked(true); });
	connect(main, &OBSBasic::BroadcastStartAborted, this,
		[this]() { this->ui->broadcastButton->setChecked(false); });
	connect(main, &OBSBasic::BroadcastStopAborted, this,
		[this]() { this->ui->broadcastButton->setChecked(true); });
	connect(main, &OBSBasic::BroadcastActionEnabled, this,
		[this]() { this->ui->broadcastButton->setEnabled(true); });

	// Record button setup
	connect(ui->recordButton, &RecordButton::ResizeEvent, this,
		&OBSBasicControls::ResizePauseButton);
	connect(ui->recordButton, &QPushButton::clicked, main,
		&OBSBasic::RecordActionTriggered);

	// Mode switch button setup
	connect(ui->modeSwitch, &QPushButton::clicked, main,
		&OBSBasic::ModeSwitchActionTriggered);
	connect(main, &OBSBasic::PreviewProgramModeChanged, ui->modeSwitch,
		&QPushButton::setChecked);

	// Settings button setup
	connect(ui->settingsButton, &QPushButton::clicked, main,
		&OBSBasic::on_action_Settings_triggered);

	// Exit button setup
	connect(ui->exitButton, &QPushButton::clicked, main, &OBSBasic::close);

	// Setup streaming connections
	connect(main, &OBSBasic::StreamingStarting, this,
		&OBSBasicControls::SetStartingStreamingState);
	connect(main, &OBSBasic::StreamingStartAborted, this,
		[this]() { this->ui->streamButton->setChecked(false); });
	connect(main, &OBSBasic::StreamingStarted, this,
		&OBSBasicControls::SetStreamingStartedState);
	connect(main, &OBSBasic::StreamingStopping, this,
		&OBSBasicControls::SetStoppingStreamingState);
	connect(main, &OBSBasic::StreamingStopAborted, this,
		[this]() { this->ui->streamButton->setChecked(true); });
	connect(main, &OBSBasic::StreamingStopped, this,
		&OBSBasicControls::SetStreamingStoppedState);

	// Setup broadcast connection
	connect(main, &OBSBasic::BroadcastStreamStarted, this,
		&OBSBasicControls::SetBroadcastStartedState);
	connect(main, &OBSBasic::BroadcastFlowStateChanged, this,
		&OBSBasicControls::SetBroadcastFlowEnabled);

	// Setup recording connections
	connect(main, &OBSBasic::RecordingStartAborted, this,
		[this]() { this->ui->recordButton->setChecked(false); });
	connect(main, &OBSBasic::RecordingStarted, this,
		&OBSBasicControls::SetRecordingStartedState);
	connect(main, &OBSBasic::RecordingStopping, this,
		&OBSBasicControls::SetStoppingRecordingState);
	connect(main, &OBSBasic::RecordingStopAborted, this,
		[this]() { this->ui->recordButton->setChecked(true); });
	connect(main, &OBSBasic::RecordingStopped, this,
		&OBSBasicControls::SetRecordingStoppedState);

	// Setup pause/unpause recording connections
	connect(main, &OBSBasic::RecordingPaused, this,
		&OBSBasicControls::PauseToUnpauseButton);
	connect(main, &OBSBasic::RecordingUnpaused, this,
		&OBSBasicControls::UnpauseToPauseButton);

	// Setup replay buffer connection
	connect(main, &OBSBasic::ReplayBufferEnabled, this,
		[this]() { this->SetupReplayBufferButton(false); });
	connect(main, &OBSBasic::ReplayBufferDisabled, this,
		[this]() { this->SetupReplayBufferButton(true); });
	connect(main, &OBSBasic::ReplayBufferStarted, this,
		&OBSBasicControls::SetReplayBufferStartedState);
	connect(main, &OBSBasic::ReplayBufferStopping2, this,
		&OBSBasicControls::SetReplayBufferStoppingState);
	connect(main, &OBSBasic::ReplayBufferStopped, this,
		&OBSBasicControls::SetReplayBufferStoppedState);

	// Setup virtual camera connections
	connect(main, &OBSBasic::VirtualCamEnabled, this,
		&OBSBasicControls::EnableVCamButton);
	connect(main, &OBSBasic::VirtualCamStarted, this,
		&OBSBasicControls::SetVCamStartedState);
	connect(main, &OBSBasic::VirtualCamStopped, this,
		&OBSBasicControls::SetVCamStoppedState);
}

void OBSBasicControls::ResizePauseButton()
{
	if (!pauseButton)
		return;

	QSize pauseSize = pauseButton->size();
	int height = ui->recordButton->size().height();

	if (pauseSize.height() != height || pauseSize.width() != height) {
		pauseButton->setMinimumSize(height, height);
		pauseButton->setMaximumSize(height, height);
	}
}

void OBSBasicControls::ResizeSaveReplayButton()
{
	if (!saveReplayButton || !replayBufferButton)
		return;

	QSize replaySize = saveReplayButton->size();
	int height = replayBufferButton->size().height();

	if (replaySize.height() != height || replaySize.width() != height) {
		saveReplayButton->setMinimumSize(height, height);
		saveReplayButton->setMaximumSize(height, height);
	}
}

void OBSBasicControls::SetStreamingStoppedState(bool add_menu)
{
	ui->streamButton->setText(QTStr("Basic.Main.StartStreaming"));
	ui->streamButton->setEnabled(true);
	ui->streamButton->setChecked(false);

	if (!streamButtonMenu.isNull()) {
		if (!add_menu)
			ui->streamButton->setMenu(nullptr);
		streamButtonMenu->deleteLater();
		if (!add_menu)
			streamButtonMenu = nullptr;
	}

	if (!add_menu)
		return;

	streamButtonMenu = new QMenu();
	streamButtonMenu->addAction(QTStr("Basic.Main.StartStreaming"), main,
				    SLOT(StartStreaming()));
	streamButtonMenu->addAction(QTStr("Basic.Main.ForceStopStreaming"),
				    main, SLOT(ForceStopStreaming()));
	ui->streamButton->setMenu(streamButtonMenu);
}

void OBSBasicControls::SetStoppingStreamingState()
{
	ui->streamButton->setText(QTStr("Basic.Main.StoppingStreaming"));
}

void OBSBasicControls::SetStartingStreamingState(bool broadcast_auto_start)
{
	ui->streamButton->setText(QTStr("Basic.Main.Connecting"));
	ui->streamButton->setEnabled(false);
	ui->streamButton->setChecked(false);

	ui->broadcastButton->setChecked(false);

	if (broadcast_auto_start)
		return;

	ui->broadcastButton->setText(QTStr("Basic.Main.StartBroadcast"));
	ui->broadcastButton->setProperty("broadcastState", "ready");
	ui->broadcastButton->style()->unpolish(ui->broadcastButton);
	ui->broadcastButton->style()->polish(ui->broadcastButton);
	// well, we need to disable button while stream is not active
	ui->broadcastButton->setEnabled(false);
}

void OBSBasicControls::SetStreamingStartedState(bool add_menu)
{
	ui->streamButton->setText(QTStr("Basic.Main.StopStreaming"));
	ui->streamButton->setEnabled(true);
	ui->streamButton->setChecked(true);

	if (!add_menu)
		return;

	if (!streamButtonMenu.isNull())
		streamButtonMenu->deleteLater();

	streamButtonMenu = new QMenu();
	streamButtonMenu->addAction(QTStr("Basic.Main.StopStreaming"), main,
				    SLOT(StopStreaming()));
	streamButtonMenu->addAction(QTStr("Basic.Main.ForceStopStreaming"),
				    main, SLOT(ForceStopStreaming()));
	ui->streamButton->setMenu(streamButtonMenu);
}

void OBSBasicControls::SetBroadcastStartedState(bool auto_stop)
{
	if (!auto_stop)
		ui->broadcastButton->setText(QTStr("Basic.Main.StopBroadcast"));
	else {
		ui->broadcastButton->setText(
			QTStr("Basic.Main.AutoStopEnabled"));
		ui->broadcastButton->setEnabled(false);
	}

	ui->broadcastButton->setProperty("broadcastState", "active");
	ui->broadcastButton->style()->unpolish(ui->broadcastButton);
	ui->broadcastButton->style()->polish(ui->broadcastButton);
}

void OBSBasicControls::SetBroadcastFlowEnabled(bool enabled, bool ready)
{
	ui->broadcastButton->setEnabled(enabled);
	ui->broadcastButton->setVisible(enabled);
	ui->broadcastButton->setChecked(ready);
	ui->broadcastButton->setProperty("broadcastState", "idle");
	ui->broadcastButton->style()->unpolish(ui->broadcastButton);
	ui->broadcastButton->style()->polish(ui->broadcastButton);
	ui->broadcastButton->setText(QTStr("Basic.Main.SetupBroadcast"));
}

void OBSBasicControls::SetRecordingStoppedState()
{
	ui->recordButton->setText(QTStr("Basic.Main.StartRecording"));
	ui->recordButton->setChecked(false);

	pauseButton.reset();
}

void OBSBasicControls::SetStoppingRecordingState()
{
	ui->recordButton->setText(QTStr("Basic.Main.StoppingRecording"));
}

void OBSBasicControls::SetRecordingStartedState(bool add_pause_button)
{
	ui->recordButton->setText(QTStr("Basic.Main.StopRecording"));
	ui->recordButton->setChecked(true);

	if (!add_pause_button)
		return;

	pauseButton.reset(new QPushButton());
	pauseButton->setAccessibleName(QTStr("Basic.Main.PauseRecording"));
	pauseButton->setToolTip(QTStr("Basic.Main.PauseRecording"));
	pauseButton->setCheckable(true);
	pauseButton->setChecked(false);
	pauseButton->setProperty("themeID",
				 QVariant(QStringLiteral("pauseIconSmall")));

	QSizePolicy sp;
	sp.setHeightForWidth(true);
	pauseButton->setSizePolicy(sp);

	connect(pauseButton.data(), &QPushButton::clicked, main,
		&OBSBasic::PauseToggled);

	ui->recordingLayout->addWidget(pauseButton.data());
}

void OBSBasicControls::PauseToUnpauseButton()
{
	if (!pauseButton)
		return;

	pauseButton->setAccessibleName(QTStr("Basic.Main.UnpauseRecording"));
	pauseButton->setToolTip(QTStr("Basic.Main.UnpauseRecording"));
	pauseButton->blockSignals(true);
	pauseButton->setChecked(true);
	pauseButton->blockSignals(false);

	if (saveReplayButton)
		saveReplayButton->setEnabled(false);
}

void OBSBasicControls::UnpauseToPauseButton()
{
	if (!pauseButton)
		return;

	pauseButton->setAccessibleName(QTStr("Basic.Main.PauseRecording"));
	pauseButton->setToolTip(QTStr("Basic.Main.PauseRecording"));
	pauseButton->blockSignals(true);
	pauseButton->setChecked(false);
	pauseButton->blockSignals(false);

	if (saveReplayButton)
		saveReplayButton->setEnabled(true);
}

void OBSBasicControls::SetupReplayBufferButton(bool remove)
{
	delete replayBufferButton;
	delete replayBufferLayout;
	disconnect(main, &OBSBasic::ReplayBufferStartAborted, this, nullptr);

	if (remove)
		return;

	replayBufferButton =
		new RecordButton(QTStr("Basic.Main.StartReplayBuffer"), this);
	replayBufferButton->setCheckable(true);

	replayBufferButton->setSizePolicy(QSizePolicy::Ignored,
					  QSizePolicy::Fixed);

	replayBufferLayout = new QHBoxLayout(this);
	replayBufferLayout->addWidget(replayBufferButton);

	replayBufferLayout->setProperty("themeID", "replayBufferButton");

	ui->buttonsVLayout->insertLayout(2, replayBufferLayout);
	setTabOrder(ui->recordButton, replayBufferButton);
	setTabOrder(replayBufferButton,
		    ui->buttonsVLayout->itemAt(3)->widget());

	connect(replayBufferButton.data(), &RecordButton::ResizeEvent, this,
		&OBSBasicControls::ResizeSaveReplayButton);
	connect(replayBufferButton.data(), &QPushButton::clicked, main,
		&OBSBasic::ReplayBufferClicked);
	connect(main, &OBSBasic::ReplayBufferStartAborted, this,
		[this]() { this->replayBufferButton->setChecked(false); });
}

void OBSBasicControls::SetReplayBufferStartedState()
{
	if (!replayBufferButton || !replayBufferLayout)
		return;

	replayBufferButton->setText(QTStr("Basic.Main.StopReplayBuffer"));
	replayBufferButton->setChecked(true);

	saveReplayButton.reset(new QPushButton());
	saveReplayButton->setAccessibleName(QTStr("Basic.Main.SaveReplay"));
	saveReplayButton->setToolTip(QTStr("Basic.Main.SaveReplay"));
	saveReplayButton->setChecked(false);
	saveReplayButton->setProperty(
		"themeID", QVariant(QStringLiteral("replayIconSmall")));

	QSizePolicy sp;
	sp.setHeightForWidth(true);
	saveReplayButton->setSizePolicy(sp);

	replayBufferLayout->addWidget(saveReplayButton.data());
	setTabOrder(replayBufferLayout->itemAt(0)->widget(),
		    replayBufferLayout->itemAt(0)->widget());
	setTabOrder(replayBufferLayout->itemAt(1)->widget(),
		    ui->buttonsVLayout->itemAt(3)->widget());

	connect(saveReplayButton.data(), &QAbstractButton::clicked, main,
		&OBSBasic::ReplayBufferSave);
}

void OBSBasicControls::SetReplayBufferStoppingState()
{
	if (!replayBufferButton)
		return;

	replayBufferButton->setText(QTStr("Basic.Main.StoppingReplayBuffer"));
}

void OBSBasicControls::SetReplayBufferStoppedState()
{
	if (!replayBufferButton)
		return;

	replayBufferButton->setText(QTStr("Basic.Main.StartReplayBuffer"));
	replayBufferButton->setChecked(false);

	saveReplayButton.reset();
}

void OBSBasicControls::EnableVCamButton()
{
	vcamButton = new QPushButton(QTStr("Basic.Main.StartVirtualCam"), this);
	vcamButton->setCheckable(true);

	vcamButton->setProperty("themeID", "vcamButton");
	ui->buttonsVLayout->insertWidget(2, vcamButton);
	setTabOrder(ui->recordButton, vcamButton);
	setTabOrder(vcamButton, ui->modeSwitch);

	connect(vcamButton.data(), &QPushButton::clicked, main,
		&OBSBasic::VCamButtonClicked);
	connect(main, &OBSBasic::VirtualCamStartAborted, this,
		[this]() { vcamButton->setChecked(false); });
}

void OBSBasicControls::SetVCamStoppedState()
{
	if (!vcamButton)
		return;

	vcamButton->setText(QTStr("Basic.Main.StartVirtualCam"));
	vcamButton->setChecked(false);
}

void OBSBasicControls::SetVCamStartedState()
{
	if (!vcamButton)
		return;

	vcamButton->setText(QTStr("Basic.Main.StopVirtualCam"));
	vcamButton->setChecked(true);
}
