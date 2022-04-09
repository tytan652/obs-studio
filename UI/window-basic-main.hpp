/******************************************************************************
    Copyright (C) 2013-2014 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <QBuffer>
#include <QAction>
#include <QThread>
#include <QWidgetAction>
#include <QSystemTrayIcon>
#if defined(_WIN32) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QWinTaskbarButton>
#endif
#include <QStyledItemDelegate>
#include <DockManager.h>
#include <obs.hpp>
#include <vector>
#include <memory>
#include "window-main.hpp"
#include "window-basic-central.hpp"
#include "window-basic-interaction.hpp"
#include "window-basic-properties.hpp"
#include "window-basic-transform.hpp"
#include "window-basic-transitions.hpp"
#include "window-basic-scenes.hpp"
#include "window-basic-adv-audio.hpp"
#include "window-basic-filters.hpp"
#include "window-missing-files.hpp"
#include "window-projector.hpp"
#include "window-basic-about.hpp"
#include "auth-base.hpp"
#include "log-viewer.hpp"
#include "undo-stack-obs.hpp"

#include <obs-frontend-internal.hpp>

#include <util/platform.h>
#include <util/threading.h>
#include <util/util.hpp>

#include <QPointer>

class QMessageBox;
class QListWidgetItem;
class SourceTreeItem;
class VolControl;
class OBSBasicControls;
class OBSBasicMixer;
class OBSBasicSources;
class OBSBasicStats;
class OBSAdvDock;

#include "ui_OBSBasic.h"
#include "ui_ColorSelect.h"

#define DESKTOP_AUDIO_1 Str("DesktopAudioDevice1")
#define DESKTOP_AUDIO_2 Str("DesktopAudioDevice2")
#define AUX_AUDIO_1 Str("AuxAudioDevice1")
#define AUX_AUDIO_2 Str("AuxAudioDevice2")
#define AUX_AUDIO_3 Str("AuxAudioDevice3")
#define AUX_AUDIO_4 Str("AuxAudioDevice4")

#define SIMPLE_ENCODER_X264 "x264"
#define SIMPLE_ENCODER_X264_LOWCPU "x264_lowcpu"
#define SIMPLE_ENCODER_QSV "qsv"
#define SIMPLE_ENCODER_NVENC "nvenc"
#define SIMPLE_ENCODER_AMD "amd"

#define PREVIEW_EDGE_SIZE 10

struct BasicOutputHandler;

enum class QtDataRole {
	OBSRef = Qt::UserRole,
	OBSSignals,
};

struct SavedProjectorInfo {
	ProjectorType type;
	int monitor;
	std::string geometry;
	std::string name;
	bool alwaysOnTop;
	bool alwaysOnTopOverridden;
};

struct SourceCopyInfo {
	OBSWeakSource weak_source;
	bool visible;
	obs_sceneitem_crop crop;
	obs_transform_info transform;
	obs_blending_method blend_method;
	obs_blending_type blend_mode;
};

struct QuickTransition {
	QPushButton *button = nullptr;
	OBSSource source;
	obs_hotkey_id hotkey = OBS_INVALID_HOTKEY_ID;
	int duration = 0;
	int id = 0;
	bool fadeToBlack = false;

	inline QuickTransition() {}
	inline QuickTransition(OBSSource source_, int duration_, int id_,
			       bool fadeToBlack_ = false)
		: source(source_),
		  duration(duration_),
		  id(id_),
		  fadeToBlack(fadeToBlack_),
		  renamedSignal(std::make_shared<OBSSignal>(
			  obs_source_get_signal_handler(source), "rename",
			  SourceRenamed, this))
	{
	}

private:
	static void SourceRenamed(void *param, calldata_t *data);
	std::shared_ptr<OBSSignal> renamedSignal;
};

class ColorSelect : public QWidget {

public:
	explicit ColorSelect(QWidget *parent = 0);

private:
	std::unique_ptr<Ui::ColorSelect> ui;
};

#ifdef BROWSER_AVAILABLE
struct PluginBrowserDockParams {
	QString uniqueName;
	QString title;
	int defaultWidth;
	int defaultHeight;
	int minWidth;
	int minHeight;
	QString url;
	bool enableCookie;
	QString startupScript;
	QStringList forcePopupUrls;
};
#endif

class OBSBasic : public OBSMainWindow {
	Q_OBJECT
	Q_PROPERTY(QIcon imageIcon READ GetImageIcon WRITE SetImageIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon colorIcon READ GetColorIcon WRITE SetColorIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon slideshowIcon READ GetSlideshowIcon WRITE
			   SetSlideshowIcon DESIGNABLE true)
	Q_PROPERTY(QIcon audioInputIcon READ GetAudioInputIcon WRITE
			   SetAudioInputIcon DESIGNABLE true)
	Q_PROPERTY(QIcon audioOutputIcon READ GetAudioOutputIcon WRITE
			   SetAudioOutputIcon DESIGNABLE true)
	Q_PROPERTY(QIcon desktopCapIcon READ GetDesktopCapIcon WRITE
			   SetDesktopCapIcon DESIGNABLE true)
	Q_PROPERTY(QIcon windowCapIcon READ GetWindowCapIcon WRITE
			   SetWindowCapIcon DESIGNABLE true)
	Q_PROPERTY(QIcon gameCapIcon READ GetGameCapIcon WRITE SetGameCapIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon cameraIcon READ GetCameraIcon WRITE SetCameraIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon textIcon READ GetTextIcon WRITE SetTextIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon mediaIcon READ GetMediaIcon WRITE SetMediaIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon browserIcon READ GetBrowserIcon WRITE SetBrowserIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon groupIcon READ GetGroupIcon WRITE SetGroupIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon sceneIcon READ GetSceneIcon WRITE SetSceneIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon defaultIcon READ GetDefaultIcon WRITE SetDefaultIcon
			   DESIGNABLE true)

	friend class OBSAbout;
	friend class OBSBasicPreview;
	friend class OBSBasicStatusBar;
	friend class OBSBasicSourceSelect;
	friend class OBSBasicTransform;
	friend class OBSBasicSettings;
	friend class Auth;
	friend class AutoConfig;
	friend class AutoConfigStreamPage;
	friend class ExtraBrowsersModel;
	friend class ExtraBrowsersDelegate;
	friend class DeviceCaptureToolbar;
	friend class DeviceToolbarPropertiesThread;
	friend class OBSBasicSourceSelect;
	friend class OBSYoutubeActions;
	friend struct BasicOutputHandler;
	friend struct OBSStudioAPI;

	// Allow this class access to private slots and gs_vertbuffer_t attributes
	friend class OBSBasicCentral;

	// Allow those classes to connect OBSBasic private slots
	friend class OBSBasicScenes;
	friend class OBSBasicSources;
	friend class OBSBasicMixer;
	friend class OBSBasicTransitions;
	friend class OBSBasicControls;

	enum class MoveDir { Up, Down, Left, Right };

	enum DropType {
		DropType_RawText,
		DropType_Text,
		DropType_Image,
		DropType_Media,
		DropType_Html,
		DropType_Url,
	};

private:
	obs_frontend_callbacks *api = nullptr;

	std::shared_ptr<Auth> auth;

	std::vector<VolControl *> volumes;

	std::vector<OBSSignal> signalHandlers;

	/* Legacy docks */
	QPointer<QMenu> legacyDocksMenu;
	QList<QPointer<QDockWidget>> legacyExtraDocks;

	bool loaded = false;
	long disableSaving = 1;
	bool projectChanged = false;
	bool previewEnabled = true;

	std::deque<SourceCopyInfo> clipboard;
	OBSWeakSourceAutoRelease copyFiltersSource;
	bool copyVisible = true;
	obs_transform_info copiedTransformInfo;
	obs_sceneitem_crop copiedCropInfo;
	bool hasCopiedTransform = false;
	OBSWeakSourceAutoRelease copySourceTransition;

	bool closing = false;
	QScopedPointer<QThread> devicePropertiesThread;
	QScopedPointer<QThread> whatsNewInitThread;
	QScopedPointer<QThread> updateCheckThread;
	QScopedPointer<QThread> introCheckThread;
	QScopedPointer<QThread> logUploadThread;

	QPointer<OBSBasicInteraction> interaction;
	QPointer<OBSBasicProperties> properties;
	QPointer<OBSBasicTransform> transformWindow;
	QPointer<OBSBasicAdvAudio> advAudioWindow;
	QPointer<OBSBasicFilters> filters;
	QPointer<OBSAdvDock> statsDock;
	QPointer<OBSAbout> about;
	QPointer<OBSMissingFiles> missDialog;
	QPointer<OBSLogViewer> logView;

	QPointer<QTimer> cpuUsageTimer;
	QPointer<QTimer> diskFullTimer;

	os_cpu_usage_info_t *cpuUsageInfo = nullptr;

	OBSService service;
	std::unique_ptr<BasicOutputHandler> outputHandler;
	bool streamingStopping = false;
	bool recordingStopping = false;
	bool replayBufferStopping = false;

	gs_vertbuffer_t *box = nullptr;
	gs_vertbuffer_t *boxLeft = nullptr;
	gs_vertbuffer_t *boxTop = nullptr;
	gs_vertbuffer_t *boxRight = nullptr;
	gs_vertbuffer_t *boxBottom = nullptr;
	gs_vertbuffer_t *circle = nullptr;

	gs_vertbuffer_t *actionSafeMargin = nullptr;
	gs_vertbuffer_t *graphicsSafeMargin = nullptr;
	gs_vertbuffer_t *fourByThreeSafeMargin = nullptr;
	gs_vertbuffer_t *leftLine = nullptr;
	gs_vertbuffer_t *topLine = nullptr;
	gs_vertbuffer_t *rightLine = nullptr;

	ConfigFile basicConfig;

	std::vector<SavedProjectorInfo *> savedProjectorsArray;
	std::vector<OBSProjector *> projectors;

	QPointer<QWidget> stats;
	QPointer<QWidget> remux;
	QPointer<QWidget> extraBrowsers;
	QPointer<QWidget> importer;

	QPointer<QPushButton> transitionButton;

	bool vcamEnabled = false;

	QScopedPointer<QSystemTrayIcon> trayIcon;
	QPointer<QAction> sysTrayStream;
	QPointer<QAction> sysTrayRecord;
	QPointer<QAction> sysTrayReplayBuffer;
	QPointer<QAction> sysTrayVirtualCam;
	QPointer<QAction> showHide;
	QPointer<QAction> exit;
	QPointer<QMenu> trayMenu;
	QPointer<QMenu> previewProjector;
	QPointer<QMenu> studioProgramProjector;
	QPointer<QMenu> multiviewProjectorMenu;
	QPointer<QMenu> previewProjectorSource;
	QPointer<QMenu> previewProjectorMain;
	QPointer<QMenu> sceneProjectorMenu;
	QPointer<QMenu> sourceProjector;
	QPointer<QMenu> scaleFilteringMenu;
	QPointer<QMenu> blendingMethodMenu;
	QPointer<QMenu> blendingModeMenu;
	QPointer<QMenu> colorMenu;
	QPointer<QWidgetAction> colorWidgetAction;
	QPointer<ColorSelect> colorSelect;
	QPointer<QMenu> deinterlaceMenu;
	QPointer<QMenu> perSceneTransitionMenu;
	QPointer<QObject> shortcutFilter;
	QPointer<QAction> renameScene;
	QPointer<QAction> renameSource;

