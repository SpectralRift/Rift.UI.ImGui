#pragma once

#include <Engine/Core/Runtime/Graphics/IGraphicsBackend.hpp>
#include <Engine/Core/Runtime/Graphics/IRenderer.hpp>

namespace engine::ui {
    extern bool ImGui_ImplEngine_Init(core::runtime::graphics::IGraphicsContext *gContext, core::runtime::graphics::IRenderer* renderer);

    extern void ImGui_ImplEngine_Shutdown();

    extern void ImGui_ImplEngine_CreateFontsTexture();

    extern void ImGui_ImplEngine_CreateDeviceObjects();

    extern void ImGui_ImplEngine_NewFrame();

    extern void ImGui_ImplEngine_RenderDrawData(ImDrawData *drawData);
}