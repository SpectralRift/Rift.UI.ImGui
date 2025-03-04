#include <Engine/UI/ImGui.hpp>
#include <Engine/UI/ImGui_Impl_Engine.hpp>

#include <imgui_internal.h>

namespace engine::ui {
    static ImGuiContext* g_ImGuiContext;

    void ImGui_BeginFrame() {
        if (!g_ImGuiContext) {
            return;
        }

        ImGui::SetCurrentContext(g_ImGuiContext);
        ImGui_ImplEngine_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
    }

    void ImGui_EndFrame() {
        ImGui::Render();
        ImGui_ImplEngine_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGui_Initialize(engine::core::runtime::graphics::IGraphicsContext* gContext, core::runtime::graphics::IRenderer* renderer) {
        g_ImGuiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(g_ImGuiContext);

        ImGuiIO &io = ImGui::GetIO();

        ImFontConfig fontConfig;
        fontConfig.OversampleH = 2;
        fontConfig.OversampleV = 2;

        io.Fonts->AddFontFromFileTTF("DataRaw/Engine/Fonts/Lexend.ttf", 16.0f, &fontConfig);
        io.Fonts->AddFontFromFileTTF("DataRaw/Engine/Fonts/SourceCodePro.ttf", 14.0f, &fontConfig);

        ImGui_ImplEngine_Init(gContext, renderer);
    }

    void ImGui_Shutdown() {
    }

    ImGuiContext* ImGui_GetGlobalContext() {
        return g_ImGuiContext;
    }
}