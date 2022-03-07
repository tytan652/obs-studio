#include "window-basic-transitions.hpp"

#include "window-basic-main.hpp"

#include "qt-wrappers.hpp"

OBSBasicTransitions::OBSBasicTransitions(OBSBasic *main)
	: QWidget(nullptr), ui(new Ui::OBSBasicTransitions)
{
	ui->setupUi(this);

	connect(main, &OBSBasic::CurrentTransitionChanged, this,
		&OBSBasicTransitions::TransitionChanged);

	connect(ui->transitions, SIGNAL(currentIndexChanged(int)), main,
		SLOT(TransitionsCurrentIndexChanged(int)));

	connect(ui->transitionProps, &QPushButton::clicked, main,
		&OBSBasic::TransitionPropsClicked);
	connect(main, &OBSBasic::TransitionPropsEnabled, ui->transitionProps,
		&QPushButton::setEnabled);

	connect(ui->transitionDuration, SIGNAL(valueChanged(int)), main,
		SLOT(SetTransitionDuration(int)));
	connect(main, &OBSBasic::TransitionDurationChanged,
		ui->transitionDuration, &QSpinBox::setValue);
}

void OBSBasicTransitions::TransitionChanged(bool has_duration, bool has_config)
{
	ui->transitionDurationLabel->setVisible(has_duration);
	ui->transitionDuration->setVisible(has_duration);

	ui->transitionProps->setEnabled(has_config);
}
