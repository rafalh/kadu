project (winamp_mediaplayer)

cmake_minimum_required (VERSION 3.0)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	winamp-module.cpp
	winamp-plugin-modules-factory.cpp
	winamp-plugin-object.cpp
	winamp.cpp
)

if (MINGW)
	set_property (SOURCE winamp.cpp APPEND_STRING PROPERTY COMPILE_FLAGS " -fno-strict-aliasing")
endif ()

kadu_plugin (winamp_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_DEPENDENCIES mediaplayer
)
