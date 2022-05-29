# Helper function to set up runtime or library targets
function(setup_binary_target target)
  set_target_properties(
    ${target}
    PROPERTIES
      BUILD_RPATH
      "${OBS_OUTPUT_DIR}/$<CONFIG>/${OBS_EXECUTABLE_DESTINATION}$<$<BOOL:LINUX_PORTABLE>:/${_ARCH_SUFFIX}bit>"
  )

  _setup_binary_target(${target})
endfunction()

# Helper function to export target to build and install tree Allows usage of
# `find_package(libobs)` by other build trees
function(export_target target)
  set(CMAKE_EXPORT_PACKAGE_REGISTRY OFF)

  install(
    TARGETS ${target}
    EXPORT ${target}Targets
    RUNTIME DESTINATION ${OBS_EXECUTABLE_DESTINATION} COMPONENT obs_libraries
    LIBRARY DESTINATION ${OBS_LIBRARY_DESTINATION} COMPONENT obs_libraries
    ARCHIVE DESTINATION ${OBS_LIBRARY_DESTINATION} COMPONENT obs_libraries
    INCLUDES
    DESTINATION ${OBS_INCLUDE_DESTINATION}
    PUBLIC_HEADER DESTINATION ${OBS_INCLUDE_DESTINATION}
                  COMPONENT obs_libraries)

  include(GenerateExportHeader)
  generate_export_header(${target} EXPORT_FILE_NAME
                         ${CMAKE_CURRENT_BINARY_DIR}/${target}_EXPORT.h)

  target_sources(${target}
                 PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${target}_EXPORT.h)

  set(TARGETS_EXPORT_NAME "${target}Targets")
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${target}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${target}Config.cmake
    INSTALL_DESTINATION ${OBS_CMAKE_DESTINATION}/${target}
    PATH_VARS OBS_PLUGIN_DESTINATION OBS_DATA_DESTINATION)

  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${target}ConfigVersion.cmake
    VERSION ${OBS_VERSION_CANONICAL}
    COMPATIBILITY SameMajorVersion)

  export(
    EXPORT ${target}Targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGETS_EXPORT_NAME}.cmake
    NAMESPACE OBS::)

  export(PACKAGE "${target}")

  install(
    EXPORT ${TARGETS_EXPORT_NAME}
    FILE ${TARGETS_EXPORT_NAME}.cmake
    NAMESPACE OBS::
    DESTINATION ${OBS_CMAKE_DESTINATION}/${target}
    COMPONENT obs_libraries)

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${target}Config.cmake
          ${CMAKE_CURRENT_BINARY_DIR}/${target}ConfigVersion.cmake
    DESTINATION ${OBS_CMAKE_DESTINATION}/${target}
    COMPONENT obs_libraries)

  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig")
    export_target_pkgconf(${target})
  endif()
endfunction()

# Helper function to build pkgconfig file for target
function(export_target_pkgconf target)
  get_target_property(_TARGET_DEPENDENCIES ${target} INTERFACE_LINK_LIBRARIES)
  get_target_property(_TARGET_DEFINITIONS ${target}
                      INTERFACE_COMPILE_DEFINITIONS)
  get_target_property(_TARGET_OPTIONS ${target} INTERFACE_LINK_OPTIONS)

  foreach(_LIBRARY IN LISTS _TARGET_DEPENDENCIES)
    get_target_property(_LINK_LIBRARY ${_LIBRARY} INTERFACE_LINK_LIBRARIES)
    get_target_property(_LINK_DEFINITIONS ${_LIBRARY}
                        INTERFACE_COMPILE_DEFINITIONS)
    list(APPEND _LINKED_LIBRARIES "${_LINK_LIBRARY}")

    if(NOT "${_LINK_DEFINITIONS}" STREQUAL "_LINK_DEFINITIONS-NOTFOUND")
      list(APPEND _LINKED_DEFINITIONS "${_LINK_DEFINITIONS}")
    endif()
  endforeach()

  string(REPLACE ";" " " _LINKED_LIBRARIES "${_LINKED_LIBRARIES}")
  string(REPLACE ";" " " _LINKED_DEFINITIONS "${_LINKED_DEFINITIONS}")

  if(NOT "${_TARGET_DEFINITIONS}" STREQUAL "_TARGET_DEFINITIONS-NOTFOUND")
    list(JOIN _TARGET_DEFINITIONS "-D" _TARGET_DEFINITIONS)
    set(_TARGET_DEFINITIONS "-D${_TARGET_DEFINITIONS}")
  else()
    set(_TARGET_DEFINITIONS "")
  endif()

  if(NOT "${_TARGET_OPTIONS}" STREQUAL "_TARGET_OPTIONS-NOTFOUND")
    list(JOIN _TARGET_OPTIONS "-" _TARGET_OPTIONS)
    set(_TARGET_OPTIONS "-${_TARGET_OPTIONS}")
  else()
    set(_TARGET_OPTIONS "")
  endif()

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/${target}.pc.in"
                 "${target}.pc" @ONLY)

  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${target}.pc"
          DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endfunction()

# Helper function to install header files
function(install_headers target)
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
    DESTINATION ${OBS_INCLUDE_DESTINATION}
    COMPONENT obs_libraries
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "obs-hevc.h" EXCLUDE
    PATTERN "*-windows.h" EXCLUDE
    PATTERN "audio-monitoring" EXCLUDE
    PATTERN "util/apple" EXCLUDE
    PATTERN "util/windows" EXCLUDE
    PATTERN "cmake" EXCLUDE
    PATTERN "pkgconfig" EXCLUDE
    PATTERN "data" EXCLUDE)

  if(ENABLE_PULSEAUDIO)
    install(
      FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/audio-monitoring/pulse/pulseaudio-wrapper.h"
      DESTINATION "${OBS_INCLUDE_DESTINATION}/audio-monitoring/pulse/"
      COMPONENT obs_libraries)
  endif()

  if(ENABLE_HEVC)
    install(
      FILES "${CMAKE_CURRENT_SOURCE_DIR}/obs-hevc.h"
      DESTINATION "${OBS_INCLUDE_DESTINATION}"
      COMPONENT obs_libraries)
  endif()

  if(NOT EXISTS "${OBS_INCLUDE_DESTINATION}/obsconfig.h")
    install(
      FILES "${CMAKE_BINARY_DIR}/config/obsconfig.h"
      DESTINATION "${OBS_INCLUDE_DESTINATION}"
      COMPONENT obs_libraries)
  endif()
endfunction()
