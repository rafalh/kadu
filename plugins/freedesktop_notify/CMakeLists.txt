project (freedesktop_notify)

cmake_minimum_required (VERSION 3.0)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	freedesktop-notifier.cpp
	freedesktop-notify-module.cpp
	freedesktop-notify-plugin-modules-factory.cpp
	freedesktop-notify-plugin-object.cpp
)

set (CONFIGURATION_FILES
	configuration/freedesktop_notify.ui
)

kadu_plugin (freedesktop_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
