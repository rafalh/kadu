set (core_SRCS
	application.cpp
	core.cpp
	core-module.cpp
	crash-aware-object.cpp
	injected-factory.cpp
	injector-provider.cpp
	myself.cpp
	session-service.cpp
	scheduler.cpp
	version-service.cpp
)

set (core_MOC_SRCS
	core.h
)

kadu_subdirectory (core "${core_SRCS}" "${core_MOC_SRCS}" "")
