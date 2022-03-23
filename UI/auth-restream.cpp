#include "auth-restream.hpp"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qt-wrappers.hpp>
#include <json11.hpp>
#include <ctime>
#include <sstream>

#include <obs-app.hpp>
#include "window-dock-browser.hpp"
#include "window-basic-main.hpp"
#include "remote-text.hpp"
#include "ui-config.h"
#include "obf.h"

using namespace json11;

/* ------------------------------------------------------------------------- */

#define RESTREAM_AUTH_URL \
	"https://obsproject.com/app-auth/restream?action=redirect"
#define RESTREAM_TOKEN_URL "https://obsproject.com/app-auth/restream-token"
#define RESTREAM_STREAMKEY_URL "https://api.restream.io/v2/user/streamKey"
#define RESTREAM_SCOPE_VERSION 1

#define RESTREAM_CHAT_DOCK_NAME "obs-restream_chat"
#define RESTREAM_INFO_DOCK_NAME "obs-restream_info"
#define RESTREAM_CHANNELS_DOCK_NAME "obs-restream_channels"

static Auth::Def restreamDef = {"Restream", Auth::Type::OAuth_StreamKey};

/* ------------------------------------------------------------------------- */

RestreamAuth::RestreamAuth(const Def &d) : OAuthStreamKey(d) {}

RestreamAuth::~RestreamAuth()
{
	OBSBasic *main = OBSBasic::Get();

	main->RemoveAdvDockWidget(RESTREAM_CHAT_DOCK_NAME);
	main->RemoveAdvDockWidget(RESTREAM_INFO_DOCK_NAME);
	main->RemoveAdvDockWidget(RESTREAM_CHANNELS_DOCK_NAME);
}

bool RestreamAuth::GetChannelInfo()
try {
	std::string client_id = RESTREAM_CLIENTID;
	deobfuscate_str(&client_id[0], RESTREAM_HASH);

	if (!GetToken(RESTREAM_TOKEN_URL, client_id, RESTREAM_SCOPE_VERSION))
		return false;
	if (token.empty())
		return false;
	if (!key_.empty())
		return true;

	std::string auth;
	auth += "Authorization: Bearer ";
	auth += token;

	std::vector<std::string> headers;
	headers.push_back(std::string("Client-ID: ") + client_id);
	headers.push_back(std::move(auth));

	std::string output;
	std::string error;
	Json json;
	bool success;

	auto func = [&]() {
		success = GetRemoteFile(RESTREAM_STREAMKEY_URL, output, error,
					nullptr, "application/json", "",
					nullptr, headers, nullptr, 5);
	};

	ExecThreadedWithoutBlocking(
		func, QTStr("Auth.LoadingChannel.Title"),
		QTStr("Auth.LoadingChannel.Text").arg(service()));
	if (!success || output.empty())
		throw ErrorInfo("Failed to get stream key from remote", error);

	json = Json::parse(output, error);
	if (!error.empty())
		throw ErrorInfo("Failed to parse json", error);

	error = json["error"].string_value();
	if (!error.empty())
		throw ErrorInfo(error,
				json["error_description"].string_value());

	key_ = json["streamKey"].string_value();

	return true;
} catch (ErrorInfo info) {
	QString title = QTStr("Auth.ChannelFailure.Title");
	QString text = QTStr("Auth.ChannelFailure.Text")
			       .arg(service(), info.message.c_str(),
				    info.error.c_str());

	QMessageBox::warning(OBSBasic::Get(), title, text);

	blog(LOG_WARNING, "%s: %s: %s", __FUNCTION__, info.message.c_str(),
	     info.error.c_str());
	return false;
}

void RestreamAuth::SaveInternal()
{
	OBSBasic *main = OBSBasic::Get();
	config_set_string(main->Config(), service(), "AdvDockState",
			  main->AdvDockState().toBase64().constData());
	OAuthStreamKey::SaveInternal();
}

static inline std::string get_config_str(OBSBasic *main, const char *section,
					 const char *name)
{
	const char *val = config_get_string(main->Config(), section, name);
	return val ? val : "";
}

bool RestreamAuth::LoadInternal()
{
	firstLoad = false;
	return OAuthStreamKey::LoadInternal();
}

