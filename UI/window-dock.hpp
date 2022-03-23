#pragma once

#include <QDockWidget>
#include <DockWidget.h>

class OBSDock : public QDockWidget {
	Q_OBJECT

public:
	inline OBSDock(QWidget *parent = nullptr) : QDockWidget(parent) {}

	virtual void closeEvent(QCloseEvent *event);
};

class OBSAdvDock : public ads::CDockWidget {
	Q_OBJECT

	int defaultWidth = 0;
	int defaultHeight = 0;
	bool useAsRatio = false;

	bool showCloseMessage = true;

	bool resetPosNextOpen = false;

	// Used by any constructor to setup common connections
	void SetupConnections();

public:
	OBSAdvDock(const QString &window_title, const QString &title,
		   QWidget *widget);

	/* Size of the when ResetPos will be triggered
	 * If ratio is true width and height are multiplied
	 * by respectively main window width and height */
	inline void SetDefaultSize(int width, int height, bool ratio = false)
	{
		defaultWidth = width;
		defaultHeight = height;
		useAsRatio = ratio;
	}

	inline void ResetPosOnNextOpen(bool reset = true)
	{
		resetPosNextOpen = reset;
	}

protected:
	/* NOTE: This constructor is meant to be only used by BrowserAdvDock
	 * to allow to add the CEF widget afterward */
	OBSAdvDock(const QString &window_title, const QString &title);

public slots:
	// Allow closing the dock without showing the warning message
	void ToggleViewNoWarning(bool open);

private slots:
	// Show a warning when the dock get closed (if showCloseMessage is true)
	void ShowCloseWarning();

	// Reset dock position
	void ResetPos(bool open);

	// Setup connections with the dock manager
	void SetupWithDockManager();
};
