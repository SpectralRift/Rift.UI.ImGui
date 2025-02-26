#include <memory>
#include <vector>

#include <Engine/UI/ImGui.hpp>
#include <Engine/UI/ImGui_Impl_Engine.hpp>

#include <Engine/Core/Runtime/Graphics/ITexture.hpp>
#include <Engine/Core/Runtime/Graphics/Vertex.hpp>
#include <Engine/Core/Runtime/IWindow.hpp>
#include <Engine/Core/Runtime/Input/InputManager.hpp>
#include <Engine/Core/Runtime/Input/InputEvent.hpp>

namespace engine::ui {
    struct ImGui_ImplEngine_Data {
        std::unique_ptr<core::runtime::graphics::ITexture> m_FontTexture;
        core::runtime::graphics::IGraphicsContext *m_GfxContext;
        core::runtime::graphics::IRenderer *m_Renderer;
    };

    static ImGui_ImplEngine_Data *ImGui_ImplEngine_GetBackendData() {
        return ImGui::GetCurrentContext() ? (ImGui_ImplEngine_Data *) ImGui::GetIO().BackendRendererUserData : nullptr;
    }

    ImGuiMouseButton ImGui_ImplEngine_MapInputDeviceButton(core::runtime::input::InputDeviceButton button) {
        switch (button) {
            default:
            case core::runtime::input::INPUT_DEVICE_BUTTON_MOUSE_LEFT:
                return ImGuiMouseButton_Left;
            case core::runtime::input::INPUT_DEVICE_BUTTON_MOUSE_RIGHT:
                return ImGuiMouseButton_Right;
            case core::runtime::input::INPUT_DEVICE_BUTTON_MOUSE_MIDDLE:
                return ImGuiMouseButton_Middle;
        }
    }

    void ImGui_ImplEngine_OnInputEvent(core::runtime::input::InputEvent *event) {
        ImGuiIO &io = ImGui::GetIO();

        auto eventType = event->GetEventType();

        if (eventType == core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_MOVE) {
            auto mvEv = dynamic_cast<engine::core::runtime::input::MouseMoveEvent *>(event);
            io.AddMousePosEvent(mvEv->Position.x, mvEv->Position.y);
        } else if (eventType == core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_KEY_DOWN) {
            auto mdEv = dynamic_cast<engine::core::runtime::input::MouseButtonDownEvent *>(event);
            io.AddMouseButtonEvent(ImGui_ImplEngine_MapInputDeviceButton(mdEv->Button), true);
        } else if (eventType == core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_KEY_UP) {
            auto mdEv = dynamic_cast<engine::core::runtime::input::MouseButtonUpEvent *>(event);
            io.AddMouseButtonEvent(ImGui_ImplEngine_MapInputDeviceButton(mdEv->Button), false);
        } else if (eventType == core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_START) {
            auto tsEv = dynamic_cast<engine::core::runtime::input::TouchStartEvent *>(event);

            // make sure we're tracking only the first finger
            if (tsEv->FingerId == 0) {
                io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
                io.AddMousePosEvent(tsEv->Position.x, tsEv->Position.y);
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
            }
        } else if (eventType == core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_DRAG) {
            auto tdEv = dynamic_cast<engine::core::runtime::input::TouchDragEvent *>(event);

            // make sure we're tracking only the first finger
            if (tdEv->FingerId == 0) {
                io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
                io.AddMousePosEvent(tdEv->Position.x, tdEv->Position.y);
            }
        } else if (eventType == core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_END) {
            auto teEv = dynamic_cast<engine::core::runtime::input::TouchEndEvent *>(event);

            // make sure we're tracking only the first finger
            if (teEv->FingerId == 0) {
                io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
                io.AddMousePosEvent(teEv->Position.x, teEv->Position.y);
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
            }
        } else if (eventType == core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_HOVER) {
            auto thEv = dynamic_cast<engine::core::runtime::input::TouchHoverEvent *>(event);

            io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
            io.AddMousePosEvent(thEv->Position.x, thEv->Position.y);
        }
    }

