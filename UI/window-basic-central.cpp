#include "window-basic-central.hpp"

#include <QScreen>

#include "window-basic-main.hpp"
#include "obs-proxy-style.hpp"

#include "display-helpers.hpp"

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

	auto displayResize = [this]() {
		struct obs_video_info ovi;

		if (obs_get_video_info(&ovi))
			ResizePreview(ovi.base_width, ovi.base_height);

		UpdateContextBarVisibility();
	};

	connect(main->windowHandle(), &QWindow::screenChanged, displayResize);
	connect(ui->preview, &OBSQTDisplay::DisplayResized, displayResize);

	auto addNudge = [this](const QKeySequence &seq, const char *s) {
		QAction *nudge = new QAction(ui->preview);
		nudge->setShortcut(seq);
		nudge->setShortcutContext(Qt::WidgetShortcut);
		ui->preview->addAction(nudge);
		connect(nudge, SIGNAL(triggered()), this, s);
	};

	addNudge(Qt::Key_Up, SLOT(NudgeUp()));
	addNudge(Qt::Key_Down, SLOT(NudgeDown()));
	addNudge(Qt::Key_Left, SLOT(NudgeLeft()));
	addNudge(Qt::Key_Right, SLOT(NudgeRight()));
	addNudge(Qt::SHIFT + Qt::Key_Up, SLOT(NudgeUpFar()));
	addNudge(Qt::SHIFT + Qt::Key_Down, SLOT(NudgeDownFar()));
	addNudge(Qt::SHIFT + Qt::Key_Left, SLOT(NudgeLeftFar()));
	addNudge(Qt::SHIFT + Qt::Key_Right, SLOT(NudgeRightFar()));
}

static bool nudge_callback(obs_scene_t *, obs_sceneitem_t *item, void *param)
{
	if (obs_sceneitem_locked(item))
		return true;

	struct vec2 &offset = *reinterpret_cast<struct vec2 *>(param);
	struct vec2 pos;

	if (!obs_sceneitem_selected(item)) {
		if (obs_sceneitem_is_group(item)) {
			struct vec3 offset3;
			vec3_set(&offset3, offset.x, offset.y, 0.0f);

			struct matrix4 matrix;
			obs_sceneitem_get_draw_transform(item, &matrix);
			vec4_set(&matrix.t, 0.0f, 0.0f, 0.0f, 1.0f);
			matrix4_inv(&matrix, &matrix);
			vec3_transform(&offset3, &offset3, &matrix);

			struct vec2 new_offset;
			vec2_set(&new_offset, offset3.x, offset3.y);
			obs_sceneitem_group_enum_items(item, nudge_callback,
						       &new_offset);
		}

		return true;
	}

	obs_sceneitem_get_pos(item, &pos);
	vec2_add(&pos, &pos, &offset);
	obs_sceneitem_set_pos(item, &pos);
	return true;
}

extern void undo_redo(const std::string &data);

void OBSBasicCentral::Nudge(int dist, MoveDir dir)
{
	if (ui->preview->Locked())
		return;

	struct vec2 offset;
	vec2_set(&offset, 0.0f, 0.0f);

	switch (dir) {
	case MoveDir::Up:
		offset.y = (float)-dist;
		break;
	case MoveDir::Down:
		offset.y = (float)dist;
		break;
	case MoveDir::Left:
		offset.x = (float)-dist;
		break;
	case MoveDir::Right:
		offset.x = (float)dist;
		break;
	}

	if (!recent_nudge) {
		recent_nudge = true;
		OBSDataAutoRelease wrapper = obs_scene_save_transform_states(
			main->GetCurrentScene(), true);
		std::string undo_data(obs_data_get_json(wrapper));

		nudge_timer = new QTimer;
		QObject::connect(
			nudge_timer, &QTimer::timeout,
			[this, &recent_nudge = recent_nudge, undo_data]() {
				OBSDataAutoRelease rwrapper =
					obs_scene_save_transform_states(
						main->GetCurrentScene(), true);
				std::string redo_data(
					obs_data_get_json(rwrapper));

				main->undo_s.add_action(
					QTStr("Undo.Transform")
						.arg(obs_source_get_name(
							main->GetCurrentSceneSource())),
					undo_redo, undo_redo, undo_data,
					redo_data);

				recent_nudge = false;
			});
		connect(nudge_timer, &QTimer::timeout, nudge_timer,
			&QTimer::deleteLater);
		nudge_timer->setSingleShot(true);
	}

	if (nudge_timer) {
		nudge_timer->stop();
		nudge_timer->start(1000);
	} else {
		blog(LOG_ERROR, "No nudge timer!");
	}

	obs_scene_enum_items(main->GetCurrentScene(), nudge_callback, &offset);
}