#if defined(_WIN32) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QWinTaskbarButton *taskBtn = new QWinTaskbarButton(this);
#endif

	QScopedPointer<QThread> patronJsonThread;
	std::string patronJson;

	void UpdateMultiviewProjectorMenu();

	void DrawBackdrop(float cx, float cy);

	void SetupEncoders();

	void CreateFirstRunSources();
	void CreateDefaultScene(bool firstStart);

	void UpdateVolumeControlsDecayRate();
	void UpdateVolumeControlsPeakMeterType();
	void ClearVolumeControls();

	void UploadLog(const char *subdir, const char *file, const bool crash);

	void Save(const char *file);
	void LoadData(obs_data_t *data, const char *file);
	void Load(const char *file);

	void InitHotkeys();
	void CreateHotkeys();
	void ClearHotkeys();

	bool InitService();

	bool InitBasicConfigDefaults();
	void InitBasicConfigDefaults2();
	bool InitBasicConfig();

	void InitOBSCallbacks();

	void InitPrimitives();

	void OnFirstLoad();

	OBSSceneItem GetSceneItem(QListWidgetItem *item);
	OBSSceneItem GetCurrentSceneItem();

	bool QueryRemoveSource(obs_source_t *source);

	void TimedCheckForUpdates();
	void CheckForUpdates(bool manualUpdate);

	void GetFPSCommon(uint32_t &num, uint32_t &den) const;
	void GetFPSInteger(uint32_t &num, uint32_t &den) const;
	void GetFPSFraction(uint32_t &num, uint32_t &den) const;
	void GetFPSNanoseconds(uint32_t &num, uint32_t &den) const;
	void GetConfigFPS(uint32_t &num, uint32_t &den) const;

	void UpdatePreviewScalingMenu();

	void LoadSceneListOrder(obs_data_array_t *array);
	obs_data_array_t *SaveSceneListOrder();
	void ChangeSceneIndex(bool relative, int idx, int invalidIdx);

	void TempFileOutput(const char *path, int vBitrate, int aBitrate);
	void TempStreamOutput(const char *url, const char *key, int vBitrate,
			      int aBitrate);

	void CloseDialogs();
	void ClearSceneData();
	void ClearProjectors();

	OBSProjector *OpenProjector(obs_source_t *source, int monitor,
				    ProjectorType type);

	void GetAudioSourceFilters();
	void GetAudioSourceProperties();
	void VolControlContextMenu();
	void ToggleVolControlLayout();

	void RefreshSceneCollections();
	void ChangeSceneCollection();
	void LogScenes();

	void ResetProfileData();
	bool AddProfile(bool create_new, const char *title, const char *text,
			const char *init_text = nullptr, bool rename = false);
	bool CreateProfile(const std::string &newName, bool create_new,
			   bool showWizardChecked, bool rename = false);
	void DeleteProfile(const char *profile_name, const char *profile_dir);
	void RefreshProfiles();
	void ChangeProfile();
	void CheckForSimpleModeX264Fallback();

	void SaveProjectNow();

	int GetTopSelectedSourceItem();

	QModelIndexList GetAllSelectedSourceItems();

	obs_hotkey_pair_id streamingHotkeys, recordingHotkeys, pauseHotkeys,
		replayBufHotkeys, vcamHotkeys, togglePreviewHotkeys,
		contextBarHotkeys;
	obs_hotkey_id forceStreamingStopHotkey;

	void InitDefaultTransitions();
	void InitTransition(obs_source_t *transition);
	obs_source_t *FindTransition(const char *name);
	OBSSource GetCurrentTransition();
	obs_data_array_t *SaveTransitions();
	void LoadTransitions(obs_data_array_t *transitions,
			     obs_load_source_cb cb, void *private_data);

	obs_source_t *fadeTransition;
	obs_source_t *cutTransition;

	void CreateProgramOptions();
	int TransitionCount();
	int AddTransitionBeforeSeparator(const QString &name,
					 obs_source_t *source);
	void AddQuickTransitionId(int id);
	void AddQuickTransition();
	void AddQuickTransitionHotkey(QuickTransition *qt);
	void RemoveQuickTransitionHotkey(QuickTransition *qt);
	void LoadQuickTransitions(obs_data_array_t *array);
	obs_data_array_t *SaveQuickTransitions();
	void ClearQuickTransitionWidgets();
	void RefreshQuickTransitions();
	void DisableQuickTransitionWidgets();
	void EnableTransitionWidgets(bool enable);
	void CreateDefaultQuickTransitions();

	void PasteShowHideTransition(obs_sceneitem_t *item, bool show,
				     obs_source_t *tr);
	QMenu *CreatePerSceneTransitionMenu();
	QMenu *CreateVisibilityTransitionMenu(bool visible);

	QuickTransition *GetQuickTransition(int id);
	int GetQuickTransitionIdx(int id);
	QMenu *CreateTransitionMenu(QWidget *parent, QuickTransition *qt);
	void ClearQuickTransitions();
	void QuickTransitionClicked();
	void QuickTransitionChange();
	void QuickTransitionChangeDuration(int value);
	void QuickTransitionRemoveClicked();

	void SetPreviewProgramMode(bool enabled);
	void SetCurrentScene(obs_scene_t *scene, bool force = false);

	std::vector<QuickTransition> quickTransitions;
	QPointer<QWidget> programOptions;
	OBSWeakSource lastScene;
	OBSWeakSource swapScene;
	OBSWeakSource programScene;
	bool editPropertiesMode = false;
	bool sceneDuplicationMode = true;
	bool swapScenesMode = true;
	volatile bool previewProgramMode = false;
	obs_hotkey_id togglePreviewProgramHotkey = 0;
	obs_hotkey_id transitionHotkey = 0;
	obs_hotkey_id statsHotkey = 0;
	obs_hotkey_id screenshotHotkey = 0;
	obs_hotkey_id sourceScreenshotHotkey = 0;
	int quickTransitionIdCounter = 1;
	bool overridingTransition = false;

	int disableOutputsRef = 0;

	inline void OnActivate(bool force = false);
	inline void OnDeactivate();

	void AddDropSource(const char *file, DropType image);
	void AddDropURL(const char *url, QString &name, obs_data_t *settings,
			const obs_video_info &ovi);
	void ConfirmDropUrl(const QString &url);
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

	void ReplayBufferClicked();

	bool sysTrayMinimizeToTray();

	void EnumDialogs();

	QList<QDialog *> visDialogs;
	QList<QDialog *> modalDialogs;
	QList<QMessageBox *> visMsgBoxes;

	QList<QPoint> visDlgPositions;

	QByteArray startingWindowState;

	obs_data_array_t *SaveProjectors();
	void LoadSavedProjectors(obs_data_array_t *savedProjectors);

	void ReceivedIntroJson(const QString &text);
	void ShowWhatsNew(const QString &url);

	/* This separator is added after custom browser docks action
	 * or legacy docks menu if one of them is added. */
	QPointer<QAction> secondMenuDocksSeparator;
	// This separator separate extra browser docks from other docks
	QPointer<QAction> extraBrowserMenuDocksSeparator;
