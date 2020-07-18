project (pcspeaker)

cmake_minimum_required (VERSION 3.0)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	pcspeaker-configuration-widget.cpp
	pcspeaker-module.cpp
	pcspeaker-notifier.cpp
	pcspeaker-plugin-modules-factory.cpp
	pcspeaker-plugin-object.cpp
)

set (LIBRARIES "")

if (UNIX AND NOT APPLE)
	find_package (X11 REQUIRED)
	if (NOT X11_FOUND)
		message (SEND_ERROR "Could NOT find X11")
	endif ()

	include_directories (${X11_INCLUDE_DIR})
	set (LIBRARIES ${X11_X11_LIB})
endif ()

kadu_plugin (pcspeaker
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES ${LIBRARIES}
)