void OBSBasicCentral::ResizePreview(uint32_t cx, uint32_t cy)
{
	QSize targetSize;
	bool isFixedScaling;
	obs_video_info ovi;

	/* resize preview panel to fix to the top section of the window */
	targetSize = GetPixelSize(ui->preview);

	isFixedScaling = ui->preview->IsFixedScaling();
	obs_get_video_info(&ovi);

	if (isFixedScaling) {
		previewScale = ui->preview->GetScalingAmount();
		GetCenterPosFromFixedScale(
			int(cx), int(cy),
			targetSize.width() - PREVIEW_EDGE_SIZE * 2,
			targetSize.height() - PREVIEW_EDGE_SIZE * 2, previewX,
			previewY, previewScale);
		previewX += ui->preview->GetScrollX();
		previewY += ui->preview->GetScrollY();

	} else {
		GetScaleAndCenterPos(int(cx), int(cy),
				     targetSize.width() - PREVIEW_EDGE_SIZE * 2,
				     targetSize.height() -
					     PREVIEW_EDGE_SIZE * 2,
				     previewX, previewY, previewScale);
	}

	previewX += float(PREVIEW_EDGE_SIZE);
	previewY += float(PREVIEW_EDGE_SIZE);
}

void OBSBasicCentral::ResizeProgram(uint32_t cx, uint32_t cy)
{
	QSize targetSize;

	/* resize program panel to fix to the top section of the window */
	targetSize = GetPixelSize(program);
	GetScaleAndCenterPos(int(cx), int(cy),
			     targetSize.width() - PREVIEW_EDGE_SIZE * 2,
			     targetSize.height() - PREVIEW_EDGE_SIZE * 2,
			     programX, programY, programScale);

	programX += float(PREVIEW_EDGE_SIZE);
	programY += float(PREVIEW_EDGE_SIZE);
}

void OBSBasicCentral::RenderMain(void *data, uint32_t cx, uint32_t cy)
{
	GS_DEBUG_MARKER_BEGIN(GS_DEBUG_COLOR_DEFAULT, "RenderMain");

	OBSBasicCentral *widget = static_cast<OBSBasicCentral *>(data);
	obs_video_info ovi;

	obs_get_video_info(&ovi);

	widget->previewCX = int(widget->previewScale * float(ovi.base_width));
	widget->previewCY = int(widget->previewScale * float(ovi.base_height));

	gs_viewport_push();
	gs_projection_push();

	obs_display_t *display = widget->ui->preview->GetDisplay();
	uint32_t width, height;
	obs_display_size(display, &width, &height);
	float right = float(width) - widget->previewX;
	float bottom = float(height) - widget->previewY;

	gs_ortho(-widget->previewX, right, -widget->previewY, bottom, -100.0f,
		 100.0f);

	widget->ui->preview->DrawOverflow();

	/* --------------------------------------- */

	gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height),
		 -100.0f, 100.0f);
	gs_set_viewport(widget->previewX, widget->previewY, widget->previewCX,
			widget->previewCY);

	if (widget->main->IsPreviewProgramMode()) {
		widget->main->DrawBackdrop(float(ovi.base_width),
					   float(ovi.base_height));

		OBSScene scene = widget->main->GetCurrentScene();
		obs_source_t *source = obs_scene_get_source(scene);
		if (source)
			obs_source_video_render(source);
	} else {
		obs_render_main_texture_src_color_only();
	}
	gs_load_vertexbuffer(nullptr);

	/* --------------------------------------- */

	gs_ortho(-widget->previewX, right, -widget->previewY, bottom, -100.0f,
		 100.0f);
	gs_reset_viewport();

	widget->ui->preview->DrawSceneEditing();

	uint32_t targetCX = widget->previewCX;
	uint32_t targetCY = widget->previewCY;

	if (widget->main->drawSafeAreas) {
		RenderSafeAreas(widget->main->actionSafeMargin, targetCX,
				targetCY);
		RenderSafeAreas(widget->main->graphicsSafeMargin, targetCX,
				targetCY);
		RenderSafeAreas(widget->main->fourByThreeSafeMargin, targetCX,
				targetCY);
		RenderSafeAreas(widget->main->leftLine, targetCX, targetCY);
		RenderSafeAreas(widget->main->topLine, targetCX, targetCY);
		RenderSafeAreas(widget->main->rightLine, targetCX, targetCY);
	}

	/* --------------------------------------- */

	gs_projection_pop();
	gs_viewport_pop();

	GS_DEBUG_MARKER_END();

	UNUSED_PARAMETER(cx);
	UNUSED_PARAMETER(cy);
}

