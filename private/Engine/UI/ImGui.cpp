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

        ImGui::GetStyle().FrameRounding = 3.0f;
        auto colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.251f, 0.251f, 0.251f, 1.0f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.359f, 0.359f, 0.359f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.359f, 0.359f, 0.359f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.251f, 0.251f, 0.251f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.67f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.50f, 1.00f, 1.00f);

        ImGui_ImplEngine_Init(gContext, renderer);
    }

    void ImGui_Shutdown() {
    }

    ImGuiContext* ImGui_GetGlobalContext() {
        return g_ImGuiContext;
    }
}