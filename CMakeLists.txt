cmake_minimum_required (VERSION 2.6)
project (gl-instancing)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

add_executable(gl-instancing main.c instanced_renderer.c standard_renderer.c)
find_package (glfw REQUIRED)
find_package (opengl REQUIRED)
find_package (Argtable2 REQUIRED)
target_link_libraries (gl-instancing ${GLFW_LIBRARY})
target_link_libraries (gl-instancing ${OPENGL_LIBRARY})
target_link_libraries (gl-instancing ${LIBARGTABLE2_LIBS})
