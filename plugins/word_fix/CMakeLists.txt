project (word_fix)

cmake_minimum_required (VERSION 3.0)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	word-fix-configuration-ui-handler.cpp
	word-fix-formatted-string-visitor.cpp
	word-fix-module.cpp
	word-fix-plugin-modules-factory.cpp
	word-fix-plugin-object.cpp
	word-fix.cpp
)

set (CONFIGURATION_FILES
	configuration/word_fix.ui
)

set (DATA_FILES
	data/wf_default_list.data
)

kadu_plugin (word_fix
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