#ifdef BROWSER_AVAILABLE
	QStringList extraBrowserDockNames;
	QStringList extraBrowserDockTargets;

	void ClearExtraBrowserDocks();
	void LoadExtraBrowserDocks();
	void SaveExtraBrowserDocks();
	void ManageExtraBrowserDocks();
	void AddExtraBrowserDock(const QString &title, const QString &url,
				 const QString &uuid, bool firstCreate);

	bool IsBrowserInitialised();
	QList<PluginBrowserDockParams> beforeInitBrowserDocks;
	inline void StorePluginBrowserDock(PluginBrowserDockParams params)
	{
		beforeInitBrowserDocks.append(params);
	}
	void LoadPluginBrowserDock();
	void AddPluginBrowserDock(PluginBrowserDockParams params);
	void DeleteBrowserCookies(const QString &url);
#endif

	QIcon imageIcon;
	QIcon colorIcon;
	QIcon slideshowIcon;
	QIcon audioInputIcon;
	QIcon audioOutputIcon;
	QIcon desktopCapIcon;
	QIcon windowCapIcon;
	QIcon gameCapIcon;
	QIcon cameraIcon;
	QIcon textIcon;
	QIcon mediaIcon;
	QIcon browserIcon;
	QIcon groupIcon;
	QIcon sceneIcon;
	QIcon defaultIcon;

	QIcon GetImageIcon() const;
	QIcon GetColorIcon() const;
	QIcon GetSlideshowIcon() const;
	QIcon GetAudioInputIcon() const;
	QIcon GetAudioOutputIcon() const;
	QIcon GetDesktopCapIcon() const;
	QIcon GetWindowCapIcon() const;
	QIcon GetGameCapIcon() const;
	QIcon GetCameraIcon() const;
	QIcon GetTextIcon() const;
	QIcon GetMediaIcon() const;
	QIcon GetBrowserIcon() const;
	QIcon GetDefaultIcon() const;

	QSlider *tBar;
	bool tBarActive = false;

	OBSSource GetOverrideTransition(OBSSource source);
	int GetOverrideTransitionDuration(OBSSource source);

	void UpdateProjectorHideCursor();
	void UpdateProjectorAlwaysOnTop(bool top);
	void ResetProjectors();

	QPointer<QObject> screenshotData;

	void MoveSceneItem(enum obs_order_movement movement,
			   const QString &action_name);

	bool autoStartBroadcast = true;
	bool autoStopBroadcast = true;
	bool broadcastActive = false;
	bool broadcastReady = false;
	QPointer<QThread> youtubeStreamCheckThread;
