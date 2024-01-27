#pragma once

#include "window-dock.hpp"
#include <QScopedPointer>

#include <obs-browser-api.hpp>

extern std::shared_ptr<OBSBrowserQCef> cef;
extern std::shared_ptr<OBSBrowserQCefCookieManager> panel_cookies;

class BrowserDock : public OBSDock {
private:
	QString title;

public:
	inline BrowserDock() : OBSDock() { setAttribute(Qt::WA_NativeWindow); }
	inline BrowserDock(const QString &title_) : OBSDock(title_)
	{
		title = title_;
		setAttribute(Qt::WA_NativeWindow);
	}

	QScopedPointer<OBSBrowserQCefWidget> cefWidget;

	inline void SetWidget(OBSBrowserQCefWidget *widget_)
	{
		setWidget(widget_->qwidget());
		cefWidget.reset(widget_);
	}

	inline void setTitle(const QString &title_) { title = title_; }

	void closeEvent(QCloseEvent *event) override;
	void showEvent(QShowEvent *event) override;
};
