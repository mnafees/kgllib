include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

set(earth_SRCS
        main.cpp
        earth.cpp
        )
qt4_automoc(${earth_SRCS})
qt4_add_resources(earth_RESOURCES earth.qrc)

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)

add_executable(earth ${earth_SRCS} ${earth_RESOURCES})

target_link_libraries(earth kgllib-extras)