#if YOUTUBE_ENABLED
	void YoutubeStreamCheck(const std::string &key);
	void ShowYouTubeAutoStartWarning();
	void YouTubeActionDialogOk(const QString &id, const QString &key,
				   bool autostart, bool autostop,
				   bool start_now);
#endif
	void BroadcastButtonClicked();
	void SetBroadcastFlowEnabled(bool enabled);

	void UpdatePreviewSafeAreas();
	bool drawSafeAreas = false;

	bool IsRecordingPausable();

	/* Central widget */
	QPointer<OBSBasicCentral> centralWidget;

	/* Scenes dock */
	QPointer<OBSBasicScenes> scenesWidget;
	QPointer<OBSAdvDock> scenesDock;
	inline SceneTree *GetScenes() { return scenesWidget->ui->scenes; }

	/* Sources dock */
	QPointer<OBSBasicSources> sourcesWidget;
	QPointer<OBSAdvDock> sourcesDock;

	/* Mixer dock */
	QPointer<OBSBasicMixer> mixerWidget;
	QPointer<OBSAdvDock> mixerDock;

	/* Transitions dock */
	QPointer<OBSBasicTransitions> transitionsWidget;
	QPointer<OBSAdvDock> transitionsDock;
	int transitionDuration = 300;
	inline QComboBox *GetTransitions()
	{
		return transitionsWidget->ui->transitions;
	}

	/* Controls dock */
	QPointer<OBSBasicControls> controlsWidget;
	QPointer<OBSAdvDock> controlsDock;

	QStringList extraDockNames;

