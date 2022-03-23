#include "window-dock-browser.hpp"
#include <DockContainerWidget.h>

void BrowserAdvDock::SetCefWidget(QCefWidget *widget_)
{
	if (widget())
		takeWidget();

	setWidget(widget_, ads::CDockWidget::ForceNoScrollArea);
	cefWidget.reset(widget_);

	connect(this, SIGNAL(closeRequested()), SLOT(CloseBrowser()));
}

void BrowserAdvDock::CloseBrowser()
{
	static int panel_version = -1;
	if (panel_version == -1) {
		panel_version = obs_browser_qcef_version();
	}

	if (panel_version >= 2 && !!cefWidget) {
		cefWidget->closeBrowser();
	}
}
