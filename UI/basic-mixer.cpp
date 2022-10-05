#include "basic-mixer.hpp"

#include "window-basic-main.hpp"

#include "volume-control.hpp"

OBSBasicMixer::OBSBasicMixer(OBSBasic *main_)
	: QWidget(nullptr), main(main_), ui(new Ui::OBSBasicMixer)
{
	ui->setupUi(this);

	connect(ui->actionMixerToolbarAdvAudio, &QAction::triggered, main,
		&OBSBasic::on_actionAdvAudioProperties_triggered);

	connect(ui->hMixerScrollArea, &QScrollArea::customContextMenuRequested,
		main, &OBSBasic::StackedMixerAreaContextMenuRequested);
	connect(ui->vMixerScrollArea, &QScrollArea::customContextMenuRequested,
		main, &OBSBasic::StackedMixerAreaContextMenuRequested);

	// Themes need the QAction dynamic properties
	for (QAction *x : ui->mixerToolbar->actions()) {
		QWidget *temp = ui->mixerToolbar->widgetForAction(x);

		for (QByteArray &y : x->dynamicPropertyNames()) {
			temp->setProperty(y, x->property(y));
		}
	}
}

void OBSBasicMixer::ToggleMixerLayout(bool vertical_)
{
	vertical = vertical_;

	if (vertical) {
		ui->stackedMixerArea->setMinimumSize(180, 220);
		ui->stackedMixerArea->setCurrentIndex(1);
	} else {
		ui->stackedMixerArea->setMinimumSize(220, 0);
		ui->stackedMixerArea->setCurrentIndex(0);
	}
}

void OBSBasicMixer::AddVolumeControl(VolControl *vol)
{
	if (vertical)
		ui->vVolControlLayout->addWidget(vol);
	else
		ui->hVolControlLayout->addWidget(vol);
}

void OBSBasicMixer::on_actionMixerToolbarMenu_triggered()
{
	QAction unhideAllAction(QTStr("UnhideAll"), main);
	connect(&unhideAllAction, &QAction::triggered, main,
		&OBSBasic::UnhideAllAudioControls, Qt::DirectConnection);

	QAction toggleControlLayoutAction(QTStr("VerticalLayout"), main);
	toggleControlLayoutAction.setCheckable(true);
	toggleControlLayoutAction.setChecked(config_get_bool(
		GetGlobalConfig(), "BasicWindow", "VerticalVolControl"));
	connect(&toggleControlLayoutAction, &QAction::changed, main,
		&OBSBasic::ToggleVolControlLayout, Qt::DirectConnection);

	QMenu popup;
	popup.addAction(&unhideAllAction);
	popup.addSeparator();
	popup.addAction(&toggleControlLayoutAction);
	popup.exec(QCursor::pos());
}
