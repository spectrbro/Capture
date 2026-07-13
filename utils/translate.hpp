#pragma once
#include <cstdint>
#include <array>
#include <Carbon/Carbon.h>

// 100% Claude generated
// PS/2 Set 1 scan codes → Mac CGKeyCodes
// Base scan codes (no E0 prefix)
// Extended scan codes have bit 8 set (E0 prefix)
// Unmapped entries hold 0xFF, the sentinel translateKey returns for "no mapping".
// (0 can't be the sentinel since kVK_ANSI_A == 0 is a real key.)
static uint32_t translateKey(uint16_t scancode)
{
    static const std::array<uint8_t, 256> base = []{
        std::array<uint8_t, 256> a;
        a.fill(0xFF);
        a[0x01] = kVK_Escape;
        a[0x02] = kVK_ANSI_1;
        a[0x03] = kVK_ANSI_2;
        a[0x04] = kVK_ANSI_3;
        a[0x05] = kVK_ANSI_4;
        a[0x06] = kVK_ANSI_5;
        a[0x07] = kVK_ANSI_6;
        a[0x08] = kVK_ANSI_7;
        a[0x09] = kVK_ANSI_8;
        a[0x0A] = kVK_ANSI_9;
        a[0x0B] = kVK_ANSI_0;
        a[0x0C] = kVK_ANSI_Minus;
        a[0x0D] = kVK_ANSI_Equal;
        a[0x0E] = kVK_Delete;           // Backspace
        a[0x0F] = kVK_Tab;
        a[0x10] = kVK_ANSI_Q;
        a[0x11] = kVK_ANSI_W;
        a[0x12] = kVK_ANSI_E;
        a[0x13] = kVK_ANSI_R;
        a[0x14] = kVK_ANSI_T;
        a[0x15] = kVK_ANSI_Y;
        a[0x16] = kVK_ANSI_U;
        a[0x17] = kVK_ANSI_I;
        a[0x18] = kVK_ANSI_O;
        a[0x19] = kVK_ANSI_P;
        a[0x1A] = kVK_ANSI_LeftBracket;
        a[0x1B] = kVK_ANSI_RightBracket;
        a[0x1C] = kVK_Return;
        a[0x1D] = kVK_Control;          // LCtrl
        a[0x1E] = kVK_ANSI_A;
        a[0x1F] = kVK_ANSI_S;
        a[0x20] = kVK_ANSI_D;
        a[0x21] = kVK_ANSI_F;
        a[0x22] = kVK_ANSI_G;
        a[0x23] = kVK_ANSI_H;
        a[0x24] = kVK_ANSI_J;
        a[0x25] = kVK_ANSI_K;
        a[0x26] = kVK_ANSI_L;
        a[0x27] = kVK_ANSI_Semicolon;
        a[0x28] = kVK_ANSI_Quote;
        a[0x29] = kVK_ANSI_Grave;
        a[0x2A] = kVK_Shift;            // LShift
        a[0x2B] = kVK_ANSI_Backslash;
        a[0x2C] = kVK_ANSI_Z;
        a[0x2D] = kVK_ANSI_X;
        a[0x2E] = kVK_ANSI_C;
        a[0x2F] = kVK_ANSI_V;
        a[0x30] = kVK_ANSI_B;
        a[0x31] = kVK_ANSI_N;
        a[0x32] = kVK_ANSI_M;
        a[0x33] = kVK_ANSI_Comma;
        a[0x34] = kVK_ANSI_Period;
        a[0x35] = kVK_ANSI_Slash;
        a[0x36] = kVK_RightShift;
        a[0x37] = kVK_ANSI_KeypadMultiply;
        a[0x38] = kVK_Option;           // LAlt
        a[0x39] = kVK_Space;
        a[0x3A] = kVK_CapsLock;
        a[0x3B] = kVK_F1;
        a[0x3C] = kVK_F2;
        a[0x3D] = kVK_F3;
        a[0x3E] = kVK_F4;
        a[0x3F] = kVK_F5;
        a[0x40] = kVK_F6;
        a[0x41] = kVK_F7;
        a[0x42] = kVK_F8;
        a[0x43] = kVK_F9;
        a[0x44] = kVK_F10;
        a[0x45] = kVK_ANSI_KeypadClear; // NumLock
        // 0x46 ScrollLock: no Mac equivalent
        a[0x47] = kVK_ANSI_Keypad7;
        a[0x48] = kVK_ANSI_Keypad8;
        a[0x49] = kVK_ANSI_Keypad9;
        a[0x4A] = kVK_ANSI_KeypadMinus;
        a[0x4B] = kVK_ANSI_Keypad4;
        a[0x4C] = kVK_ANSI_Keypad5;
        a[0x4D] = kVK_ANSI_Keypad6;
        a[0x4E] = kVK_ANSI_KeypadPlus;
        a[0x4F] = kVK_ANSI_Keypad1;
        a[0x50] = kVK_ANSI_Keypad2;
        a[0x51] = kVK_ANSI_Keypad3;
        a[0x52] = kVK_ANSI_Keypad0;
        a[0x53] = kVK_ANSI_KeypadDecimal;
        a[0x56] = kVK_ISO_Section;      // ISO extra key (between LShift and Z)
        a[0x57] = kVK_F11;
        a[0x58] = kVK_F12;
        return a;
    }();

    static const std::array<uint8_t, 256> ext = []{
        std::array<uint8_t, 256> a;
        a.fill(0xFF);
        a[0x1C] = kVK_ANSI_KeypadEnter;
        a[0x1D] = kVK_RightControl;
        a[0x35] = kVK_ANSI_KeypadDivide;
        a[0x36] = kVK_RightShift;
        // 0x37 PrintScreen: no direct Mac equivalent
        a[0x38] = kVK_RightOption;
        a[0x47] = kVK_Home;
        a[0x48] = kVK_UpArrow;
        a[0x49] = kVK_PageUp;
        a[0x4B] = kVK_LeftArrow;
        a[0x4D] = kVK_RightArrow;
        a[0x4F] = kVK_End;
        a[0x50] = kVK_DownArrow;
        a[0x51] = kVK_PageDown;
        a[0x52] = kVK_Help;             // Insert → Help
        a[0x53] = kVK_ForwardDelete;
        a[0x5B] = kVK_Command;          // LWin
        a[0x5C] = kVK_RightCommand;     // RWin
        return a;
    }();

    uint8_t sc = scancode & 0xFF;
    return (scancode & 0x100) ? ext[sc] : base[sc];
}
