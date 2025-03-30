#include <memory>
#include <vector>

#include <Engine/UI/ImGui_Engine_Mappings.hpp>
#include <Engine/UI/ImGui_Impl_Engine.hpp>

#include <Engine/Core/Runtime/Graphics/ITexture.hpp>
#include <Engine/Core/Runtime/Graphics/Vertex.hpp>
#include <Engine/Core/Runtime/IWindow.hpp>

#include <Engine/Input/ImGui_InputTarget.hpp>
#include <Engine/Input/InputManager.hpp>

#include <Engine/Core/Platform.hpp>

#include <imgui_internal.h>

namespace engine::ui {
    struct ImGui_ImplEngine_Data {
        std::unique_ptr<core::runtime::graphics::ITexture> m_FontTexture;
        core::runtime::graphics::IGraphicsContext *m_GfxContext;
        core::runtime::graphics::IRenderer *m_Renderer;
        std::unique_ptr<input::ImGuiInputTarget> m_UIInputTarget;
    };

    static ImGui_ImplEngine_Data *ImGui_ImplEngine_GetBackendData() {
        return ImGui::GetCurrentContext() ? (ImGui_ImplEngine_Data *) ImGui::GetIO().BackendRendererUserData : nullptr;
    }

    void ImGui_ImplEngine_OnKeyStateChanged(input::InputKeyHandle key, bool state) {
        auto x = ImGui_ImplEngine_MapKey(key);

        if (x != ImGuiKey_None) {
            ImGui::GetIO().AddKeyEvent(x, state);
        }
    }

    void ImGui_ImplEngine_OnMouseStateChanged(input::InputKeyHandle key, bool state) {
        auto x = ImGui_ImplEngine_MapInputDeviceButton(key);

        if (x != ImGuiMouseButton_COUNT) {
            ImGui::GetIO().AddMouseButtonEvent(x, state);
        }
    }

    void ImGui_ImplEngine_OnMousePosition(core::math::Vector2 position) {
        ImGuiIO &io = ImGui::GetIO();

        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMousePosEvent(position.x, position.y);
    }

    void ImGui_ImplEngine_OnTouchMove(int fingerId, core::math::Vector2 position) {
        ImGuiIO &io = ImGui::GetIO();

        // make sure we're tracking only the first finger
        if (fingerId == 0) {
            io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
            io.AddMousePosEvent(position.x, position.y);
        }
    }

