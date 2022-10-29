#pragma once

#include "window-dock.hpp"
#include <QScopedPointer>

#include <browser-panel.hpp>
extern QCef *cef;
extern QCefCookieManager *panel_cookies;

class BrowserAdvDock : public OBSAdvDock {
	Q_OBJECT

protected:
	QScopedPointer<QCefWidget> cefWidget;

public:
	inline BrowserAdvDock(const QString &window_title, const QString &title)
		: OBSAdvDock(window_title, title)
	{
		setAttribute(Qt::WA_NativeWindow);
	}

	void SetCefWidget(QCefWidget *widget);
	inline QCefWidget *CefWidget() const { return cefWidget.data(); }

public slots:
	void CloseBrowser();
};
