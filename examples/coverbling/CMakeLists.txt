include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )

set(coverbling_SRCS
        main.cpp
        coverbling.cpp
        )
#qt4_automoc(${coverbling_SRCS})

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(coverbling ${coverbling_SRCS})

target_link_libraries(coverbling kgllib-extras)
