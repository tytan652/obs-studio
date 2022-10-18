#include "window-dock.hpp"
#include "obs-app.hpp"
#include "window-basic-main.hpp"

#include <QAction>
#include <QMessageBox>
#include <QCheckBox>

#include <FloatingDockContainer.h>

void OBSDock::closeEvent(QCloseEvent *event)
{
	auto msgBox = []() {
		QMessageBox msgbox(App()->GetMainWindow());
		msgbox.setWindowTitle(QTStr("DockCloseWarning.Title"));
		msgbox.setText(QTStr("DockCloseWarning.Text"));
		msgbox.setIcon(QMessageBox::Icon::Information);
		msgbox.addButton(QMessageBox::Ok);

		QCheckBox *cb = new QCheckBox(QTStr("DoNotShowAgain"));
		msgbox.setCheckBox(cb);

		msgbox.exec();

		if (cb->isChecked()) {
			config_set_bool(App()->GlobalConfig(), "General",
					"WarnedAboutClosingDocks", true);
			config_save_safe(App()->GlobalConfig(), "tmp", nullptr);
		}
	};

	bool warned = config_get_bool(App()->GlobalConfig(), "General",
				      "WarnedAboutClosingDocks");
	if (!warned) {
		QMetaObject::invokeMethod(App(), "Exec", Qt::QueuedConnection,
					  Q_ARG(VoidFunc, msgBox));
	}

	QDockWidget::closeEvent(event);
}

OBSAdvDock::OBSAdvDock(const QString &window_title, const QString &title,
		       QWidget *widget)
	: ads::CDockWidget(title)
{
	setWindowTitle(window_title);
	setWidget(widget, ads::CDockWidget::ForceNoScrollArea);
	setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromContent);
	setSizePolicy(widget->sizePolicy());

	SetupConnections();
}

OBSAdvDock::OBSAdvDock(const QString &window_title, const QString &title)
	: ads::CDockWidget(title)
{
	setWindowTitle(window_title);
	setMinimumSizeHintMode(ads::CDockWidget::MinimumSizeHintFromContent);

	SetupConnections();
}

void OBSAdvDock::SetupConnections()
{
	// Show a warning message when the dock is closed
	connect(this, &ads::CDockWidget::closed, this,
		&OBSAdvDock::ShowCloseWarning);

	// Avoid showing the warning message if closing the dock from the dock menu
	disconnect(toggleViewAction(), &QAction::triggered, this,
		   &ads::CDockWidget::toggleView);
	connect(toggleViewAction(), &QAction::triggered, this,
		&OBSAdvDock::ToggleViewNoWarning);

	// Allow the dock having his position and size reset when needed
	connect(this, &ads::CDockWidget::viewToggled, this,
		&OBSAdvDock::ResetPos);

	/* Since the dock manager toggle the dock once added
	 * connections are setup with the dock manager when it happen */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	connect(this, &ads::CDockWidget::viewToggled, this,
		&OBSAdvDock::SetupWithDockManager);
#else
	connect(this, &ads::CDockWidget::viewToggled, this,
		&OBSAdvDock::SetupWithDockManager,
		(Qt::ConnectionType)(Qt::AutoConnection |
				     Qt::SingleShotConnection));
#endif
}

void OBSAdvDock::ToggleViewNoWarning(bool open)
{
	showCloseMessage = false;
	toggleView(open);
	showCloseMessage = true;
}

void OBSAdvDock::ShowCloseWarning()
{
	if (!showCloseMessage)
		return;

	auto msgBox = []() {
		QMessageBox msgbox(App()->GetMainWindow());
		msgbox.setWindowTitle(QTStr("DockCloseWarning.Title"));
		msgbox.setText(QTStr("DockCloseWarning.Text"));
		msgbox.setIcon(QMessageBox::Icon::Information);
		msgbox.addButton(QMessageBox::Ok);

		QCheckBox *cb = new QCheckBox(QTStr("DoNotShowAgain"));
		msgbox.setCheckBox(cb);

		msgbox.exec();

		if (cb->isChecked()) {
			config_set_bool(App()->GlobalConfig(), "General",
					"WarnedAboutClosingDocks", true);
			config_save_safe(App()->GlobalConfig(), "tmp", nullptr);
		}
	};

	bool warned = config_get_bool(App()->GlobalConfig(), "General",
				      "WarnedAboutClosingDocks");
	if (!warned) {
		QMetaObject::invokeMethod(App(), "Exec", Qt::QueuedConnection,
					  Q_ARG(VoidFunc, msgBox));
	}
}

void OBSAdvDock::ResetPos(bool open)
{
	/* To allow the dock position and size to be reset:
	 *  - `resetPosNextOpen` shall be true
	 *  - `open shall` be true, the dock is being opened
	 *  - `isFloating()` shall return true,
	 *    it indicates that the dock is floating
	 *    and is alone in its container */
	if (!(resetPosNextOpen && open && isFloating()))
		return;

	// NOTE: To reposition the dock, we should reposition its floating container instead
	ads::CFloatingDockContainer *container =
		dockContainer()->floatingWidget();
	OBSBasic *main = OBSBasic::Get();

	// Resize the container if default width and height were set
	if (defaultWidth || defaultHeight) {
		if (useAsRatio)
			container->resize(main->width() * defaultWidth,
					  main->height() * defaultHeight);
		else
			container->resize(defaultWidth, defaultHeight);
	}

	QPoint mainSize(main->width(), main->height());

	QPoint dockSize(container->width(), container->height());
	QPoint dockPos = mainSize / 2 - dockSize / 2;
	QPoint newPos = main->pos() + dockPos;
	container->move(newPos);

	resetPosNextOpen = false;
}

void OBSAdvDock::SetupWithDockManager()
{
	ads::CDockManager *manager = dockManager();
	if (!manager)
		return;

	// Avoid close message when restoring dock state
	connect(manager, &ads::CDockManager::restoringState, this,
		[this]() { this->showCloseMessage = false; });
	connect(manager, &ads::CDockManager::stateRestored, this,
		[this]() { this->showCloseMessage = true; });

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	// Disconnect this slot since it has done its job
	disconnect(this, &ads::CDockWidget::viewToggled, this,
		   &OBSAdvDock::SetupWithDockManager);
#endif
}
