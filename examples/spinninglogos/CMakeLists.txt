include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

set(spinninglogos_SRCS
        main.cpp
        logos.cpp
        )
#qt4_automoc(${spinninglogos_SRCS})
qt5_add_resources(spinninglogos_RESOURCES logos.qrc)

if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


add_executable(spinninglogos ${spinninglogos_SRCS} ${spinninglogos_RESOURCES})

target_link_libraries(spinninglogos kgllib-extras)