public slots:
	void DeferSaveBegin();
	void DeferSaveEnd();

	void DisplayStreamStartError();

	void SetupBroadcast();

	void StartStreaming();
	void StopStreaming();
	void ForceStopStreaming();

	void StreamDelayStarting(int sec);
	void StreamDelayStopping(int sec);

	void StreamingStart();
	void StreamStopping();
	void StreamingStop(int errorcode, QString last_error);

	void StartRecording();
	void StopRecording();

	void RecordingStart();
	void RecordStopping();
	void RecordingStop(int code, QString last_error);
	void RecordingFileChanged(QString lastRecordingPath);

	void ShowReplayBufferPauseWarning();
	void StartReplayBuffer();
	void StopReplayBuffer();

	void ReplayBufferStart();
	void ReplayBufferSave();
	void ReplayBufferSaved();
	void ReplayBufferStopping();
	void ReplayBufferStop(int code);

	void StartVirtualCam();
	void StopVirtualCam();

	void OnVirtualCamStart();
	void OnVirtualCamStop(int code);

	void SaveProjectDeferred();
	void SaveProject();

	void SetTransition(OBSSource transition);
	void OverrideTransition(OBSSource transition);
	void TransitionToScene(OBSScene scene, bool force = false);
	void TransitionToScene(OBSSource scene, bool force = false,
			       bool quickTransition = false,
			       int quickDuration = 0, bool black = false,
			       bool manual = false);
	void SetCurrentScene(OBSSource scene, bool force = false);

	bool AddSceneCollection(bool create_new,
				const QString &name = QString());

	bool NewProfile(const QString &name);
	bool DuplicateProfile(const QString &name);
	void DeleteProfile(const QString &profileName);

	void UpdatePatronJson(const QString &text, const QString &error);

	void PauseRecording();
	void UnpauseRecording();

