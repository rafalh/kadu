project (Kadu)

cmake_minimum_required (VERSION 3.0)

# Options (some of them)

option (ENABLE_DEVELOPER_BUILD "Turn on some features helpful during development process (has nothing to do with debugging symbols)" OFF)
option (INSTALL_SDK "Install SDK (API headers, CMake modules, MSVC program libraries)" ON)
option (WINDOWS_ARCHITECTURE "Architecture for Windows binaries." "x86")

# Global CMake variables

list (INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

if (NOT DEFINED CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	if (ENABLE_DEVELOPER_BUILD)
		set (default Debug)
	else ()
		set (default RelWithDebInfo)
	endif ()

	set (CMAKE_BUILD_TYPE "${default}" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# C++14 support, warnings and other flags

set (DEFINITIONS QT_NO_CAST_TO_ASCII QT_DISABLE_DEPRECATED_BEFORE=0x040900)
list (APPEND DEFINITIONS KADU_EXPORT_TESTS)
set (DEFINITIONS_DEBUG DEBUG_ENABLED DEBUG_OUTPUT_ENABLED)

if (MSVC)
	set (COMMON_COMPILE_FLAGS "/MP /Zc:wchar_t-")
	list (APPEND DEFINITIONS _CRT_SECURE_NO_WARNINGS=1)
else ()
	set (COMMON_COMPILE_FLAGS "-Wall -Wextra -Wundef -Wunused -Wuninitialized -Wcast-align -Wpointer-arith -fno-common")
	set (C_FLAGS "-Wwrite-strings")
	set (CXX_FLAGS "-Woverloaded-virtual -Wnon-virtual-dtor ${CXX_FLAGS}")
	set (CMAKE_CXX_STANDARD 14)

	if (NOT WIN32)
		set (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -fvisibility=hidden")
		set (CXX_FLAGS "${CXX_FLAGS} -fvisibility-inlines-hidden")
	endif ()

	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -U__GNUC_MINOR__ -D__GNUC_MINOR__=8")
	endif ()

	if (ENABLE_DEVELOPER_BUILD)
		# -pipe can speed up the build
		# -ftrapv generates trap on signed integer overflow, which is undefined by C/C++
		# -fno-omit-frame-pointer gives potentially better stack traces at the cost of negligible performance drop
		set (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Werror -Wno-deprecated -pipe -ftrapv -fno-omit-frame-pointer")

		if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
			set (SANITIZE_OPTIONS "-fsanitize=integer -fsanitize=undefined-trap -fsanitize=unsigned-integer-overflow")
			set (COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} ${SANITIZE_OPTIONS} -fsanitize-blacklist=${CMAKE_SOURCE_DIR}/sanitize-blacklist.txt")
			set (LINK_FLAGS "${LINK_FLAGS} ${SANITIZE_OPTIONS}")
			# Clang is links its sanitizer libraries only to executables,
			# even though they are actually needed all over the code. :-(
			set (ALLOW_UNDEFINED ON)
		endif ()

		if (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "GNU")
			# -z now checks there are no unresolved symbols at executalbe/library load time, instead of that specific symbol load time
			set (LINK_FLAGS "${LINK_FLAGS} -Wl,-z,now -Wl,--as-needed")
		endif ()
	endif ()

	if (NOT ALLOW_UNDEFINED AND (CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "GNU"))
		set (LINK_FLAGS "${LINK_FLAGS} -Wl,--no-undefined")
	endif ()
endif ()

## CCache is cool stuff to improve compilation time
find_program (CCACHE_FOUND ccache)
if (CCACHE_FOUND)
	set_property (GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property (GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

# Look above, DEBUG_ENABLED is always enabled in Debug configuration.
# Enable it in all configurations when ENABLE_DEVELOPER_BUILD is on.
if (ENABLE_DEVELOPER_BUILD)
	list (APPEND DEFINITIONS DEBUG_ENABLED)
endif ()

# The same notes as DEBUG_ENABLED concern also DEBUG_OUTPUT_ENABLED.
# Additionally, Windows needs DEBUG_OUTPUT_ENABLED because we always
# install kadu_c.exe with console output.
if (ENABLE_DEVELOPER_BUILD OR WIN32)
	list (APPEND DEFINITIONS DEBUG_OUTPUT_ENABLED)
endif ()

# Version information

file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" KADU_VERSION LIMIT_COUNT 1)

find_package (Git QUIET)
if (GIT_FOUND)
	execute_process (COMMAND "${GIT_EXECUTABLE}" describe --tags --exact-match HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE result
		OUTPUT_VARIABLE git_tag
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (NOT (result STREQUAL 0) OR NOT (git_tag STREQUAL KADU_VERSION))
		execute_process (COMMAND "${GIT_EXECUTABLE}" rev-parse --short --verify HEAD
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
			RESULT_VARIABLE result
			OUTPUT_VARIABLE git_commit
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		if (result STREQUAL 0)
			set (KADU_VERSION "${KADU_VERSION}-g${git_commit}")
		endif ()
	endif ()
endif ()

string (REGEX REPLACE "-.*" "" simple_version ${KADU_VERSION})

if (NOT KADU_VERSION STREQUAL simple_version)
	set (KADU_PRERELEASE TRUE)
endif ()

# if ((NOT DEFINED NETWORK_IMPLEMENTATION AND NOT Qt5Core_FOUND) OR "${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
# 	find_package (QNtrack 010)
# endif ()

if (NOT DEFINED NETWORK_IMPLEMENTATION)
#	if (QNTRACK_FOUND)
#		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: ntrack")
#		set (NETWORK_IMPLEMENTATION "ntrack")
#	else ()
		message (STATUS "No NETWORK_IMPLEMENTATION defined. Autodetected implementation: Qt")
		set (NETWORK_IMPLEMENTATION "Qt")
#	endif ()
endif ()

set (NETWORK_IMPLEMENTATION "${NETWORK_IMPLEMENTATION}" CACHE STRING "Select network-aware implementation (Qt/ntrack/dummy)" FORCE)

if ("${NETWORK_IMPLEMENTATION}" STREQUAL "ntrack")
	list (APPEND ADDITIONAL_LIBKADU_LIBRARIES ${QNTRACK_LIBRARIES})
endif ()
# The rest of NETWORK_IMPLEMENTATION stuff is handled in network/CMakeLists.txt.

# injeqt
find_package (PkgConfig)
pkg_check_modules (INJEQT REQUIRED injeqt>=1.1)
set (INJEQT_DEFINITIONS ${INJEQT_CFLAGS_OTHER})
include_directories (${INJEQT_INCLUDEDIR})
link_directories (${INJEQT_LIBRARY_DIRS})

list (APPEND ADDITIONAL_LIBKADU_LIBRARIES ${INJEQT_LIBRARIES})
list (APPEND ADDITIONAL_LIBRARIES ${INJEQT_LIBRARIES})

# Tests
find_package (Qt5Test REQUIRED)

function (kadu_add_plugin_test plugin_name name_)
	set (test_file "${name_}.test")
	string (REPLACE "/" "-" test_name ${test_file})
	list (APPEND UNIT_TEST_TARGETS ${test_name} PARENT_SCOPE)

	add_executable (${test_name} "${test_file}.cpp")
	kadu_set_flags (${test_name})
	if (NOT MSVC)
		set_property (TARGET ${test_name} APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-error")
	endif ()
	add_test (
		NAME ${test_name}
		COMMAND ${test_name})

	qt5_use_modules (${test_name} LINK_PRIVATE Core Gui Network Qml Quick QuickWidgets Test WebKit WebKitWidgets Widgets Xml)

	# Add libkadu after the plugin so that --as-needed won't drop anything
	target_link_libraries (${test_name} LINK_PRIVATE libkadu ${plugin_name} ${ADDITIONAL_LIBRARIES})
endfunction ()

function (kadu_add_test name_)
	kadu_add_plugin_test ("" "${name_}")
endfunction ()

# Installation paths.

set (INSTALL_SDK_DIR "" CACHE PATH "SDK installation root")
mark_as_advanced (INSTALL_SDK_DIR)
if (NOT INSTALL_SDK_DIR)
	set (INSTALL_SDK_DIR "${CMAKE_INSTALL_PREFIX}/sdk")
	set (install_sdk_dir_set_to_default TRUE)
endif ()

include (GNUInstallDirs)

set (INSTALL_BIN_DIR "${CMAKE_INSTALL_FULL_BINDIR}")
set (INSTALL_LIB_DIR "${CMAKE_INSTALL_FULL_LIBDIR}")
set (INSTALL_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/kadu")
set (INSTALL_PLUGINS_DATA_DIR "${INSTALL_DATA_DIR}/plugins")
set (INSTALL_PLUGINS_LIB_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/kadu/plugins")
set (INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}/kadu")
set (INSTALL_CMAKE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/cmake/Kadu")

if (install_sdk_dir_set_to_default)
	set (INSTALL_SDK_DIR "${INSTALL_DATA_DIR}/sdk")
endif ()

if (WIN32)
	set (KADU_DATADIR_RELATIVE_TO_BIN "./")
	set (KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN "./plugins/")
else ()
	file (RELATIVE_PATH KADU_DATADIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${INSTALL_DATA_DIR}")
	file (RELATIVE_PATH KADU_PLUGINS_LIBDIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${INSTALL_PLUGINS_LIB_DIR}")
endif ()

if (UNIX AND NOT APPLE)
	set (DESKTOP_FILE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/applications")
	set (DESKTOP_FILE_NAME kadu.desktop)

	file (RELATIVE_PATH KADU_DESKTOP_FILE_DIR_RELATIVE_TO_BIN "${INSTALL_BIN_DIR}" "${DESKTOP_FILE_DIR}")
endif ()

set (install_vars
	INSTALL_SDK_DIR INSTALL_BIN_DIR INSTALL_LIB_DIR INSTALL_DATA_DIR INSTALL_PLUGINS_DATA_DIR
	INSTALL_PLUGINS_LIB_DIR INSTALL_INCLUDE_DIR INSTALL_CMAKE_DIR
)
foreach (install_var ${install_vars})
	if (NOT DEFINED ${install_var})
		message (FATAL_ERROR "Variable ${install_var} does not exist")
	else ()
		if (IS_ABSOLUTE "${${install_var}}")
			file (TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" sane_install_prefix)
			file (TO_CMAKE_PATH "${${install_var}}" sane_install_path)
			if (sane_install_path STREQUAL sane_install_prefix)
				set (RELATIVE_${install_var} "")
			else ()
				string (REPLACE "${sane_install_prefix}/" "" RELATIVE_${install_var} "${sane_install_path}")
			endif ()
			if (RELATIVE_${install_var} STREQUAL "")
				set (RELATIVE_${install_var} ".")
			endif ()

			if (IS_ABSOLUTE "${RELATIVE_${install_var}}")
				message (FATAL_ERROR "${install_var} (${${install_var}}) is not under install prefix.")
			endif ()
		else ()
			set (RELATIVE_${install_var} "${${install_var}}")
		endif()
	endif ()
endforeach ()

set (KADU_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set (path_vars KADU_INSTALL_PREFIX)
if (UNIX AND NOT APPLE)
	list (APPEND path_vars DESKTOP_FILE_DIR)
endif ()

# Generate CMake configuration files

include (CMakePackageConfigHelpers)

macro (kadu_configure_package_config_file _in _out _mode)
	if ("${_mode}" STREQUAL BUILD_TREE)
		foreach (path_var ${path_vars})
			set (PACKAGE_${path_var} "${${path_var}}")
		endforeach ()

		set (PACKAGE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}")
		set (PACKAGE_SDK_DIR "${CMAKE_SOURCE_DIR}")

		configure_package_config_file ("${_in}" "${_out}"
			INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
			NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
		)
	elseif ("${_mode}" STREQUAL INSTALL_TREE)
		set (INCLUDE_DIR "${INSTALL_INCLUDE_DIR}")
		set (SDK_DIR "${INSTALL_SDK_DIR}")
		list (APPEND path_vars INCLUDE_DIR SDK_DIR)

		configure_package_config_file ("${_in}" "${_out}"
			INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
			PATH_VARS ${path_vars}
			NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
		)
	endif ()
endmacro ()

# This one is only for usable within the build tree. We want to actually
# set some variables in there from within the subdirectories, so we generated
# the intallation version later.
kadu_configure_package_config_file (cmake/KaduConfig.cmake.in
	"${CMAKE_BINARY_DIR}/KaduConfig.cmake" BUILD_TREE)

write_basic_package_version_file ("${CMAKE_BINARY_DIR}/KaduConfigVersion.cmake"
	VERSION ${simple_version} COMPATIBILITY AnyNewerVersion
)

# Include the generated file

set (Kadu_DIR "${CMAKE_BINARY_DIR}")
find_package (Kadu REQUIRED CONFIG)

if (NOT WIN32)
	include (CheckIncludeFiles)

	# TODO: Add libexecinfo support
	check_include_files ("execinfo.h" HAVE_EXECINFO)
	if (HAVE_EXECINFO)
		include (CheckFunctionExists)

		check_function_exists ("backtrace" HAVE_EXECINFO)
	endif ()
endif ()

include_directories (${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kadu-core)

enable_testing ()

add_subdirectory (kadu-core)
add_subdirectory (varia)
add_subdirectory (translations)

include (Plugins.cmake)
add_subdirectory (plugins)
add_subdirectory (kthxbye)
add_subdirectory (tests)

# Now write KaduConfig for installation.
if (UNIX)
	kadu_configure_package_config_file (cmake/KaduConfig.cmake.in
		"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake" INSTALL_TREE)
endif ()

if (KADU_INSTALL_SDK)
	install (FILES
		"${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/KaduConfig.cmake"
		"${CMAKE_BINARY_DIR}/KaduConfigVersion.cmake"
		cmake/KaduMacros.cmake
		DESTINATION "${KADU_INSTALL_CMAKE_DIR}"
	)

	install (EXPORT KaduTargets DESTINATION "${KADU_INSTALL_CMAKE_DIR}")
endif ()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake IMMEDIATE @ONLY)
add_custom_target (uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

configure_file (varia/scripts/nsis_installer.nsi.in "${CMAKE_CURRENT_BINARY_DIR}/nsis_installer.nsi" ESCAPE_QUOTES @ONLY)

message (STATUS "Kadu (version: ${KADU_VERSION}) will be built:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
	message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")
endif ()

install (FILES AUTHORS AUTHORS.html HISTORY README THANKS ChangeLog ChangeLog.OLD-PL DESTINATION "${KADU_INSTALL_DATA_DIR}")
# For Win32 we use GPL3, and this COPYING is GPL2
if (WIN32)
	install (FILES COPYING.WIN32 DESTINATION "${KADU_INSTALL_DATA_DIR}")
else ()
	install (FILES COPYING.GPL2 COPYING.LGPL2.1 DESTINATION "${KADU_INSTALL_DATA_DIR}")
endif ()
