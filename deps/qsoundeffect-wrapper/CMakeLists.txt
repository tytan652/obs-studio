# On Windows, QtMultimedia 6 library is linked against DLLs that are not present
# by default on Windows KN/N editions.
#
# Those DLLs on inside the Media Feature Pack that Windows KN/N users should
# install.
#
# Linking OBS Studio against QtMultimedia will make Windows builds fail to run
# with a cryptic missing DLL error message on Windows KN/N if the pack is not
# installed.
#
# To avoid this error and not force users to install the pack, this module which
# wrap QSoundEffect is dynamically loaded, so if the Media Feature Pack is not
# installed only the module will fail to load not OBS. So OBS will just run
# without the sound notifications feature from accesibility features.

project(qsoundeffect-wrapper)

add_library(qsoundeffect-wrapper MODULE)

target_sources(qsoundeffect-wrapper PRIVATE qsoundeffect.cpp)

find_qt(VERSION ${QT_VERSION} COMPONENTS Core Multimedia)

target_link_libraries(qsoundeffect-wrapper PRIVATE Qt::Core Qt::Multimedia)

target_include_directories(
  qsoundeffect-wrapper PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                              $<INSTALL_INTERFACE:${OBS_INCLUDE_DESTINATION}>)

set_target_properties(qsoundeffect-wrapper PROPERTIES FOLDER "deps")

if(OS_WINDOWS)
  set(MODULE_DESCRIPTION
      "Module to dynamically load QSoundEffect from QtMultimedia, to avoid issue with Windows N"
  )
  configure_file(${CMAKE_SOURCE_DIR}/cmake/bundle/windows/obs-module.rc.in
                 qsoundeffect-wrapper.rc)

  target_sources(qsoundeffect-wrapper PRIVATE qsoundeffect-wrapper.rc)
endif()

setup_binary_target(qsoundeffect-wrapper)
