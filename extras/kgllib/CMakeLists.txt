add_definitions(${QT_DEFINITIONS} -Wall)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../core/kgllib
        ${CMAKE_CURRENT_BINARY_DIR}/../../core/kgllib
        ${EIGEN2_INCLUDE_DIR}
        )

set(kgllib-extras_SRCS
        hdrglwidget.cpp
        hdrglwidgetcontrol.cpp
        modelloader.cpp
        rendertarget.cpp
        shapes.cpp
        simpleterrain.cpp
        trackball.cpp
        widgetproxy.cpp
        )
qt5_wrap_ui(kgllib-extras_UIS_H hdrglwidgetcontrol.ui)
#qt5_automoc(${kgllib-extras_SRCS})

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_library(kgllib-extras SHARED ${kgllib-extras_SRCS} ${kgllib-extras_UIS_H})

target_link_libraries(kgllib-extras kgllib)
set_target_properties(kgllib-extras PROPERTIES
        VERSION ${KGLLIB_LIB_VERSION}
        SOVERSION ${KGLLIB_LIB_SOVERSION}
        DEFINE_SYMBOL MAKE_KGLLIB_EXTRAS_LIB)

install(FILES
        hdrglwidget.h
        hdrglwidgetcontrol.h
        modelloader.h
        rendertarget.h
        shapes.h
        simpleterrain.h
        trackball.h
        widgetproxy.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/kgllib
        )
install(TARGETS kgllib-extras ${INSTALL_TARGETS_DEFAULT_ARGS})