    bool ImGui_ImplEngine_Init(core::runtime::graphics::IGraphicsContext *gContext,
                               core::runtime::graphics::IRenderer *renderer) {
        ImGuiIO &io = ImGui::GetIO();
        IMGUI_CHECKVERSION();
        IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

        io.BackendRendererUserData = IM_NEW(ImGui_ImplEngine_Data)();
        io.BackendRendererName = "imgui_impl_engine_rift";
        io.BackendPlatformName = "imgui_impl_engine_rift";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

        ImFontConfig font_cfg;
        font_cfg.SizePixels = 20.0f;

//        io.Fonts->AddFontDefault(&font_cfg);
//        ImGui::GetStyle().ScaleAllSizes(1.3f);

        ImGui::GetStyle().FrameRounding = 3.0f;
        auto colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.251f, 0.251f, 0.251f, 1.0f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.359f, 0.359f, 0.359f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.359f, 0.359f, 0.359f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.251f, 0.251f, 0.251f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.67f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.50f, 1.00f, 1.00f);

        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        bd->m_GfxContext = gContext;
        bd->m_Renderer = renderer;

        auto inputManager = engine::core::runtime::input::InputManager::Instance();

        inputManager->RegisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_MOVE,
                                           ImGui_ImplEngine_OnInputEvent);
        inputManager->RegisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_KEY_DOWN,
                                           ImGui_ImplEngine_OnInputEvent);
        inputManager->RegisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_KEY_UP,
                                           ImGui_ImplEngine_OnInputEvent);
        inputManager->RegisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_START,
                                           ImGui_ImplEngine_OnInputEvent);
        inputManager->RegisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_DRAG,
                                           ImGui_ImplEngine_OnInputEvent);
        inputManager->RegisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_END,
                                           ImGui_ImplEngine_OnInputEvent);
        inputManager->RegisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_HOVER,
                                           ImGui_ImplEngine_OnInputEvent);
//        auto mousePos = inputManager->GetAxis(core::runtime::input::INPUT_DEVICE_AXIS_MOUSE);
//
//        io.AddMousePosEvent(mousePos.x, mousePos.y);
//        io.AddMouseButtonEvent(ImGuiMouseButton_Left, inputManager->GetButton(core::runtime::input::INPUT_DEVICE_BUTTON_MOUSE_LEFT));
//        io.AddMouseButtonEvent(ImGuiMouseButton_Right, inputManager->GetButton(core::runtime::input::INPUT_DEVICE_BUTTON_MOUSE_RIGHT));
//        io.AddMouseButtonEvent(ImGuiMouseButton_Middle, inputManager->GetButton(core::runtime::input::INPUT_DEVICE_BUTTON_MOUSE_MIDDLE));

        return true;
    }

    void ImGui_ImplEngine_DestroyFontsTexture() {
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();

        if (bd->m_FontTexture) {
//            bd->m_FontTexture->Destroy();
            io.Fonts->SetTexID(nullptr);
            bd->m_FontTexture = nullptr;
        }
    }

    void ImGui_ImplEngine_Shutdown() {
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
        ImGuiIO &io = ImGui::GetIO();

        auto inputManager = engine::core::runtime::input::InputManager::Instance();
        inputManager->UnregisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_MOVE,
                                             ImGui_ImplEngine_OnInputEvent);
        inputManager->UnregisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_KEY_DOWN,
                                             ImGui_ImplEngine_OnInputEvent);
        inputManager->UnregisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_MOUSE_KEY_UP,
                                             ImGui_ImplEngine_OnInputEvent);
        inputManager->UnregisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_START,
                                             ImGui_ImplEngine_OnInputEvent);
        inputManager->UnregisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_DRAG,
                                             ImGui_ImplEngine_OnInputEvent);
        inputManager->UnregisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_END,
                                             ImGui_ImplEngine_OnInputEvent);
        inputManager->UnregisterEventHandler(core::runtime::input::InputEventType::INPUT_EVENT_TYPE_TOUCH_HOVER,
                                             ImGui_ImplEngine_OnInputEvent);