    void ImGui_ImplEngine_OnTouchDown(int fingerId, core::math::Vector2 position) {
        ImGui_ImplEngine_OnTouchMove(fingerId, position);

        // make sure we're tracking only the first finger
        if (fingerId == 0) {
            ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Left, true);
        }
    }

    void ImGui_ImplEngine_OnTouchUp(int fingerId, core::math::Vector2 position) {
        ImGui_ImplEngine_OnTouchMove(fingerId, position);

        // make sure we're tracking only the first finger
        if (fingerId == 0) {
            ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Left, false);
        }
    }

    bool ImGui_ImplEngine_OnInputEvent(const input::InputEvent &event) {
        ImGuiIO &io = ImGui::GetIO();

        switch (event.Type) {
            case input::INPUT_EVENT_TYPE_INPUT_CHAR:
                io.AddInputCharacterUTF16(event.UInputChar);
                break;
            case input::INPUT_EVENT_TYPE_KEY_STATE_CHANGE:
                ImGui_ImplEngine_OnKeyStateChanged(event.Key, event.KeyState);
                ImGui_ImplEngine_OnMouseStateChanged(event.Key, event.KeyState);
                break;
            case input::INPUT_EVENT_TYPE_MOUSE_POSITION:
                ImGui_ImplEngine_OnMousePosition(event.Position);
                break;
            case input::INPUT_EVENT_TYPE_TOUCH_MOVE:
                ImGui_ImplEngine_OnTouchMove(event.TouchFinger, event.Position);
                break;
            case input::INPUT_EVENT_TYPE_TOUCH_DOWN:
                ImGui_ImplEngine_OnTouchDown(event.TouchFinger, event.Position);
                break;
            case input::INPUT_EVENT_TYPE_TOUCH_UP:
                ImGui_ImplEngine_OnTouchUp(event.TouchFinger, event.Position);
                break;
            case input::INPUT_EVENT_TYPE_TOUCH_HOVER:
                ImGui_ImplEngine_OnTouchMove(0, event.Position);
                break;
            default:
                break;
        }

        return io.WantCaptureMouse || io.WantCaptureKeyboard;
    }

    bool ImGui_ImplEngine_Init(core::runtime::graphics::IGraphicsContext *gContext,
                               core::runtime::graphics::IRenderer *renderer) {
        ImGuiIO &io = ImGui::GetIO();
        IMGUI_CHECKVERSION();
        IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

        io.BackendRendererUserData = IM_NEW(ImGui_ImplEngine_Data)();
        io.BackendRendererName = "imgui_impl_engine_rift";
        io.BackendPlatformName = "imgui_impl_engine_rift";
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

        io.PlatformSetImeDataFn = [](ImGuiContext* context, ImGuiViewport *viewport, ImGuiPlatformImeData *data) {
            if (!core::Platform::GetVirtualKeyboard()) {
                return;
            }

            auto bd = (ImGui_ImplEngine_Data *) context->IO.BackendRendererUserData;

            if(data->WantVisible) {
                auto textState = ImGui::GetInputTextState(ImGui::GetActiveID());

                if(!bd->m_UIInputTarget) {
                    bd->m_UIInputTarget = std::make_unique<input::ImGuiInputTarget>(
                            textState,
                            context->IO
                    );
                } else {
                    bd->m_UIInputTarget->SetTextState(textState);
                }

                core::Platform::GetVirtualKeyboard()->Show(true, bd->m_UIInputTarget.get());
            } else {
                bd->m_UIInputTarget.reset();
                core::Platform::GetVirtualKeyboard()->Show(false, nullptr);
            }
        };

        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        bd->m_GfxContext = gContext;
        bd->m_Renderer = renderer;

        input::InputManager::Instance()->AddInputListener(ImGui_ImplEngine_OnInputEvent, true);

        return true;
    }

    void ImGui_ImplEngine_DestroyFontsTexture() {
        ImGuiIO &io = ImGui::GetIO();
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();

        if (bd->m_FontTexture) {
            bd->m_FontTexture->Destroy();
            io.Fonts->SetTexID(nullptr);
            bd->m_FontTexture = nullptr;
        }
    }

    void ImGui_ImplEngine_Shutdown() {
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
        ImGuiIO &io = ImGui::GetIO();

        auto inputManager = input::InputManager::Instance();
        inputManager->RemoveInputListener(ImGui_ImplEngine_OnInputEvent);

        ImGui_ImplEngine_DestroyFontsTexture();

        io.BackendRendererName = nullptr;
        io.BackendRendererUserData = nullptr;
        io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;

        IM_DELETE(bd);
    }

    void ImGui_ImplEngine_CreateFontsTexture() {
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplEngine_Init()?");

        ImGuiIO &io = ImGui::GetIO();

        // create font atlas texture from ImGUI
        core::runtime::graphics::Color *pixels;
        int width, height;

        io.Fonts->GetTexDataAsRGBA32(reinterpret_cast<unsigned char **>(&pixels), &width, &height);

        // create font texture and upload it to GPU
        bd->m_FontTexture = bd->m_GfxContext->GetBackend()->CreateTexture();
        IM_ASSERT(bd->m_FontTexture != nullptr && "Failed to create font atlas texture!");
        bd->m_FontTexture->Create({
                                          {
                                                  pixels,
                                                  pixels + width * height
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

        if (!bd->m_FontTexture) {
            ImGui_ImplEngine_CreateFontsTexture();
        }
    }

    void ImGui_ImplEngine_RenderDrawData(ImDrawData *drawData) {
        ImGui_ImplEngine_Data *bd = ImGui_ImplEngine_GetBackendData();
        IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplEngine_Init()?");

        auto fb_width = (int) (drawData->DisplaySize.x * drawData->FramebufferScale.x);
        auto fb_height = (int) (drawData->DisplaySize.y * drawData->FramebufferScale.y);

        if (fb_width == 0 || fb_height == 0) { return; }

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

                    if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) { continue; }

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
                            {clip_min.x, clip_min.y},
                            {clip_max.x - clip_min.x, clip_max.y - clip_min.y}
                    });
                }
            }
        }
    }
}