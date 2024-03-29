#[=======================================================================[.rst
FindSimde
--------

FindModule for simde and the associated library

Imported Targets
^^^^^^^^^^^^^^^^

.. versionadded:: 3.0

This module defines the :prop_tgt:`IMPORTED` target ``Simde::Simde``.

Result Variables
^^^^^^^^^^^^^^^^

This module sets the following variables:

``Simde_FOUND``
  True, if the library was found.
``Simde_VERSION``
  Detected version of found simde library.

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``Simde_INCLUDE_DIR``
  Directory containing ``simde/simde-common.h``.

#]=======================================================================]

# cmake-format: off
# cmake-lint: disable=C0103
# cmake-lint: disable=C0301
# cmake-format: on

include(FindPackageHandleStandardArgs)

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_search_module(PC_Simde QUIET simde)
endif()

find_path(
  Simde_INCLUDE_DIR
  NAMES simde/simde-common.h
  HINTS ${PC_Simde_INCLUDE_DIRS}
  PATHS /usr/include /usr/local/include
  DOC "simde include directory")

if(PC_Simde_VERSION VERSION_GREATER 0)
  set(Simde_VERSION ${PC_Simde_VERSION})
elseif(EXISTS "${Simde_INCLUDE_DIR}/simde/simde-common.h")
  file(STRINGS "${Simde_INCLUDE_DIR}/simde/simde-common.h" _version_string
REGEX "^.*VERSION_(MAJOR|MINOR|MICRO)[ \t]+[0-9]+[ \t]*$")

  string(REGEX REPLACE ".*VERSION_MAJOR[ \t]+([0-9]+).*" "\\1" _version_major "${_version_string}")
  string(REGEX REPLACE ".*VERSION_MINOR[ \t]+([0-9]+).*" "\\1" _version_minor "${_version_string}")
  string(REGEX REPLACE ".*VERSION_MICRO[ \t]+([0-9]+).*" "\\1" _version_micro "${_version_string}")

set(Simde_VERSION "${_version_major}.${_version_minor}.${_version_micro}")
unset(_version_major)
unset(_version_minor)
unset(_version_micro)
else()
  if(NOT Simde_FIND_QUIETLY)
    message(AUTHOR_WARNING "Failed to find simde version.")
  endif()
  set(Simde_VERSION 0.0.0)
endif()

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin|Windows")
  set(Simde_ERROR_REASON "Ensure that obs-deps is provided as part of CMAKE_PREFIX_PATH.")
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux|FreeBSD")
  set(Simde_ERROR_REASON "Ensure simde library is available in local include paths.")
endif()

find_package_handle_standard_args(
  Simde
  REQUIRED_VARS Simde_INCLUDE_DIR
  VERSION_VAR Simde_VERSION REASON_FAILURE_MESSAGE "${Simde_ERROR_REASON}")
mark_as_advanced(Simde_INCLUDE_DIR)
unset(Simde_ERROR_REASON)

if(Simde_FOUND)
  if(NOT TARGET Simde::Simde)
    add_library(Simde::Simde INTERFACE IMPORTED)
    set_target_properties(Simde::Simde PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Simde_INCLUDE_DIR}")
  endif()
endif()

include(FeatureSummary)
set_package_properties(
  Simde PROPERTIES
  URL "https://simd-everywhere.github.io/blog/"
  DESCRIPTION
    "Implementations of SIMD instruction sets for systems which don't natively support them."
)