//        ImGui_ImplOpenGL2_DestroyDeviceObjects();
        io.BackendRendererName = nullptr;
        io.BackendRendererUserData = nullptr;
        io.BackendFlags &= ~ImGuiBackendFlags_RendererHasViewports;

        IM_DELETE(bd);
    }

    void ImGui_ImplEngine_CreateFontsTexture() {
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplEngine_Init()?");

        ImGuiIO &io = ImGui::GetIO();

        // create font atlas texture from ImGUI
        unsigned char *pixels;
        int width, height;

        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        // create font texture and upload it to GPU
        bd->m_FontTexture = bd->m_GfxContext->GetBackend()->CreateTexture();
        bd->m_FontTexture->Create({
                                          {
                                                  reinterpret_cast<core::runtime::graphics::Color *>(pixels),
                                                  reinterpret_cast<core::runtime::graphics::Color *>(pixels) +
                                                  width * height
                                          },
                                          {
                                                  (float) width,
                                                  (float) height
                                          }
                                  });

        io.Fonts->SetTexID((ImTextureID) bd->m_FontTexture.get());
    }

    void ImGui_ImplEngine_NewFrame() {
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplEngine_Init()?");

        ImGuiIO &io = ImGui::GetIO();

        io.DeltaTime = (float) 1.f / 60.f;
        auto winSize = bd->m_GfxContext->GetOwnerWindow()->GetSize();
        io.DisplaySize = {winSize.x, winSize.y};

        if (!bd->m_FontTexture)
            ImGui_ImplEngine_CreateFontsTexture();
    }

    void ImGui_ImplEngine_RenderDrawData(ImDrawData *drawData) {
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplEngine_Init()?");

        auto fb_width = (int) (drawData->DisplaySize.x * drawData->FramebufferScale.x);
        auto fb_height = (int) (drawData->DisplaySize.y * drawData->FramebufferScale.y);

        if (fb_width == 0 || fb_height == 0)
            return;

        auto io = ImGui::GetIO();

        auto clip_off = drawData->DisplayPos;
        auto clip_scale = drawData->FramebufferScale;

        for (int n = 0; n < drawData->CmdListsCount; n++) {
            const ImDrawList *cmdList = drawData->CmdLists[n];

            for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; cmdIdx++) {
                const ImDrawCmd *cmdPtr = &cmdList->CmdBuffer[cmdIdx];

                if (cmdPtr->UserCallback) {
                    if (cmdPtr->UserCallback != ImDrawCallback_ResetRenderState) {
                        cmdPtr->UserCallback(cmdList, cmdPtr);
                    }
                } else {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec2 clip_min((cmdPtr->ClipRect.x - clip_off.x) * clip_scale.x,
                                    (cmdPtr->ClipRect.y - clip_off.y) * clip_scale.y);
                    ImVec2 clip_max((cmdPtr->ClipRect.z - clip_off.x) * clip_scale.x,
                                    (cmdPtr->ClipRect.w - clip_off.y) * clip_scale.y);
                    if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                        continue;

                    std::vector<core::runtime::graphics::Vertex> vtxCollection(cmdPtr->ElemCount);

                    const ImDrawIdx *indexBuffer = cmdList->IdxBuffer.Data + cmdPtr->IdxOffset;
                    const ImDrawVert *vertexBuffer = cmdList->VtxBuffer.Data + cmdPtr->VtxOffset;

                    // put vertices into the list
                    for (int idxIdx = 0; idxIdx < cmdPtr->ElemCount; idxIdx++) {
                        auto vtx = vertexBuffer[indexBuffer[idxIdx]];

                        vtxCollection.emplace_back(core::runtime::graphics::Vertex{
                                {vtx.pos.x, vtx.pos.y, 0.f},
                                {vtx.uv.x,  1.f - vtx.uv.y},
                                {0.f,       0.f,       0.f},
                                {
                                 (uint8_t) (vtx.col & 0xFF),
                                            (uint8_t) ((vtx.col >> 8) & 0xFF),
                                                       (uint8_t) ((vtx.col >> 16) & 0xFF),
                                        (uint8_t) ((vtx.col >> 24) & 0xFF),
                                }
                        });
                    }

                    bd->m_Renderer->SubmitUI(engine::core::runtime::graphics::UIRenderItem{
                            engine::core::runtime::graphics::PrimitiveType::PRIMITIVE_TYPE_TRIANGLES,
                            std::move(vtxCollection),
                            (core::runtime::graphics::ITexture *) cmdPtr->GetTexID(),
                            {clip_min.x, clip_max.y},
                            {clip_max.x - clip_min.x, clip_max.y - clip_min.y}
                    });
                }
            }
        }
    }
}