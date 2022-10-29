#include "basic-transitions.hpp"

#include "window-basic-main.hpp"

OBSBasicTransitions::OBSBasicTransitions(OBSBasic *main)
	: QWidget(nullptr), ui(new Ui::OBSBasicTransitions)
{
	ui->setupUi(this);

	connect(main, &OBSBasic::TransitionsWidgetsEnabled, this,
		&OBSBasicTransitions::TransitionsWidgetsEnabled);

	connect(main, &OBSBasic::TransitionAdded, this,
		&OBSBasicTransitions::TransitionAdded);

	connect(this, SIGNAL(ChangeTransition(const QString &)), main,
		SLOT(SetTransition(const QString &)));
	connect(main, &OBSBasic::TransitionChanged, this,
		&OBSBasicTransitions::TransitionChanged);

	connect(ui->transitionDuration, &QSpinBox::valueChanged, main,
		&OBSBasic::SetTransitionDuration);
	connect(main, &OBSBasic::TransitionDurationChanged, this,
		&OBSBasicTransitions::TransitionDurationChanged);

	connect(ui->transitionAdd, &QPushButton::clicked, main,
		&OBSBasic::AddTransitionMenu);
	connect(ui->transitionRemove, &QPushButton::clicked, main,
		&OBSBasic::RemoveTransition);
	connect(ui->transitionProps, &QPushButton::clicked, main,
		&OBSBasic::TransitionPropsAction);
}

void OBSBasicTransitions::on_transitions_currentIndexChanged(int)
{
	emit ChangeTransition(ui->transitions->currentText());
}

void OBSBasicTransitions::TransitionsWidgetsEnabled(bool enable)
{
	enabled = enable;
	ui->transitions->setEnabled(enabled);
	ui->transitionDuration->setEnabled(enabled);
	ui->transitionAdd->setEnabled(enabled);
	ui->transitionProps->setEnabled(enabled);
	ui->transitionRemove->setEnabled(enabled);
}

void OBSBasicTransitions::TransitionAdded(const QString &name)
{
	ui->transitions->addItem(name);
}

void OBSBasicTransitions::TransitionChanged(const QString &transition,
					    bool hasDuration,
					    bool hasProperties)
{
	QSignalBlocker blocker(ui->transitions);

	int idx = ui->transitions->findText(transition);

	if (idx == -1) {
		ui->transitions->addItem(transition);
		idx = ui->transitions->count() - 1;
	}
	ui->transitions->setCurrentIndex(idx);

	ui->transitionDurationLabel->setVisible(hasDuration);
	ui->transitionDuration->setVisible(hasDuration);

	ui->transitionProps->setVisible(hasProperties);
	ui->transitionRemove->setVisible(hasProperties);

	ui->transitionDuration->setEnabled(enabled);
	ui->transitionAdd->setEnabled(enabled);
	ui->transitionProps->setEnabled(enabled);
	ui->transitionRemove->setEnabled(enabled);
}

void OBSBasicTransitions::UnknownTransitionSet()
{
	QSignalBlocker blocker(ui->transitions);
	ui->transitions->setCurrentIndex(-1);

	ui->transitionDuration->setEnabled(false);
	ui->transitionAdd->setEnabled(false);
	ui->transitionProps->setEnabled(false);
	ui->transitionRemove->setEnabled(false);
}

void OBSBasicTransitions::TransitionDurationChanged(int value)
{
	QSignalBlocker blocker(ui->transitionDuration);

	ui->transitionDuration->setValue(value);
}

void OBSBasicTransitions::TransitionRemoved(const QString &name)
{
	int idx = ui->transitions->findText(name);
	ui->transitions->removeItem(idx);
}

void OBSBasicTransitions::TransitionRenamed(const QString &oldName,
					    const QString &newName)
{
	int idx = ui->transitions->findText(oldName);

	ui->transitions->setItemText(idx, newName);
}
