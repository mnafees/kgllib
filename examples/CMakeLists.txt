add_definitions(${QT_DEFINITIONS} -Wall)
include_directories(
        ${QT_INCLUDE_DIR}
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTGUI_INCLUDE_DIR}
        ${QT_QTOPENGL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../core
        ${CMAKE_CURRENT_SOURCE_DIR}/../core/kgllib
        ${CMAKE_CURRENT_BINARY_DIR}/../core/kgllib
        ${CMAKE_CURRENT_SOURCE_DIR}/../extras
        ${EIGEN2_INCLUDE_DIR}
        )


add_subdirectory(coverbling)
add_subdirectory(earth)
add_subdirectory(simpleflying)
add_subdirectory(spinninglogos)
add_subdirectory(widgets)
