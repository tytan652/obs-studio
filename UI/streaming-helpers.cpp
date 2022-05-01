#include "streaming-helpers.hpp"
#include "qt-wrappers.hpp"
#include "obs-app.hpp"

#include "../plugins/rtmp-services/rtmp-format-ver.h"

#include <util/platform.h>
#include <util/util.hpp>

using namespace json11;

static Json open_json_file(const char *path)
{
	BPtr<char> file_data = os_quick_read_utf8_file(path);
	if (!file_data)
		return Json();

	std::string err;
	Json json = Json::parse(file_data, err);
	if (json["format_version"].int_value() != RTMP_SERVICES_FORMAT_VERSION)
		return Json();
	return json;
}

static inline bool name_matches(const Json &service, const char *name)
{
	if (service["name"].string_value() == name)
		return true;

	auto &alt_names = service["alt_names"].array_items();
	for (const Json &alt_name : alt_names) {
		if (alt_name.string_value() == name) {
			return true;
		}
	}

	return false;
}

Json get_services_json()
{
	obs_module_t *mod = obs_get_module("rtmp-services");
	Json root;

	BPtr<char> file = obs_module_get_config_path(mod, "services.json");
	if (file)
		root = open_json_file(file);

	if (root.is_null()) {
		file = obs_find_module_file(mod, "services.json");
		if (file)
			root = open_json_file(file);
	}

	return root;
}

Json get_service_from_json(const Json &root, const char *name)
{
	auto &services = root["services"].array_items();
	for (const Json &service : services) {
		if (name_matches(service, name)) {
			return service;
		}
	}

	return Json();
}

QString get_protocol_from_service(const QString &service_name)
{
	Json service = get_service_from_json(get_services_json(),
					     QT_TO_UTF8(service_name));
	if (!service["protocol"].is_string())
		return "RTMP";

	return QString::fromStdString(service["protocol"].string_value());
}

QString get_stream_output_type(const obs_service_t *service)
{
	QString protocol(obs_service_get_protocol(service));

	if (protocol.isEmpty()) {
		blog(LOG_WARNING, "The service '%s' has no protocol set",
		     obs_service_get_id(service));
		return QString();
	}

	if (!obs_output_is_protocol_registered(QT_TO_UTF8(protocol))) {
		blog(LOG_WARNING, "The protocol '%s' is not registered",
		     QT_TO_UTF8(protocol));
		return QString();
	}

	// Check if the service has a preferred output type
	QString type(obs_service_get_preferred_output_type(service));
	if (!type.isEmpty())
		return type;

	// Otherwise, prefer first-party output types
	if (protocol.startsWith("RTMP"))
		return "rtmp_output";
	else if (protocol.compare("HLS") == 0)
		return "ffmpeg_hls_muxer";
	else if (protocol.compare("SRT") == 0 || protocol.compare("RIST") == 0)
		return "ffmpeg_mpegts_muxer";

	// If third-party protocol, use the first enumarated type
	const char *first_type;
	if (obs_enum_output_service_types(QT_TO_UTF8(protocol), 0, &first_type))
		return QT_UTF8(first_type);

	blog(LOG_WARNING, "No output registered for the protocol '%s'",
	     QT_TO_UTF8(protocol));
	return QString();
}

bool StreamSettingsUI::IsServiceOutputHasNetworkFeatures()
{
	if (IsCustomService())
		return ui_customServer->text().startsWith("rtmp");

	Json service = get_service_from_json(
		GetServicesJson(), QT_TO_UTF8(ui_service->currentText()));

	if (!service["recommended"]["output"].is_string())
		return true;

	if (service["recommended"]["output"].string_value().compare(
		    "rtmp_output") == 0)
		return true;

	return false;
}

void StreamSettingsUI::UpdateMoreInfoLink()
{
	if (IsCustomService()) {
		ui_moreInfoButton->hide();
		return;
	}

	QString serviceName = ui_service->currentText();
	Json service = get_service_from_json(GetServicesJson(),
					     QT_TO_UTF8(serviceName));

	const std::string &more_info_link =
		service["more_info_link"].string_value();

	if (more_info_link.empty()) {
		ui_moreInfoButton->hide();
	} else {
		ui_moreInfoButton->setTargetUrl(QUrl(more_info_link.c_str()));
		ui_moreInfoButton->show();
	}
}

