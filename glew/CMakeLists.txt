add_definitions(-fPIC)

add_library(glew STATIC glew.c)

install(FILES
        GL/glew.h
        GL/glxew.h
        GL/wglew.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/kgllib/GL
        )