void RestreamAuth::LoadUI()
{
	if (uiLoaded)
		return;
	if (!GetChannelInfo())
		return;

	OBSBasic::InitBrowserPanelSafeBlock();
	OBSBasic *main = OBSBasic::Get();

	QCefWidget *browser;
	std::string url;
	std::string script;

	/* ----------------------------------- */

	url = "https://restream.io/chat-application";

	BrowserAdvDock *chat =
		new BrowserAdvDock(QTStr("Auth.Chat"), RESTREAM_CHAT_DOCK_NAME);
	chat->SetDefaultSize(420, 600);
	chat->setMinimumSize(200, 300);

	browser = cef->create_widget(chat, url, panel_cookies);
	chat->SetCefWidget(browser);

	main->AddAdvDockWidget(chat);

	/* ----------------------------------- */

	url = "https://restream.io/titles/embed";

	BrowserAdvDock *info = new BrowserAdvDock(QTStr("Auth.StreamInfo"),
						  RESTREAM_INFO_DOCK_NAME);
	info->SetDefaultSize(410, 600);
	info->setMinimumSize(200, 150);

	browser = cef->create_widget(info, url, panel_cookies);
	info->SetCefWidget(browser);

	main->AddAdvDockWidget(info);

	/* ----------------------------------- */

	url = "https://restream.io/channel/embed";

	BrowserAdvDock *channels = new BrowserAdvDock(
		QTStr("RestreamAuth.Channels"), RESTREAM_CHANNELS_DOCK_NAME);
	channels->SetDefaultSize(410, 600);
	channels->setMinimumSize(410, 300);

	browser = cef->create_widget(channels, url, panel_cookies);
	channels->SetCefWidget(browser);

	main->AddAdvDockWidget(channels);

	/* ----------------------------------- */

	if (firstLoad) {
		QSize size = main->frameSize();
		QPoint pos = main->pos();

		ads::CFloatingDockContainer *container =
			chat->dockContainer()->floatingWidget();
		container->resize(420, 600);
		container->move(pos.x() + size.width() - container->width() -
					30,
				pos.y() + 60);

		container = info->dockContainer()->floatingWidget();
		container->resize(410, 600);
		container->move(pos.x() + 20, pos.y() + 60);

		int infoWidth = container->width();

		container = channels->dockContainer()->floatingWidget();
		container->resize(410, 600);
		container->move(pos.x() + 20 + infoWidth + 10, pos.y() + 60);

		chat->toggleView(true);
		info->toggleView(true);
		channels->toggleView(true);
	} else {
		const char *advDockStateStr = config_get_string(
			main->Config(), service(), "AdvDockState");

		if (!advDockStateStr) {
			/* Use deprecated "DockState" value if available */
			const char *dockStateStr = config_get_string(
				main->Config(), service(), "DockState");

			if (dockStateStr) {
				QByteArray dockState = QByteArray::fromBase64(
					QByteArray(dockStateStr));
				main->restoreState(dockState);
			}
		} else {
			QByteArray state = QByteArray::fromBase64(
				QByteArray(advDockStateStr));
			main->RestoreAdvDocksState(state);
		}
	}

	uiLoaded = true;
}

bool RestreamAuth::RetryLogin()
{
	OAuthLogin login(OBSBasic::Get(), RESTREAM_AUTH_URL, false);
	cef->add_popup_whitelist_url("about:blank", &login);
	if (login.exec() == QDialog::Rejected) {
		return false;
	}

	std::shared_ptr<RestreamAuth> auth =
		std::make_shared<RestreamAuth>(restreamDef);

	std::string client_id = RESTREAM_CLIENTID;
	deobfuscate_str(&client_id[0], RESTREAM_HASH);

	return GetToken(RESTREAM_TOKEN_URL, client_id, RESTREAM_SCOPE_VERSION,
			QT_TO_UTF8(login.GetCode()), true);
}

std::shared_ptr<Auth> RestreamAuth::Login(QWidget *parent, const std::string &)
{
	OAuthLogin login(parent, RESTREAM_AUTH_URL, false);
	cef->add_popup_whitelist_url("about:blank", &login);

	if (login.exec() == QDialog::Rejected) {
		return nullptr;
	}

	std::shared_ptr<RestreamAuth> auth =
		std::make_shared<RestreamAuth>(restreamDef);

	std::string client_id = RESTREAM_CLIENTID;
	deobfuscate_str(&client_id[0], RESTREAM_HASH);

	if (!auth->GetToken(RESTREAM_TOKEN_URL, client_id,
			    RESTREAM_SCOPE_VERSION,
			    QT_TO_UTF8(login.GetCode()))) {
		return nullptr;
	}

	std::string error;
	if (auth->GetChannelInfo()) {
		return auth;
	}

	return nullptr;
}

static std::shared_ptr<Auth> CreateRestreamAuth()
{
	return std::make_shared<RestreamAuth>(restreamDef);
}

static void DeleteCookies()
{
	if (panel_cookies) {
		panel_cookies->DeleteCookies("restream.io", std::string());
	}
}

void RegisterRestreamAuth()
{
	OAuth::RegisterOAuth(restreamDef, CreateRestreamAuth,
			     RestreamAuth::Login, DeleteCookies);
}
