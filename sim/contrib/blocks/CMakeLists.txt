execute_process(COMMAND git submodule update --recursive --init -- Cinder-ImGui
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Cinder-ImGui
  ${CMAKE_CURRENT_SOURCE_DIR}/Cinder-ImGui/src/CinderImGui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cinder-ImGui/lib/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cinder-ImGui/lib/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Cinder-ImGui/lib/imgui/imgui_draw.cpp)
target_include_directories(Cinder-ImGui PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Cinder-ImGui/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Cinder-ImGui/lib/imgui)

target_link_libraries(Cinder-ImGui cinder)

