#include "window-basic-central.hpp"

#include "window-basic-main.hpp"
#include "obs-proxy-style.hpp"

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
}

void OBSBasicCentral::EnablePreviewDisplay(bool enable)
{
	obs_display_set_enabled(ui->preview->GetDisplay(), enable);
	ui->preview->setVisible(enable);
	ui->previewDisabledWidget->setVisible(!enable);
}

void OBSBasicCentral::ClearContextBar()
{
	QLayoutItem *la = ui->emptySpace->layout()->itemAt(0);
	if (la) {
		delete la->widget();
		ui->emptySpace->layout()->removeItem(la);
	}
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
