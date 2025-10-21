include_directories(
    ${PROJECT_SOURCE_DIR}/Dependencies/stb/include
    ${PROJECT_SOURCE_DIR}/Dependencies/GLFW/include
    ${PROJECT_SOURCE_DIR}/Dependencies/GLAD/include
)

set(SGE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SGE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SGE_GRAPHIC_SRCS
    ${SGE_SRC_DIR}/buffer.cpp
    ${SGE_SRC_DIR}/shader.cpp
    ${SGE_SRC_DIR}/buffer_layout.cpp
    ${SGE_SRC_DIR}/texture.cpp
)
set(SGE_GRAPHIC_INCLUDES
    ${PROJECT_SOURCE_DIR}/Dependencies/GLFW/include
    ${PROJECT_SOURCE_DIR}/Dependencies/GLAD/include
    ${PROJECT_SOURCE_DIR}/Dependencies/stb/include
    ${SGE_INCLUDE_DIR}
)

add_library(SGE_GRAPHIC_LIBRARIES
    ${SGE_GRAPHIC_SRCS}
    ${PROJECT_SOURCE_DIR}/Dependencies/stb/src/stb_image.cpp
)
target_include_directories(SGE_GRAPHIC_LIBRARIES PRIVATE ${SGE_GRAPHIC_INCLUDES})

add_library(SGE_LIBRARIES INTERFACE)
target_link_libraries(SGE_LIBRARIES INTERFACE SGE_GRAPHIC_LIBRARIES)