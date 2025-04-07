cmake_minimum_required(VERSION 3.14)
project(Rift_UI_ImGui VERSION 1.12.7)

add_library(
        Rift_UI_ImGui
        STATIC
        private/Engine/UI/ImGui.cpp
        private/Engine/UI/ImGui_Engine_Mappings.cpp
        private/Engine/UI/ImGui_Impl_Engine.cpp
        private/Engine/Input/ImGui_InputTarget.cpp
        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_demo.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_widgets.cpp
)

target_include_directories(
        Rift_UI_ImGui
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public" "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui"
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private"
)

target_compile_definitions(
        Rift_UI_ImGui
        PRIVATE "-DIMGUI_USER_CONFIG=<Engine/UI/ImGui_Rift_Config.hpp>"
)

rift_resolve_module_libs("Rift.Core.Runtime;Rift.Input" RIFT_IMGUI_DEPS)

target_link_libraries(Rift_UI_ImGui ${RIFT_IMGUI_DEPS})

rift_bundle_folder(${CMAKE_CURRENT_SOURCE_DIR}/assets)