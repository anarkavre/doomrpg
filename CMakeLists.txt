cmake_minimum_required(VERSION 3.7)

project(doomrpg)

if(MSVC)
	add_definitions("/D_CRT_SECURE_NO_WARNINGS")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
include_directories("src" ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

add_subdirectory(src)