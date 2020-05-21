# CMakeList.txt : CMake project for Sour_Editor, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(SYSTEM_HEADER_FILES
	${INCLUDE_DIR}/main/PhysicsSystem.h
	${INCLUDE_DIR}/main/RenderSystem.h
	${INCLUDE_DIR}/main/EditorSystems.h
	${INCLUDE_DIR}/main/Inspector.h)

set(SYSTEM_SOURCE_FILES
	./PhysicsSystem.cpp
	./RenderSystem.cpp	
	./EditorSystems.cpp
	./Inspector.cpp)

set(MAIN_HEADER_FILES
	${INCLUDE_DIR}/main/pch.h
	${INCLUDE_DIR}/main/EventHandler.h
	${INCLUDE_DIR}/main/InputMapping.h
	${INCLUDE_DIR}/main/EditorTools.h
	${INCLUDE_DIR}/main/Components.h
	${INCLUDE_DIR}/main/Voronoi_Tessellation.h
	${SYSTEM_HEADER_FILES})

set(MAIN_SOURCE_FILES
	./Sour_Editor.cpp
	./pch.cpp
	./EventHandler.cpp
	./InputMapping.cpp
	./EditorTools.cpp
	./Voronoi_Tessellation.cpp
	${SYSTEM_SOURCE_FILES})

# specify linker flags
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:main")

FIND_PACKAGE( Boost REQUIRED COMPONENTS)

add_executable (Sour_Editor)
target_precompile_headers(Sour_Editor PRIVATE ${INCLUDE_DIR}/main/pch.h)
# Add source to this project's executable.
target_sources(Sour_Editor
	PRIVATE
	${MAIN_SOURCE_FILES}
	${MAIN_HEADER_FILES}
)

# this also sets the includes and pulls third party dependencies
target_include_directories(Sour_Editor 
	PRIVATE 
	${SFML_DIR}/include
	${BOX2D_DIR}/include
	${IMGUI_DIR}
	${INCLUDE_DIR}
	${INCLUDE_DIR}/main
	${Boost_INCLUDE_DIR}
)

set(SOUR_LIBRARIES
	optimized ${BOX2D_DIR}/lib/Box2D.lib debug ${BOX2D_DIR}/lib/Box2D-d.lib 
	optimized ${SFML_DIR}/lib/sfml-window.lib debug ${SFML_DIR}/lib/sfml-window-d.lib
	optimized ${SFML_DIR}/lib/sfml-system.lib debug ${SFML_DIR}/lib/sfml-system-d.lib
	optimized ${SFML_DIR}/lib/sfml-graphics.lib debug ${SFML_DIR}/lib/sfml-graphics-d.lib
	opengl32.lib
	ECS
	DELAUNAY_TRIANGULATION
	imgui_SFML
	${Boost_LIBRARIES}
)

target_link_libraries(Sour_Editor ${SOUR_LIBRARIES})

set_property(TARGET Sour_Editor PROPERTY CXX_STANDARD 17)

source_group("Source Files" FILES ${MAIN_SOURCE_FILES})
source_group("Header Files" FILES ${MAIN_HEADER_FILES})
source_group("Systems\\Header Files" FILES ${SYSTEM_HEADER_FILES})
source_group("Systems\\Source Files" FILES ${SYSTEM_SOURCE_FILES})


# Copy dlls to executable directory
add_custom_command(TARGET Sour_Editor POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy_directory
     "${SFML_DIR}/bin/$<CONFIGURATION>"
     "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/" 
  COMMENT "Copying SFML binaries to output directory")