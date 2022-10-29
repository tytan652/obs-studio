#include "basic-scenes.hpp"

#include "window-basic-main.hpp"

OBSBasicScenes::OBSBasicScenes(OBSBasic *main)
	: QWidget(nullptr), ui(new Ui::OBSBasicScenes)
{
	ui->setupUi(this);

	ui->scenes->setAttribute(Qt::WA_MacShowFocusRect, false);

	bool sceneGrid = config_get_bool(App()->GlobalConfig(), "BasicWindow",
					 "gridMode");
	ui->scenes->SetGridMode(sceneGrid);
	ui->scenes->setItemDelegate(new SceneRenameDelegate(ui->scenes));

	connect(ui->scenes, &SceneTree::currentItemChanged, main,
		&OBSBasic::ScenesCurrentItemChanged);
	connect(ui->scenes, &SceneTree::customContextMenuRequested, main,
		&OBSBasic::ScenesContextMenuRequested);
	connect(ui->scenes, &SceneTree::itemDoubleClicked, main,
		&OBSBasic::ScenesItemDoubleClicked);
	connect(ui->scenes, SIGNAL(scenesReordered()), main,
		SLOT(ScenesReordered()));

	connect(ui->actionAddScene, &QAction::triggered, main,
		&OBSBasic::AddSceneActionTriggered);
	connect(ui->actionRemoveScene, &QAction::triggered, main,
		&OBSBasic::RemoveSceneActionTriggered);
	connect(ui->actionSceneUp, &QAction::triggered, main,
		&OBSBasic::SceneUpActionTriggered);
	connect(ui->actionSceneDown, &QAction::triggered, main,
		&OBSBasic::SceneDownActionTriggered);

	connect(ui->scenes->itemDelegate(),
		SIGNAL(closeEditor(QWidget *,
				   QAbstractItemDelegate::EndEditHint)),
		main,
		SLOT(SceneNameEdited(QWidget *,
				     QAbstractItemDelegate::EndEditHint)));

	// Themes need the QAction dynamic properties
	for (QAction *x : ui->scenesToolbar->actions()) {
		QWidget *temp = ui->scenesToolbar->widgetForAction(x);

		for (QByteArray &y : x->dynamicPropertyNames()) {
			temp->setProperty(y, x->property(y));
		}
	}

#ifdef __APPLE__
	ui->actionRemoveScene->setShortcuts({Qt::Key_Backspace});
#endif
}
