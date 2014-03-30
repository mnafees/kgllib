configure_file(kgllib_version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kgllib_version.h)

add_definitions(${QT_DEFINITIONS} -Wall)
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${QT_INCLUDE_DIR}
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTGUI_INCLUDE_DIR}
        ${QT_QTOPENGL_INCLUDE_DIR}
        ${EIGEN2_INCLUDE_DIR}
        )

set(kgllib_SRCS
        kgllib.cpp
        renderer.cpp
        texture.cpp
        shader.cpp
        program.cpp
        batch.cpp
        camera.cpp
        fpscounter.cpp
        glwidget.cpp
        mesh.cpp
        textrenderer.cpp
        geometrybuffer.cpp
        kgllib_version.cpp
        )
qt4_automoc(${kgllib_SRCS})

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_library(kgllib SHARED ${kgllib_SRCS})

target_link_libraries(kgllib ${OPENGL_gl_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${GLEW_GLEW_LIBRARY})
set_target_properties(kgllib PROPERTIES
        VERSION ${KGLLIB_LIB_VERSION}
        SOVERSION ${KGLLIB_LIB_SOVERSION}
        DEFINE_SYMBOL MAKE_KGLLIB_LIB)

install(FILES
        kgllib.h
        kgllib_export.h
        renderer.h
        texture.h
        shader.h
        program.h
        batch.h
        camera.h
        fpscounter.h
        glwidget.h
        mesh.h
        textrenderer.h
        geometrybuffer.h
        ${CMAKE_CURRENT_BINARY_DIR}/kgllib_version.h

        DESTINATION ${INCLUDE_INSTALL_DIR}/kgllib
        )
install(TARGETS kgllib ${INSTALL_TARGETS_DEFAULT_ARGS})
