project (antistring)

cmake_minimum_required (VERSION 3.0)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	antistring-configuration.cpp
	antistring-configuration-ui-handler.cpp
	antistring-message-filter.cpp
	antistring-module.cpp
	antistring-notification-service.cpp
	antistring-plugin-modules-factory.cpp
	antistring-plugin-object.cpp
)

set (CONFIGURATION_FILES
	configuration/antistring.ui
)

set (DATA_FILES
	data/ant_conditions.conf
)

kadu_plugin (antistring
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
