# dear imgui for sfml
set(IMGUI_SOURCE_FILES
  imgui.cpp
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_widgets.cpp
  imgui-SFML.cpp)

set(IMGUI_HEADER_FILES
  imconfig.h
  imgui.h
  imgui_internal.h
  imstb_rectpack.h
  imstb_textedit.h
  imstb_truetype.h
  imgui-SFML.h
  imgui-SFML_export.h)

add_library(imgui_SFML STATIC ${IMGUI_SOURCE_FILES} ${IMGUI_HEADER_FILES})
target_include_directories(imgui_SFML PUBLIC .. ${SFML_DIR}/include)

target_link_libraries(imgui_SFML optimized ${SFML_DIR}/lib/sfml-graphics.lib)
target_link_libraries(imgui_SFML debug ${SFML_DIR}/lib/sfml-graphics-d.lib)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${IMGUI_SOURCE_FILES} ${IMGUI_HEADER_FILES})
