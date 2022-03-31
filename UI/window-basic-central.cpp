#include "window-basic-central.hpp"

#include "window-basic-main.hpp"
#include "obs-proxy-style.hpp"

#include "context-bar-controls.hpp"
#include "media-controls.hpp"

OBSBasicCentral::OBSBasicCentral(OBSBasic *main_)
	: QWidget(nullptr), main(main_), ui(new Ui::OBSBasicCentral)
{
	ui->setupUi(this);

	ui->previewDisabledWidget->setVisible(false);
	ui->contextContainer->setStyle(new OBSContextBarProxyStyle);

	ui->previewLabel->setProperty("themeID", "previewProgramLabels");
	ui->previewLabel->style()->polish(ui->previewLabel);

	bool labels = config_get_bool(GetGlobalConfig(), "BasicWindow",
				      "StudioModeLabels");

	if (!main->IsPreviewProgramMode())
		ui->previewLabel->setHidden(true);
	else
		ui->previewLabel->setHidden(!labels);

	ui->previewDisabledWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->previewDisabledWidget, &QFrame::customContextMenuRequested,
		main, &OBSBasic::PreviewDisabledMenu);
	connect(ui->enablePreviewButton, &QPushButton::clicked, main,
		&OBSBasic::TogglePreview);

	connect(ui->preview, &OBSBasicPreview::customContextMenuRequested, main,
		&OBSBasic::on_preview_customContextMenuRequested);

	connect(main, &OBSBasic::PreviewProgramModeChanged, this,
		[this](bool enabled) {
			this->ui->previewLabel->setHidden(!enabled);
		});
}

void OBSBasicCentral::EnablePreviewDisplay(bool enable)
{
	obs_display_set_enabled(ui->preview->GetDisplay(), enable);
	ui->preview->setVisible(enable);
	ui->previewDisabledWidget->setVisible(!enable);
}

void OBSBasicCentral::InitContextContainerVisibility(bool visible)
{
	ui->contextContainer->setVisible(visible);
	if (visible)
		UpdateContextBar(true);
}

obs_hotkey_pair_id OBSBasicCentral::CreateContextBarHotkeyPair()
{
#define MAKE_CALLBACK(pred, method, log_action)                            \
	[](void *data, obs_hotkey_pair_id, obs_hotkey_t *, bool pressed) { \
		OBSBasicCentral &central =                                 \
			*static_cast<OBSBasicCentral *>(data);             \
		if ((pred) && pressed) {                                   \
			blog(LOG_INFO, log_action " due to hotkey");       \
			method;                                            \
			return true;                                       \
		}                                                          \
		return false;                                              \
	}

	return obs_hotkey_pair_register_frontend(
		"OBSBasicCentral.ShowContextBar",
		Str("Basic.Main.ShowContextBar"),
		"OBSBasicCentral.HideContextBar",
		Str("Basic.Main.HideContextBar"),
		MAKE_CALLBACK(!central.ui->contextContainer->isVisible(),
			      central.UpdateContextContainerVisibility(true),
			      "Showing Context Bar"),
		MAKE_CALLBACK(central.ui->contextContainer->isVisible(),
			      central.UpdateContextContainerVisibility(false),
			      "Hiding Context Bar"),
		this, this);

#undef MAKE_CALLBACK
}

void OBSBasicCentral::ClearContextBar()
{
	QLayoutItem *la = ui->emptySpace->layout()->itemAt(0);
	if (la) {
		delete la->widget();
		ui->emptySpace->layout()->removeItem(la);
	}
}

void OBSBasicCentral::UpdateContextBarVisibility()
{
	int width = size().width();

	ContextBarSize contextBarSizeNew;
	if (width >= 740) {
		contextBarSizeNew = ContextBarSize_Normal;
	} else if (width >= 600) {
		contextBarSizeNew = ContextBarSize_Reduced;
	} else {
		contextBarSizeNew = ContextBarSize_Minimized;
	}

	if (contextBarSize == contextBarSizeNew)
		return;

	contextBarSize = contextBarSizeNew;

	QMetaObject::invokeMethod(this, "UpdateContextBar",
				  Qt::QueuedConnection, Q_ARG(bool, false));
}

void OBSBasicCentral::ResetUI()
{
	bool studioPortraitLayout = config_get_bool(
		GetGlobalConfig(), "BasicWindow", "StudioPortraitLayout");

	bool labels = config_get_bool(GetGlobalConfig(), "BasicWindow",
				      "StudioModeLabels");

	if (studioPortraitLayout)
		ui->previewLayout->setDirection(QBoxLayout::TopToBottom);
	else
		ui->previewLayout->setDirection(QBoxLayout::LeftToRight);

	if (main->IsPreviewProgramMode())
		ui->previewLabel->setHidden(!labels);
}

static bool is_network_media_source(obs_source_t *source, const char *id)
{
	if (strcmp(id, "ffmpeg_source") != 0)
		return false;

	OBSDataAutoRelease s = obs_source_get_settings(source);
	bool is_local_file = obs_data_get_bool(s, "is_local_file");

	return !is_local_file;
}

