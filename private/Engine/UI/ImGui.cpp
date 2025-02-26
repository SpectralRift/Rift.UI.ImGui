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

        io.Fonts->AddFontFromFileTTF("DataRaw/Engine/Fonts/Lexend.ttf", 16.0f);

        ImGui_ImplEngine_Init(gContext, renderer);
    }

    void ImGui_Shutdown() {
    }
}