include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

set(widgets_SRCS
        main.cpp
        mainwidget.cpp
        demoglwidget.cpp
        )
qt4_automoc(${widgets_SRCS})

if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(widgets ${widgets_SRCS})

target_link_libraries(widgets kgllib-extras)
