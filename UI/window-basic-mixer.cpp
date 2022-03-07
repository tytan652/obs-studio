#include "window-basic-mixer.hpp"

#include "window-basic-main.hpp"

OBSBasicMixer::OBSBasicMixer(OBSBasic *main)
	: QWidget(nullptr), ui(new Ui::OBSBasicMixer)
{
	ui->setupUi(this);

	connect(ui->hMixerScrollArea, &QScrollArea::customContextMenuRequested,
		main, &OBSBasic::StackedMixerAreaContextMenuRequested);
	connect(ui->vMixerScrollArea, &QScrollArea::customContextMenuRequested,
		main, &OBSBasic::StackedMixerAreaContextMenuRequested);
}

void OBSBasicMixer::ToggleMixerLayout(bool vertical)
{
	verticalLayout = vertical;

	if (verticalLayout) {
		ui->stackedMixerArea->setMinimumSize(180, 220);
		ui->stackedMixerArea->setCurrentIndex(1);
	} else {
		ui->stackedMixerArea->setMinimumSize(220, 0);
		ui->stackedMixerArea->setCurrentIndex(0);
	}
}

void OBSBasicMixer::AddVolumeControl(VolControl *vol)
{
	if (verticalLayout)
		ui->vVolControlLayout->addWidget(vol);
	else
		ui->hVolControlLayout->addWidget(vol);
}