private slots:

	void on_actionMainUndo_triggered();
	void on_actionMainRedo_triggered();

	void AddSceneItem(OBSSceneItem item);
	void AddScene(OBSSource source);
	void RemoveScene(OBSSource source);
	void RenameSources(OBSSource source, QString newName, QString prevName);

	void ActivateAudioSource(OBSSource source);
	void DeactivateAudioSource(OBSSource source);

	void DuplicateSelectedScene();
	void RemoveSelectedScene();

	void ToggleAlwaysOnTop();

	void ReorderSources(OBSScene scene);
	void RefreshSources(OBSScene scene);

	void ProcessHotkey(obs_hotkey_id id, bool pressed);

	void AddTransition(QString id);
	void RenameTransition();
	void TransitionClicked();
	void TransitionStopped();
	void TransitionFullyStopped();
	void TriggerQuickTransition(int id);

	void SetDeinterlacingMode();
	void SetDeinterlacingOrder();

	void SetScaleFilter();

	void SetBlendingMethod();
	void SetBlendingMode();

	void IconActivated(QSystemTrayIcon::ActivationReason reason);
	void SetShowing(bool showing);

	void ToggleShowHide();

	void HideAudioControl();
	void UnhideAllAudioControls();
	void ToggleHideMixer();

	void MixerRenameSource();

	void on_actionCopySource_triggered();
	void on_actionPasteRef_triggered();
	void on_actionPasteDup_triggered();

	void on_actionCopyFilters_triggered();
	void on_actionPasteFilters_triggered();

	void ColorChange();

	SourceTreeItem *GetItemWidgetFromSceneItem(obs_sceneitem_t *sceneItem);

	void on_actionShowAbout_triggered();

	void AudioMixerCopyFilters();
	void AudioMixerPasteFilters();

	void EnablePreview();
	void DisablePreview();

	void SceneCopyFilters();
	void ScenePasteFilters();

	void CheckDiskSpaceRemaining();
	void OpenSavedProjector(SavedProjectorInfo *info);

	void ScenesReordered();

	void ResetStatsHotkey();

	void SetImageIcon(const QIcon &icon);
	void SetColorIcon(const QIcon &icon);
	void SetSlideshowIcon(const QIcon &icon);
	void SetAudioInputIcon(const QIcon &icon);
	void SetAudioOutputIcon(const QIcon &icon);
	void SetDesktopCapIcon(const QIcon &icon);
	void SetWindowCapIcon(const QIcon &icon);
	void SetGameCapIcon(const QIcon &icon);
	void SetCameraIcon(const QIcon &icon);
	void SetTextIcon(const QIcon &icon);
	void SetMediaIcon(const QIcon &icon);
	void SetBrowserIcon(const QIcon &icon);
	void SetGroupIcon(const QIcon &icon);
	void SetSceneIcon(const QIcon &icon);
	void SetDefaultIcon(const QIcon &icon);

	void TBarChanged(int value);
	void TBarReleased();

	void LockVolumeControl(bool lock);

private:
	/* OBS Callbacks */
	static void SceneReordered(void *data, calldata_t *params);
	static void SceneRefreshed(void *data, calldata_t *params);
	static void SceneItemAdded(void *data, calldata_t *params);
	static void SourceCreated(void *data, calldata_t *params);
	static void SourceRemoved(void *data, calldata_t *params);
	static void SourceActivated(void *data, calldata_t *params);
	static void SourceDeactivated(void *data, calldata_t *params);
	static void SourceAudioActivated(void *data, calldata_t *params);
	static void SourceAudioDeactivated(void *data, calldata_t *params);
	static void SourceRenamed(void *data, calldata_t *params);

	void AddSource(const char *id);
	QMenu *CreateAddSourcePopupMenu();
	void AddSourcePopupMenu(const QPoint &pos);

	static void HotkeyTriggered(void *data, obs_hotkey_id id, bool pressed);

	void AutoRemux(QString input, bool no_show = false);

	bool OutputPathValid();
	void OutputPathInvalidMessage();

	bool LowDiskSpace();
	void DiskSpaceMessage();

	OBSSource prevFTBSource = nullptr;

public:
	undo_stack undo_s;
	OBSSource GetProgramSource();
	OBSScene GetCurrentScene();

	void SysTrayNotify(const QString &text, QSystemTrayIcon::MessageIcon n);

	inline OBSSource GetCurrentSceneSource()
	{
		OBSScene curScene = GetCurrentScene();
		return OBSSource(obs_scene_get_source(curScene));
	}

	obs_service_t *GetService();
	void SetService(obs_service_t *service);

	int GetTransitionDuration();
	int GetTbarPosition();

	inline bool IsPreviewProgramMode() const
	{
		return os_atomic_load_bool(&previewProgramMode);
	}

	inline bool VCamEnabled() const { return vcamEnabled; }

	bool Active() const;

	int ResetVideo();
	bool ResetAudio();

	void ResetOutputs();

	void ResetAudioDevice(const char *sourceId, const char *deviceId,
			      const char *deviceDesc, int channel);

	void NewProject();
	void LoadProject();

	inline bool SavingDisabled() const { return disableSaving; }

	inline double GetCPUUsage() const
	{
		return os_cpu_usage_info_query(cpuUsageInfo);
	}

	void SaveService();
	bool LoadService();

	inline Auth *GetAuth() { return auth.get(); }

	inline void EnableOutputs(bool enable)
	{
		if (enable) {
			if (--disableOutputsRef < 0)
				disableOutputsRef = 0;
		} else {
			disableOutputsRef++;
		}
	}

	QMenu *AddDeinterlacingMenu(QMenu *menu, obs_source_t *source);
	QMenu *AddScaleFilteringMenu(QMenu *menu, obs_sceneitem_t *item);
	QMenu *AddBlendingMethodMenu(QMenu *menu, obs_sceneitem_t *item);
	QMenu *AddBlendingModeMenu(QMenu *menu, obs_sceneitem_t *item);
	QMenu *AddBackgroundColorMenu(QMenu *menu, QWidgetAction *widgetAction,
				      ColorSelect *select,
				      obs_sceneitem_t *item);
	void CreateSourcePopupMenu(int idx, bool preview);

	void UpdateTitleBar();

	void SystemTrayInit();
	void SystemTray(bool firstStarted);

	void OpenSavedProjectors();

	void CreateInteractionWindow(obs_source_t *source);
	void CreatePropertiesWindow(obs_source_t *source);
	void CreateFiltersWindow(obs_source_t *source);

	/* Legacy docks */
	QAction *AddDockWidget(QDockWidget *dock);
	/* Advanced docks */
	void AddAdvDockWidget(OBSAdvDock *dock, bool browser = false);
	void RemoveAdvDockWidget(const QString &title);
	bool ApplyDocksLayout(const QByteArray &state);
	inline QByteArray AdvDockState() { return dockManager->saveState(); }
	inline bool RestoreAdvDocksState(const QByteArray &state)
	{
		return dockManager->restoreState(state);
	}

	static OBSBasic *Get();

	const char *GetCurrentOutputPath();

	void DeleteProjector(OBSProjector *projector);
	void AddProjectorMenuMonitors(QMenu *parent, QObject *target,
				      const char *slot);

	QIcon GetSourceIcon(const char *id) const;
	QIcon GetGroupIcon() const;
	QIcon GetSceneIcon() const;

	OBSWeakSource copyFilter;

	void ShowStatusBarMessage(const QString &message);

	static OBSData
	BackupScene(obs_scene_t *scene,
		    std::vector<obs_source_t *> *sources = nullptr);
	void CreateSceneUndoRedoAction(const QString &action_name,
				       OBSData undo_data, OBSData redo_data);

	static inline OBSData
	BackupScene(obs_source_t *scene_source,
		    std::vector<obs_source_t *> *sources = nullptr)
	{
		obs_scene_t *scene = obs_scene_from_source(scene_source);
		return BackupScene(scene, sources);
	}

	void CreateFilterPasteUndoRedoAction(const QString &text,
					     obs_source_t *source,
					     obs_data_array_t *undo_array,
					     obs_data_array_t *redo_array);

	void UpdateEditMenu();

	void SetDisplayAffinity(QWindow *window);

