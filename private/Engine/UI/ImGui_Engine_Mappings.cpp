#include <Engine/UI/ImGui_Engine_Mappings.hpp>

#include <Engine/Input/InputKeyRepository.hpp>
#include <Engine/Core/Hashing/FNV.hpp>

namespace engine::ui {
    ImGuiMouseButton ImGui_ImplEngine_MapInputDeviceButton(input::InputKeyHandle key) {
        switch (key) {
            case engine::FNVConstHash("Mouse_Left"):
                return ImGuiMouseButton_Left;
            case engine::FNVConstHash("Mouse_Right"):
                return ImGuiMouseButton_Right;
            case engine::FNVConstHash("Mouse_Middle"):
                return ImGuiMouseButton_Middle;
            default:
                return ImGuiMouseButton_COUNT;
        }
    }

    ImGuiKey ImGui_ImplEngine_MapKey(input::InputKeyHandle key) {
        switch (key) {
            case engine::FNVConstHash("Key_Tab"):
                return ImGuiKey_Tab;
            case engine::FNVConstHash("Key_LeftArrow"):
                return ImGuiKey_LeftArrow;
            case engine::FNVConstHash("Key_RightArrow"):
                return ImGuiKey_RightArrow;
            case engine::FNVConstHash("Key_UpArrow"):
                return ImGuiKey_UpArrow;
            case engine::FNVConstHash("Key_DownArrow"):
                return ImGuiKey_DownArrow;
            case engine::FNVConstHash("Key_PageUp"):
                return ImGuiKey_PageUp;
            case engine::FNVConstHash("Key_PageDown"):
                return ImGuiKey_PageDown;
            case engine::FNVConstHash("Key_Home"):
                return ImGuiKey_Home;
            case engine::FNVConstHash("Key_End"):
                return ImGuiKey_End;
            case engine::FNVConstHash("Key_Insert"):
                return ImGuiKey_Insert;
            case engine::FNVConstHash("Key_Delete"):
                return ImGuiKey_Delete;
            case engine::FNVConstHash("Key_Backspace"):
                return ImGuiKey_Backspace;
            case engine::FNVConstHash("Key_Space"):
                return ImGuiKey_Space;
            case engine::FNVConstHash("Key_Enter"):
                return ImGuiKey_Enter;
            case engine::FNVConstHash("Key_Escape"):
                return ImGuiKey_Escape;
            case engine::FNVConstHash("Key_LeftCtrl"):
                return ImGuiKey_LeftCtrl;
            case engine::FNVConstHash("Key_LeftShift"):
                return ImGuiKey_LeftShift;
            case engine::FNVConstHash("Key_LeftAlt"):
                return ImGuiKey_LeftAlt;
            case engine::FNVConstHash("Key_LeftSuper"):
                return ImGuiKey_LeftSuper;
            case engine::FNVConstHash("Key_RightCtrl"):
                return ImGuiKey_RightCtrl;
            case engine::FNVConstHash("Key_RightShift"):
                return ImGuiKey_RightShift;
            case engine::FNVConstHash("Key_RightAlt"):
                return ImGuiKey_RightAlt;
            case engine::FNVConstHash("Key_RightSuper"):
                return ImGuiKey_RightSuper;
            case engine::FNVConstHash("Key_Menu"):
                return ImGuiKey_Menu;
            case engine::FNVConstHash("Key_Apostrophe"):
                return ImGuiKey_Apostrophe;
            case engine::FNVConstHash("Key_Comma"):
                return ImGuiKey_Comma;
            case engine::FNVConstHash("Key_Minus"):
                return ImGuiKey_Minus;
            case engine::FNVConstHash("Key_Period"):
                return ImGuiKey_Period;
            case engine::FNVConstHash("Key_Slash"):
                return ImGuiKey_Slash;
            case engine::FNVConstHash("Key_Semicolon"):
                return ImGuiKey_Semicolon;
            case engine::FNVConstHash("Key_Equal"):
                return ImGuiKey_Equal;
            case engine::FNVConstHash("Key_LeftBracket"):
                return ImGuiKey_LeftBracket;
            case engine::FNVConstHash("Key_Backslash"):
                return ImGuiKey_Backslash;
            case engine::FNVConstHash("Key_RightBracket"):
                return ImGuiKey_RightBracket;
            case engine::FNVConstHash("Key_GraveAccent"):
                return ImGuiKey_GraveAccent;
            case engine::FNVConstHash("Key_CapsLock"):
                return ImGuiKey_CapsLock;
            case engine::FNVConstHash("Key_ScrollLock"):
                return ImGuiKey_ScrollLock;
            case engine::FNVConstHash("Key_NumLock"):
                return ImGuiKey_NumLock;
            case engine::FNVConstHash("Key_PrintScreen"):
                return ImGuiKey_PrintScreen;
            case engine::FNVConstHash("Key_Pause"):
                return ImGuiKey_Pause;
            case engine::FNVConstHash("Key_KeypadDecimal"):
                return ImGuiKey_KeypadDecimal;
            case engine::FNVConstHash("Key_KeypadDivide"):
                return ImGuiKey_KeypadDivide;
            case engine::FNVConstHash("Key_KeypadMultiply"):
                return ImGuiKey_KeypadMultiply;
            case engine::FNVConstHash("Key_KeypadSubtract"):
                return ImGuiKey_KeypadSubtract;
            case engine::FNVConstHash("Key_KeypadAdd"):
                return ImGuiKey_KeypadAdd;
            case engine::FNVConstHash("Key_KeypadEnter"):
                return ImGuiKey_KeypadEnter;
            case engine::FNVConstHash("Key_KeypadEqual"):
                return ImGuiKey_KeypadEqual;
            default:
                auto buttonName = input::InputKeyRepository::Instance().GetKey(key);

                // smart trick to save up on if statements for numeric keys; we'll just calculate offsets for 0-9 and A-Z
                if (buttonName.size() == 5 && buttonName.starts_with("Key_")) {
                    char c = buttonName[4];
                    if (c >= '0' && c <= '9') { return (ImGuiKey) (ImGuiKey_0 + (c - '0')); }
                    if (c >= 'A' && c <= 'Z') { return (ImGuiKey) (ImGuiKey_A + (c - 'A')); }
                }

                // if it starts with "Key_F" then it must be a function key
                else if (buttonName.starts_with("Key_F")) {
                    int fKey = atoi(buttonName.data() + 5); // get the number after F
                    if (fKey >= 1 && fKey <= 24) {
                        return (ImGuiKey) (ImGuiKey_F1 + (fKey - 1)); // do some smart tricks again to calculate offsets!
                    }
                }

                // if its a keypad key, we check it
                else if (buttonName.starts_with("Key_Keypad")) {
                    char c = buttonName[10]; // get the number after "Key_Keypad"
                    if (c >= '0' && c <= '9') { return (ImGuiKey) (ImGuiKey_Keypad0 + (c - '0')); }
                }

                // invalid result by default.
                return ImGuiKey_None;
        }
    }
}