cmake_minimum_required(VERSION 3.1)
project(OpenglShader)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

include_directories(include
    ${GLUT_INCLUDE_DIR}
    ${glm_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    )

#-----------compilation step
add_executable( myOpengl
    src/program.cc
    src/util.cc
    src/image.cc
    src/image_io.cc
    src/main.cc
    src/camera.cc
    )

#-----------linking step
target_link_libraries(myOpengl ${GLUT_LIBRARY}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
 )
