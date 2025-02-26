#pragma once

#include <imgui.h>

#include <Engine/Core/Runtime/Graphics/IGraphicsContext.hpp>
#include <Engine/Core/Runtime/Graphics/IRenderer.hpp>

namespace engine::ui {
    extern void ImGui_Initialize(core::runtime::graphics::IGraphicsContext *gContext, core::runtime::graphics::IRenderer* renderer);
    extern void ImGui_Shutdown();

    extern void ImGui_BeginFrame();
    extern void ImGui_EndFrame();
}