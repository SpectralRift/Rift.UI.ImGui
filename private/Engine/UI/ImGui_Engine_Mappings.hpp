#pragma once

#include <Engine/Input/InputKeyRepository.hpp>
#include <Engine/UI/ImGui.hpp>

namespace engine::ui {
    extern ImGuiMouseButton ImGui_ImplEngine_MapInputDeviceButton(input::InputKeyHandle key);
    extern ImGuiKey ImGui_ImplEngine_MapKey(input::InputKeyHandle key);
}