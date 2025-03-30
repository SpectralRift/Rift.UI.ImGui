#include <Engine/Input/ImGui_InputTarget.hpp>

#include <imgui_internal.h>

namespace engine::input {
    std::string ImGuiInputTarget::GetText() {
        if(!m_TextState) {
            return "";
        }

        return {
            m_TextState->TextA.Data,
            m_TextState->TextA.Data + m_TextState->TextA.Size
        };
    }

    void ImGuiInputTarget::SetText(std::string_view text, bool isEnter) {
        if(!m_TextState) {
            printf("m_TextState = NULL!\n");
            return;
        }

        m_TextState->ClearText();
        m_IO.AddInputCharactersUTF8(text.data());

        if(isEnter) {
            // simulate enter press
            m_IO.AddKeyEvent(ImGuiKey_Enter, true);
            m_IO.AddKeyEvent(ImGuiKey_Enter, false);
        }
    }

    engine::input::InputTargetType ImGuiInputTarget::GetTargetType() {
        if(m_TextState && (m_TextState->Flags & ImGuiInputTextFlags_Password) > 0) {
            return engine::input::InputTargetType::INPUT_TARGET_TYPE_PASSWORD;
        }

        return engine::input::InputTargetType::INPUT_TARGET_TYPE_TEXT;
    }

    ImGuiID ImGuiInputTarget::GetID() const {
        return m_TextState ? m_TextState->ID : 0;
    }
}