void OBSBasicCentral::RenderProgram(void *data, uint32_t cx, uint32_t cy)
{
	GS_DEBUG_MARKER_BEGIN(GS_DEBUG_COLOR_DEFAULT, "RenderProgram");

	OBSBasicCentral *widget = static_cast<OBSBasicCentral *>(data);
	obs_video_info ovi;

	obs_get_video_info(&ovi);

	widget->programCX = int(widget->programScale * float(ovi.base_width));
	widget->programCY = int(widget->programScale * float(ovi.base_height));

	gs_viewport_push();
	gs_projection_push();

	/* --------------------------------------- */

	gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height),
		 -100.0f, 100.0f);
	gs_set_viewport(widget->programX, widget->programY, widget->programCX,
			widget->programCY);

	obs_render_main_texture_src_color_only();
	gs_load_vertexbuffer(nullptr);

	/* --------------------------------------- */

	gs_projection_pop();
	gs_viewport_pop();

	GS_DEBUG_MARKER_END();

	UNUSED_PARAMETER(cx);
	UNUSED_PARAMETER(cy);
}

void OBSBasicCentral::OBSInit()
{
	auto addDisplay = [this](OBSQTDisplay *window) {
		obs_display_add_draw_callback(window->GetDisplay(),
					      OBSBasicCentral::RenderMain,
					      this);

		struct obs_video_info ovi;
		if (obs_get_video_info(&ovi))
			ResizePreview(ovi.base_width, ovi.base_height);
	};

	connect(ui->preview, &OBSQTDisplay::DisplayCreated, this, addDisplay);
}

void OBSBasicCentral::RemovePreviewDrawCallback()
{
	obs_display_remove_draw_callback(ui->preview->GetDisplay(),
					 OBSBasicCentral::RenderMain, this);
}

void OBSBasicCentral::CreateProgramDisplay()
{
	program = new OBSQTDisplay();

	program->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(program.data(), &QWidget::customContextMenuRequested, main,
		&OBSBasic::ProgramViewContextMenuRequested);

	auto displayResize = [this]() {
		struct obs_video_info ovi;

		if (obs_get_video_info(&ovi))
			ResizeProgram(ovi.base_width, ovi.base_height);
	};

	connect(program.data(), &OBSQTDisplay::DisplayResized, displayResize);

	auto addDisplay = [this](OBSQTDisplay *window) {
		obs_display_add_draw_callback(window->GetDisplay(),
					      OBSBasicCentral::RenderProgram,
					      this);

		struct obs_video_info ovi;
		if (obs_get_video_info(&ovi))
			ResizeProgram(ovi.base_width, ovi.base_height);
	};

	connect(program.data(), &OBSQTDisplay::DisplayCreated, addDisplay);

	program->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void OBSBasicCentral::AddProgramDisplay(QWidget *programOptions)
{
	programLabel = new QLabel(QTStr("StudioMode.Program"), this);
	programLabel->setSizePolicy(QSizePolicy::Preferred,
				    QSizePolicy::Preferred);
	programLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	programLabel->setProperty("themeID", "previewProgramLabels");

	programWidget = new QWidget();
	programLayout = new QVBoxLayout();

	programLayout->setContentsMargins(0, 0, 0, 0);
	programLayout->setSpacing(0);

	programLayout->addWidget(programLabel);
	programLayout->addWidget(program);

	bool labels = config_get_bool(GetGlobalConfig(), "BasicWindow",
				      "StudioModeLabels");

	programLabel->setHidden(!labels);

	programWidget->setLayout(programLayout);

	ui->previewLayout->addWidget(programOptions);
	ui->previewLayout->addWidget(programWidget);
	ui->previewLayout->setAlignment(programOptions, Qt::AlignCenter);
}

void OBSBasicCentral::RemoveProgramDisplay()
{
	delete program;
	delete programLabel;
	delete programWidget;
}

void OBSBasicCentral::ResizePreviewProgram(obs_video_info ovi)
{
	ResizePreview(ovi.base_width, ovi.base_height);
	if (program)
		ResizeProgram(ovi.base_width, ovi.base_height);
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

	if (programLabel)
		programLabel->setHidden(!labels);
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
