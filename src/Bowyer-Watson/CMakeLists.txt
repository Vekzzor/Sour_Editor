# CMakeList.txt : CMake project for Mustig_Engine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(DEL_TRI_HEADER_FILES 
	${INCLUDE_DIR}/Bowyer-Watson/Delunay.h
	${INCLUDE_DIR}/Bowyer-Watson/Data_Structures/DCELHE.h)

set(DEL_TRI_SOURCE_FILES 
	./Delunay.cpp
	./Data_Structures/DCELHE.cpp)

add_library(DELAUNAY_TRIANGULATION)

target_sources(DELAUNAY_TRIANGULATION
	PRIVATE
	${DEL_TRI_SOURCE_FILES}
	${DEL_TRI_HEADER_FILES})

# this also sets the includes and pulls third party dependencies
target_include_directories(DELAUNAY_TRIANGULATION 
	PUBLIC ..
	${SFML_DIR}/include
	${INCLUDE_DIR}/Bowyer-Watson
	${INCLUDE_DIR}/Bowyer-Watson/Data_Structures
)

target_link_libraries(DELAUNAY_TRIANGULATION optimized ${SFML_DIR}/lib/sfml-graphics.lib)
target_link_libraries(DELAUNAY_TRIANGULATION debug ${SFML_DIR}/lib/sfml-graphics-d.lib)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DEL_TRI_SOURCE_FILES})
source_group(TREE ${INCLUDE_DIR}/Bowyer-Watson FILES ${DEL_TRI_HEADER_FILES})
