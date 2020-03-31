# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Sour_Editor")
#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)
set(BOX2D_DIR ${EXTERN_DIR}/Box2D)
set(SFML_DIR ${EXTERN_DIR}/SFML)
set(IMGUI_DIR ${EXTERN_DIR}/imgui_SFML)
set(MEDIA_DIR ${CMAKE_SOURCE_DIR}/media)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)


# Include sub-projects.
add_subdirectory (src/main)
add_subdirectory (src/Bowyer-Watson)
add_subdirectory (src/ECS)
add_subdirectory (extern/imgui_SFML)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sour_Editor)
