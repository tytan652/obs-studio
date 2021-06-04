function(install_obs_plugin_with_data)
  message(
    DEPRECATION
      "OBS: The install_obs_plugin_with_data command is deprecated and will be removed soon - use 'setup_plugin_target' instead."
  )
  _install_obs_plugin_with_data(${ARGV})
endfunction()

function(install_obs_plugin)
  message(
    DEPRECATION
      "OBS: The install_obs_plugin command is deprecated and will be removed soon - use 'setup_plugintarget' instead."
  )
  _install_obs_plugin(${ARGV})
endfunction()

function(install_obs_datatarget)
  message(
    DEPRECATION
      "OBS: The install_obs_datatarget function is deprecated and will be removed soon - use 'setup_target_resources' instead."
  )
  _install_obs_datatarget(${ARGV})
endfunction()

function(__deprecated_var var access)
  if(access STREQUAL "READ_ACCESS")
    message(DEPRECATION "OBS: The variable '${var}' is deprecated!")
  endif()
endfunction()

function(__deprecated_feature var access)
  if(access STREQUAL "UNKNOWN_READ_ACCESS")
    message(
      DEPRECATION
        "OBS: The feature enabled by '${var}' is deprecated and will soon be removed from OBS."
    )
  endif()
endfunction()

set(_DEPRECATED_VARS
    zlibPath
    vulkanPath
    SwigPath
    PythonPath
    mbedtlsPath
    LuajitPath
    x264Path
    VLCPath
    speexPath
    rnnoisePath
    LibfdkPath
    curlPath
    JanssonPath
    FFmpegPath
    DepsDir
    DISABLE_UI
    UI_ENABLED
    UNIX_STRUCTURE
    UPDATE_SPARKLE
    LIBOBS_PREFER_IMAGEMAGICK
    DEBUG_FFMPEG_MUX
    ENABLE_WINMF
    USE_QT_LOOP
    SHARED_TEXTURE_SUPPORT_ENABLED
    BROWSER_PANEL_SUPPORT_ENABLED
    BROWSER_LEGACY)

foreach(_DEPRECATED_VAR ${_DEPRECATED_VARS})
  variable_watch(_DEPRECATED_VAR __deprecated_var)
endforeach()

variable_watch(FTL_FOUND __deprecated_feature)
