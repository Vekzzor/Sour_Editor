# CMakeList.txt : CMake project for Mustig_Engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(ECS_HEADER_FILES
	${INCLUDE_DIR}/ECS/ECS.h
	${INCLUDE_DIR}/ECS/Coordinator.h
	${INCLUDE_DIR}/ECS/EntityManager.h
	${INCLUDE_DIR}/ECS/SystemManager.h
	${INCLUDE_DIR}/ECS/ComponentManager.h
	${INCLUDE_DIR}/ECS/ComponentArray.h)

set(ECS_SOURCE_FILES
	./ComponentArray.cpp
	./ComponentManager.cpp
	./Coordinator.cpp
	./EntityManager.cpp
	./SystemManager.cpp)


add_library(ECS)
# Add source to this project's executable.
target_sources(ECS
	PRIVATE
	${ECS_SOURCE_FILES}
	${ECS_HEADER_FILES}
	
)

target_include_directories(ECS PRIVATE ${SFML_DIR}/include ${INCLUDE_DIR}/ECS)

target_link_libraries(ECS optimized ${SFML_DIR}/lib/sfml-graphics.lib)
target_link_libraries(ECS debug ${SFML_DIR}/lib/sfml-graphics-d.lib)

source_group("Source Files" FILES ${ECS_SOURCE_FILES})
source_group("Header Files" FILES ${ECS_HEADER_FILES})