void OBSBasicCentral::UpdateContextBar(bool force)
{
	if (!ui->contextContainer->isVisible() && !force)
		return;

	OBSSceneItem item = main->GetCurrentSceneItem();

	if (item) {
		obs_source_t *source = obs_sceneitem_get_source(item);

		bool updateNeeded = true;
		QLayoutItem *la = ui->emptySpace->layout()->itemAt(0);
		if (la) {
			if (SourceToolbar *toolbar =
				    dynamic_cast<SourceToolbar *>(
					    la->widget())) {
				if (toolbar->GetSource() == source)
					updateNeeded = false;
			} else if (MediaControls *toolbar =
					   dynamic_cast<MediaControls *>(
						   la->widget())) {
				if (toolbar->GetSource() == source)
					updateNeeded = false;
			}
		}

		const char *id = obs_source_get_unversioned_id(source);
		uint32_t flags = obs_source_get_output_flags(source);

		ui->sourceInteractButton->setVisible(flags &
						     OBS_SOURCE_INTERACTION);

		if (contextBarSize >= ContextBarSize_Reduced &&
		    (updateNeeded || force)) {
			ClearContextBar();

			if (flags & OBS_SOURCE_CONTROLLABLE_MEDIA) {
				if (!is_network_media_source(source, id)) {
					MediaControls *mediaControls =
						new MediaControls(
							ui->emptySpace);
					mediaControls->SetSource(source);

					ui->emptySpace->layout()->addWidget(
						mediaControls);
				}
			} else if (strcmp(id, "browser_source") == 0) {
				BrowserToolbar *c = new BrowserToolbar(
					ui->emptySpace, source);
				ui->emptySpace->layout()->addWidget(c);
			} else if (strcmp(id, "wasapi_input_capture") == 0 ||
				   strcmp(id, "wasapi_output_capture") == 0 ||
				   strcmp(id, "coreaudio_input_capture") == 0 ||
				   strcmp(id, "coreaudio_output_capture") ==
					   0 ||
				   strcmp(id, "pulse_input_capture") == 0 ||
				   strcmp(id, "pulse_output_capture") == 0 ||
				   strcmp(id, "alsa_input_capture") == 0) {
				AudioCaptureToolbar *c =
					new AudioCaptureToolbar(ui->emptySpace,
								source);
				c->Init();
				ui->emptySpace->layout()->addWidget(c);
			} else if (strcmp(id, "window_capture") == 0 ||
				   strcmp(id, "xcomposite_input") == 0) {
				WindowCaptureToolbar *c =
					new WindowCaptureToolbar(ui->emptySpace,
								 source);
				c->Init();
				ui->emptySpace->layout()->addWidget(c);
			} else if (strcmp(id, "monitor_capture") == 0 ||
				   strcmp(id, "display_capture") == 0 ||
				   strcmp(id, "xshm_input") == 0) {
				DisplayCaptureToolbar *c =
					new DisplayCaptureToolbar(
						ui->emptySpace, source);
				c->Init();
				ui->emptySpace->layout()->addWidget(c);
			} else if (strcmp(id, "dshow_input") == 0) {
				DeviceCaptureToolbar *c =
					new DeviceCaptureToolbar(ui->emptySpace,
								 source);
				ui->emptySpace->layout()->addWidget(c);
			} else if (strcmp(id, "game_capture") == 0) {
				GameCaptureToolbar *c = new GameCaptureToolbar(
					ui->emptySpace, source);
				ui->emptySpace->layout()->addWidget(c);
			} else if (strcmp(id, "image_source") == 0) {
				ImageSourceToolbar *c = new ImageSourceToolbar(
					ui->emptySpace, source);
				ui->emptySpace->layout()->addWidget(c);
			} else if (strcmp(id, "color_source") == 0) {
				ColorSourceToolbar *c = new ColorSourceToolbar(
					ui->emptySpace, source);
				ui->emptySpace->layout()->addWidget(c);
			} else if (strcmp(id, "text_ft2_source") == 0 ||
				   strcmp(id, "text_gdiplus") == 0) {
				TextSourceToolbar *c = new TextSourceToolbar(
					ui->emptySpace, source);
				ui->emptySpace->layout()->addWidget(c);
			}
		} else if (contextBarSize == ContextBarSize_Minimized) {
			ClearContextBar();
		}

		QIcon icon;

		if (strcmp(id, "scene") == 0)
			icon = main->GetSceneIcon();
		else if (strcmp(id, "group") == 0)
			icon = main->GetGroupIcon();
		else
			icon = main->GetSourceIcon(id);

		QPixmap pixmap = icon.pixmap(QSize(16, 16));
		ui->contextSourceIcon->setPixmap(pixmap);
		ui->contextSourceIconSpacer->hide();
		ui->contextSourceIcon->show();

		const char *name = obs_source_get_name(source);
		ui->contextSourceLabel->setText(name);

		ui->sourceFiltersButton->setEnabled(true);
		ui->sourcePropertiesButton->setEnabled(
			obs_source_configurable(source));
	} else {
		ClearContextBar();
		ui->contextSourceIcon->hide();
		ui->contextSourceIconSpacer->show();
		ui->contextSourceLabel->setText(
			QTStr("ContextBar.NoSelectedSource"));

		ui->sourceFiltersButton->setEnabled(false);
		ui->sourcePropertiesButton->setEnabled(false);
		ui->sourceInteractButton->setVisible(false);
	}

	if (contextBarSize == ContextBarSize_Normal) {
		ui->sourcePropertiesButton->setText(QTStr("Properties"));
		ui->sourceFiltersButton->setText(QTStr("Filters"));
		ui->sourceInteractButton->setText(QTStr("Interact"));
	} else {
		ui->sourcePropertiesButton->setText("");
		ui->sourceFiltersButton->setText("");
		ui->sourceInteractButton->setText("");
	}
}

void OBSBasicCentral::UpdateContextContainerVisibility(bool visible)
{
	config_set_bool(GetGlobalConfig(), "BasicWindow", "ShowContextToolbars",
			visible);
	ui->contextContainer->setVisible(visible);
	UpdateContextBar(true);
}