void StreamSettingsUI::UpdateKeyLink()
{
	QString serviceName = ui_service->currentText();
	QString customServer = ui_customServer->text().trimmed();

	Json service = get_service_from_json(GetServicesJson(),
					     QT_TO_UTF8(serviceName));

	std::string streamKeyLink = service["stream_key_link"].string_value();
	if (customServer.contains("fbcdn.net") && IsCustomService()) {
		streamKeyLink =
			"https://www.facebook.com/live/producer?ref=OBS";
	}

	if (serviceName == "Dacast") {
		ui_streamKeyLabel->setText(
			QTStr("Basic.AutoConfig.StreamPage.EncoderKey"));
	} else {
		ui_streamKeyLabel->setText(
			QTStr("Basic.AutoConfig.StreamPage.StreamKey"));
	}

	if (streamKeyLink.empty()) {
		ui_streamKeyButton->hide();
	} else {
		ui_streamKeyButton->setTargetUrl(QUrl(streamKeyLink.c_str()));
		ui_streamKeyButton->show();
	}
}

static bool check_service_protocol(const Json &service)
{
	if (service["protocol"].is_string())
		return obs_output_is_protocol_registered(
			service["protocol"].string_value().c_str());

	auto &servers = service["servers"].array_items();
	for (const Json &server : servers) {
		std::string url = server["url"].string_value();

		size_t pos = url.find("://");
		if (pos == std::string::npos)
			continue;

		if (!obs_output_get_prefix_protocol(
			    url.substr(0, pos + 3).c_str()))
			continue;

		return true;
	}

	return false;
}

void StreamSettingsUI::LoadServices(bool showAll)
{
	auto &services = GetServicesJson()["services"].array_items();

	ui_service->blockSignals(true);
	ui_service->clear();

	QStringList names;

	for (const Json &service : services) {
		if (!showAll && !service["common"].bool_value())
			continue;

		//Skip services with non-registered protocol
		if (!check_service_protocol(service))
			continue;

		names.push_back(service["name"].string_value().c_str());
	}

	if (showAll)
		names.sort(Qt::CaseInsensitive);

	for (QString &name : names)
		ui_service->addItem(name);

	if (!showAll) {
		ui_service->addItem(
			QTStr("Basic.AutoConfig.StreamPage.Service.ShowAll"),
			QVariant((int)ListOpt::ShowAll));
	}

	ui_service->insertItem(
		0, QTStr("Basic.AutoConfig.StreamPage.Service.Custom"),
		QVariant((int)ListOpt::Custom));

	if (!lastService.isEmpty()) {
		int idx = ui_service->findText(lastService);
		if (idx != -1)
			ui_service->setCurrentIndex(idx);
	}

	ui_service->blockSignals(false);
}

void StreamSettingsUI::UpdateServerList()
{
	QString serviceName = ui_service->currentText();
	bool showMore = ui_service->currentData().toInt() ==
			(int)ListOpt::ShowAll;

	if (showMore) {
		LoadServices(true);
		ui_service->showPopup();
		return;
	} else {
		lastService = serviceName;
	}

	Json service = get_service_from_json(GetServicesJson(),
					     QT_TO_UTF8(serviceName));

	ui_server->clear();

	auto &servers = service["servers"].array_items();
	for (const Json &entry : servers) {
		std::string url = entry["url"].string_value();

		// Skip RTMPS server if protocol not registered
		if (!obs_output_is_protocol_registered("RTMPS") &&
		    service["protocol"].is_null() &&
		    url.find("rtmps://") != std::string::npos)
			continue;

		ui_server->addItem(entry["name"].string_value().c_str(),
				   entry["url"].string_value().c_str());
	}
}

QString StreamSettingsUI::GetProtocol()
{
	Json service = get_service_from_json(
		GetServicesJson(), QT_TO_UTF8(ui_service->currentText()));
	if (!service["protocol"].is_string())
		return "RTMP";

	return QString::fromStdString(service["protocol"].string_value());
}
