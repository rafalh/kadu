project (ext_sound)

cmake_minimum_required (VERSION 3.0)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	ext-sound-module.cpp
	ext-sound-plugin-modules-factory.cpp
	ext-sound-plugin-object.cpp
	external-player.cpp
)

set (CONFIGURATION_FILES
	data/configuration/ext_sound.ui
)

kadu_plugin (ext_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES sound
)
