project (screenshot)

cmake_minimum_required (VERSION 3.0)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	configuration/gui/screenshot-configuration-ui-handler.cpp
	configuration/screen-shot-configuration.cpp
	gui/actions/screenshot-action.cpp
	gui/actions/screenshot-actions.cpp
	gui/graphics-items/handler-rect-item.cpp
	gui/graphics-items/selection-frame-item.cpp
	gui/widgets/crop-image-widget.cpp
	gui/widgets/screenshot-tool-box.cpp
	gui/widgets/screenshot-widget.cpp
	pixmap-grabber.cpp
	screen-shot-saver.cpp
	screenshot-module.cpp
	screenshot-notification-service.cpp
	screenshot-plugin-modules-factory.cpp
	screenshot-plugin-object.cpp
	screenshot-taker.cpp
	screenshot.cpp
)

set (CONFIGURATION_FILES
	data/configuration/screenshot.ui
)

if (UNIX AND NOT APPLE)
	set (PLUGIN_ADDITIONAL_QT_MODULES X11Extras)
	find_package (X11 REQUIRED COMPONENTS Xext Xshape)
	if (X11_Xshape_FOUND)
		message (STATUS "Found required X11 libraries: ${X11_X11_LIB};${X11_Xext_LIB}")
	else ()
		message (SEND_ERROR "Could NOT find X11 Xshape")
	endif ()

	include_directories (${X11_INCLUDE_DIR})
	list (APPEND LIBRARIES ${X11_X11_LIB} ${X11_Xext_LIB})
endif ()

if (WIN32)
	set (PLUGIN_ADDITIONAL_QT_MODULES WinExtras)
endif ()

kadu_plugin (screenshot
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${LIBRARIES}
	PLUGIN_ADDITIONAL_QT_MODULES ${PLUGIN_ADDITIONAL_QT_MODULES}
)
