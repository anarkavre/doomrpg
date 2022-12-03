file(GLOB_RECURSE HEADER_FILES
	"*.hpp"
	"*.h")

file(GLOB_RECURSE SOURCE_FILES
	"*.cpp"
	"*.c")

source_group(TREE "./" PREFIX "Header Files" FILES ${HEADER_FILES})
source_group(TREE "./" PREFIX "Source Files" FILES ${SOURCE_FILES})

add_executable(doomrpg ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(doomrpg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${OPENGL_LIBRARIES} GLEW::GLEW)