protected:
	virtual void closeEvent(QCloseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	virtual bool nativeEvent(const QByteArray &eventType, void *message,
				 qintptr *result) override;
#else
	virtual bool nativeEvent(const QByteArray &eventType, void *message,
				 long *result) override;
#endif
	virtual void changeEvent(QEvent *event) override;

private slots:
	void on_actionFullscreenInterface_triggered();

	void on_actionShow_Recordings_triggered();
	void on_actionRemux_triggered();
	void on_action_Settings_triggered();
	void on_actionShowMissingFiles_triggered();
	void on_actionAdvAudioProperties_triggered();
	void on_actionShowLogs_triggered();
	void on_actionUploadCurrentLog_triggered();
	void on_actionUploadLastLog_triggered();
	void on_actionViewCurrentLog_triggered();
	void on_actionCheckForUpdates_triggered();

	void on_actionShowCrashLogs_triggered();
	void on_actionUploadLastCrashLog_triggered();

	void on_actionEditTransform_triggered();
	void on_actionCopyTransform_triggered();
	void on_actionPasteTransform_triggered();
	void on_actionRotate90CW_triggered();
	void on_actionRotate90CCW_triggered();
	void on_actionRotate180_triggered();
	void on_actionFlipHorizontal_triggered();
	void on_actionFlipVertical_triggered();
	void on_actionFitToScreen_triggered();
	void on_actionStretchToScreen_triggered();
	void on_actionCenterToScreen_triggered();
	void on_actionVerticalCenter_triggered();
	void on_actionHorizontalCenter_triggered();

	void on_customContextMenuRequested(const QPoint &pos);

	void ScenesCurrentItemChanged(QListWidgetItem *current,
				      QListWidgetItem *prev);
	void ScenesContextMenuRequested(const QPoint &pos);
	void GridActionClicked();
	void AddSceneActionTriggered();
	void RemoveSceneActionTriggered();
	void SceneUpActionTriggered();
	void SceneDownActionTriggered();
	void SourcesContextMenuRequested(const QPoint &pos);
	void ScenesItemDoubleClicked(QListWidgetItem *item);
	void AddSourceActionTriggered();
	void RemoveSourceActionTriggered();
	void on_actionInteract_triggered();
	void SourcePropertiesActionTriggered();
	void SourceUpActionTriggered();
	void SourceDownActionTriggered();

	void on_actionMoveUp_triggered();
	void on_actionMoveDown_triggered();
	void on_actionMoveToTop_triggered();
	void on_actionMoveToBottom_triggered();

	void on_actionLockPreview_triggered();

	void on_scalingMenu_aboutToShow();

	void StreamActionTriggered();
	void RecordActionTriggered();
	void VCamButtonClicked();
	void Screenshot(OBSSource source_ = nullptr);
	void ScreenshotSelectedSource();
	void ScreenshotProgram();
	void ScreenshotScene();

	void on_actionHelpPortal_triggered();
	void on_actionWebsite_triggered();
	void on_actionDiscord_triggered();

	void on_preview_customContextMenuRequested(const QPoint &pos);
	void ProgramViewContextMenuRequested(const QPoint &pos);
	void PreviewDisabledMenu(const QPoint &pos);

	void on_actionNewSceneCollection_triggered();
	void on_actionDupSceneCollection_triggered();
	void on_actionRenameSceneCollection_triggered();
	void on_actionRemoveSceneCollection_triggered();
	void on_actionImportSceneCollection_triggered();
	void on_actionExportSceneCollection_triggered();

	void on_actionNewProfile_triggered();
	void on_actionDupProfile_triggered();
	void on_actionRenameProfile_triggered();
	void on_actionRemoveProfile_triggered(bool skipConfirmation = false);
	void on_actionImportProfile_triggered();
	void on_actionExportProfile_triggered();

	void on_actionShowSettingsFolder_triggered();
	void on_actionShowProfileFolder_triggered();

	void on_actionAlwaysOnTop_triggered();

	void on_toggleListboxToolbars_toggled(bool visible);
	void on_toggleStatusBar_toggled(bool visible);
	void on_toggleSourceIcons_toggled(bool visible);

	void TransitionsCurrentIndexChanged(int index);
	void RemoveTransitionClicked();
	void TransitionPropsClicked();
	void SetTransitionDuration(int duration);
	void on_tbar_position_valueChanged(int value);

	void on_actionShowTransitionProperties_triggered();
	void on_actionHideTransitionProperties_triggered();

	void ModeSwitchActionTriggered();

	// Source Context Buttons
	void on_sourcePropertiesButton_clicked();
	void on_sourceFiltersButton_clicked();
	void on_sourceInteractButton_clicked();

	void on_autoConfigure_triggered();
	void on_stats_triggered();

	void on_resetUI_triggered();
	void on_lockUI_toggled(bool lock);

	void PauseToggled();

	void logUploadFinished(const QString &text, const QString &error);
	void crashUploadFinished(const QString &text, const QString &error);
	void openLogDialog(const QString &text, const bool crash);

	void updateCheckFinished();

	void AddSourceFromAction();

	void MoveSceneToTop();
	void MoveSceneToBottom();

	void EditSceneName();
	void EditSceneItemName();

	void SceneNameEdited(QWidget *editor,
			     QAbstractItemDelegate::EndEditHint endHint);

	void OpenSceneFilters();
	void OpenFilters(OBSSource source = nullptr);
	void OpenProperties(OBSSource source = nullptr);
	void OpenInteraction(OBSSource source = nullptr);

	void TogglePreview();

	void OpenStudioProgramProjector();
	void OpenPreviewProjector();
	void OpenSourceProjector();
	void OpenMultiviewProjector();
	void OpenSceneProjector();

	void OpenStudioProgramWindow();
	void OpenPreviewWindow();
	void OpenSourceWindow();
	void OpenMultiviewWindow();
	void OpenSceneWindow();

	void StackedMixerAreaContextMenuRequested();

	void ResizeOutputSizeOfSource();

public slots:
	void on_actionResetTransform_triggered();

	bool StreamingActive();
	bool RecordingActive();
	bool ReplayBufferActive();
	bool VirtualCamActive();

	inline void UpdateContextBar() { centralWidget->UpdateContextBar(); }
	void UpdateContextBarDeferred(bool force = false);

public:
	explicit OBSBasic(QWidget *parent = 0);
	virtual ~OBSBasic();

	virtual void OBSInit() override;

	virtual config_t *Config() const override;

	virtual int GetProfilePath(char *path, size_t size,
				   const char *file) const override;

	static void InitBrowserPanelSafeBlock();

private:
	std::unique_ptr<Ui::OBSBasic> ui;
	QPointer<ads::CDockManager> dockManager;

signals:
	// Transitions signals
	void CurrentTransitionChanged(bool has_duration, bool has_config);
	void TransitionPropsEnabled(bool enabled);
	void TransitionDurationChanged(int duration);

	// Streaming signals
	void StreamingStarting(bool broadcast_auto_start);
	void StreamingStartAborted();
	void StreamingStarted(bool with_delay = false);
	void StreamingStopping();
	void StreamingStopAborted();
	void StreamingStopped(bool with_delay = false);

	// Broadcast signals
	void BroadcastActionEnabled();
	void BroadcastReady();
	void BroadcastStreamStarted(bool auto_stop);
	void BroadcastStartAborted();
	void BroadcastStopAborted();
	void BroadcastFlowStateChanged(bool enabled, bool ready);

	//Recording signals
	void RecordingStartAborted();
	void RecordingStarted(bool pausable = false);
	void RecordingPaused();
	void RecordingUnpaused();
	void RecordingStopping();
	void RecordingStopAborted();
	void RecordingStopped();

	// Replay buffer signals
	void ReplayBufferEnabled();
	void ReplayBufferDisabled();
	void ReplayBufferStartAborted();
	void ReplayBufferStarted();
	void ReplayBufferStopping2();
	void ReplayBufferStopped();

	// Virtual camera signals
	void VirtualCamEnabled();
	void VirtualCamStartAborted();
	void VirtualCamStarted();
	void VirtualCamStopped();

	// Studio mode signal
	void PreviewProgramModeChanged(bool enabled);
};

class SceneRenameDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:
	SceneRenameDelegate(QObject *parent);
	virtual void setEditorData(QWidget *editor,
				   const QModelIndex &index) const override;

protected:
	virtual bool eventFilter(QObject *editor, QEvent *event) override;
};
