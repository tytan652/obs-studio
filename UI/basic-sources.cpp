#include "basic-sources.hpp"

#include "window-basic-main.hpp"

OBSBasicSources::OBSBasicSources(OBSBasic *main)
	: QWidget(nullptr), ui(new Ui::OBSBasicSources)
{
	ui->setupUi(this);

	ui->sources->setAttribute(Qt::WA_MacShowFocusRect, false);

	connect(ui->sources, &SourceTree::customContextMenuRequested, main,
		&OBSBasic::SourcesContextMenuRequested);

	connect(ui->actionAddSource, &QAction::triggered, main,
		&OBSBasic::AddSourceActionTriggered);
	connect(ui->actionRemoveSource, &QAction::triggered, main,
		&OBSBasic::RemoveSourceActionTriggered);
	connect(ui->actionSourceProperties, &QAction::triggered, main,
		&OBSBasic::SourcePropertiesActionTriggered);
	connect(ui->actionSourceUp, &QAction::triggered, main,
		&OBSBasic::SourceUpActionTriggered);
	connect(ui->actionSourceDown, &QAction::triggered, main,
		&OBSBasic::SourceDownActionTriggered);

	// Themes need the QAction dynamic properties
	for (QAction *x : ui->sourcesToolbar->actions()) {
		QWidget *temp = ui->sourcesToolbar->widgetForAction(x);

		for (QByteArray &y : x->dynamicPropertyNames()) {
			temp->setProperty(y, x->property(y));
		}
	}

#ifdef __APPLE__
	ui->actionRemoveSource->setShortcuts({Qt::Key_Backspace});
#endif
}
