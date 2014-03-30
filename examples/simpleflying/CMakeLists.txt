include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

set(kgllib-test_SRCS
        main.cpp
        testwidget.cpp
        )
#qt4_automoc(${kgllib-test_SRCS})
qt5_add_resources(kgllib-test_RESOURCES flying.qrc)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(kgllib-test ${kgllib-test_SRCS} ${kgllib-test_RESOURCES})

target_link_libraries(kgllib-test kgllib-extras)
