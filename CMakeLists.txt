cmake_minimum_required(VERSION 3.10.0)
cmake_policy(SET CMP0072 NEW)
project(Number VERSION 0.1.0 LANGUAGES C CXX)
add_subdirectory(${CMAKE_SOURCE_DIR}/Dependencies/GLFW)
add_subdirectory(${CMAKE_SOURCE_DIR}/Core)

# Disable building GLFW examples, tests, and docs if not needed
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

# Setup 
#set(GLFW3_ROOT "${CMAKE_SOURCE_DIR}/glfw-3.4.bin.WIN64") 
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}\\CMake)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(glad "${CMAKE_SOURCE_DIR}/Dependencies/GLAD/src/glad.c")
target_include_directories(glad PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/GLAD/include")
add_library(stb_image "${CMAKE_SOURCE_DIR}/Dependencies/stb/src/stb_image.cpp")
target_include_directories(stb_image PRIVATE "${CMAKE_SOURCE_DIR}/Dependencies/stb/include")

set(APPLICATION_SRCS
    ${CMAKE_SOURCE_DIR}/Application/src/game.cpp
)
add_library(APPLICATION_LIBRARY
    ${APPLICATION_SRCS}
)

add_executable(Number 
    "${CMAKE_SOURCE_DIR}\\Application\\src\\application.cpp"
)
target_link_libraries(Number PRIVATE glfw glad stb_image OpenGL::GL
    SGE_LIBRARIES
    APPLICATION_LIBRARY
)
set(APPLICATION_DIRS
    Dependencies/GLFW/include
    Dependencies/GLAD/include
    Dependencies/stb/include
    Dependencies/nlohmann

    ${PROJECT_SOURCE_DIR}
    Application/
)

target_include_directories(Number PRIVATE 
    ${APPLICATION_DIRS}
)
target_include_directories(APPLICATION_LIBRARY PRIVATE 
    ${APPLICATION_DIRS}
)

include(CTest)
enable_testing()

