project (autoaway)

cmake_minimum_required (VERSION 3.0)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	autoaway-configuration-ui-handler.cpp
	autoaway-module.cpp
	autoaway-plugin-modules-factory.cpp
	autoaway-plugin-object.cpp
	autoaway-status-changer.cpp
	autoaway.cpp
)

set (CONFIGURATION_FILES
	configuration/autoaway.ui
)

kadu_plugin (autoaway
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES idle
)
