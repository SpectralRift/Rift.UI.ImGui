#include <Engine/Input/IInputTarget.hpp>

#include <Engine/UI/ImGui.hpp>

struct ImGuiInputTextState;

namespace engine::input {
    struct ImGuiInputTarget : engine::input::IInputTarget {
        ImGuiInputTarget(ImGuiInputTextState* state, ImGuiIO& io) : m_TextState(state), m_IO(io) {}

        std::string GetText() override;

        void SetText(std::string_view text, bool isEnter) override;

        std::string GetHint() override {
            return "";
        }

        engine::input::InputTargetType GetTargetType() override;

        ImGuiID GetID() const;

        void SetTextState(ImGuiInputTextState* state) { m_TextState = state; }
        ImGuiInputTextState* GetTextState() const { return m_TextState; }
    protected:
        ImGuiInputTextState* m_TextState;
        ImGuiIO& m_IO;
    };
}