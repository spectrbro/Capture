#pragma once
#include <cstdint>
#include <Carbon/Carbon.h>

// 100% Claude generated
// PS/2 Set 1 scan codes → Mac CGKeyCodes
// Base scan codes (no E0 prefix)
// Extended scan codes have bit 8 set (E0 prefix)
static uint32_t translateKey(uint16_t scancode)
{
    static const uint8_t base[256] = {
        [0x01] = kVK_Escape,
        [0x02] = kVK_ANSI_1,
        [0x03] = kVK_ANSI_2,
        [0x04] = kVK_ANSI_3,
        [0x05] = kVK_ANSI_4,
        [0x06] = kVK_ANSI_5,
        [0x07] = kVK_ANSI_6,
        [0x08] = kVK_ANSI_7,
        [0x09] = kVK_ANSI_8,
        [0x0A] = kVK_ANSI_9,
        [0x0B] = kVK_ANSI_0,
        [0x0C] = kVK_ANSI_Minus,
        [0x0D] = kVK_ANSI_Equal,
        [0x0E] = kVK_Delete,           // Backspace
        [0x0F] = kVK_Tab,
        [0x10] = kVK_ANSI_Q,
        [0x11] = kVK_ANSI_W,
        [0x12] = kVK_ANSI_E,
        [0x13] = kVK_ANSI_R,
        [0x14] = kVK_ANSI_T,
        [0x15] = kVK_ANSI_Y,
        [0x16] = kVK_ANSI_U,
        [0x17] = kVK_ANSI_I,
        [0x18] = kVK_ANSI_O,
        [0x19] = kVK_ANSI_P,
        [0x1A] = kVK_ANSI_LeftBracket,
        [0x1B] = kVK_ANSI_RightBracket,
        [0x1C] = kVK_Return,
        [0x1D] = kVK_Control,          // LCtrl
        [0x1E] = kVK_ANSI_A,
        [0x1F] = kVK_ANSI_S,
        [0x20] = kVK_ANSI_D,
        [0x21] = kVK_ANSI_F,
        [0x22] = kVK_ANSI_G,
        [0x23] = kVK_ANSI_H,
        [0x24] = kVK_ANSI_J,
        [0x25] = kVK_ANSI_K,
        [0x26] = kVK_ANSI_L,
        [0x27] = kVK_ANSI_Semicolon,
        [0x28] = kVK_ANSI_Quote,
        [0x29] = kVK_ANSI_Grave,
        [0x2A] = kVK_Shift,            // LShift
        [0x2B] = kVK_ANSI_Backslash,
        [0x2C] = kVK_ANSI_Z,
        [0x2D] = kVK_ANSI_X,
        [0x2E] = kVK_ANSI_C,
        [0x2F] = kVK_ANSI_V,
        [0x30] = kVK_ANSI_B,
        [0x31] = kVK_ANSI_N,
        [0x32] = kVK_ANSI_M,
        [0x33] = kVK_ANSI_Comma,
        [0x34] = kVK_ANSI_Period,
        [0x35] = kVK_ANSI_Slash,
        [0x36] = kVK_RightShift,
        [0x37] = kVK_ANSI_KeypadMultiply,
        [0x38] = kVK_Option,           // LAlt
        [0x39] = kVK_Space,
        [0x3A] = kVK_CapsLock,
        [0x3B] = kVK_F1,
        [0x3C] = kVK_F2,
        [0x3D] = kVK_F3,
        [0x3E] = kVK_F4,
        [0x3F] = kVK_F5,
        [0x40] = kVK_F6,
        [0x41] = kVK_F7,
        [0x42] = kVK_F8,
        [0x43] = kVK_F9,
        [0x44] = kVK_F10,
        [0x45] = kVK_ANSI_KeypadClear, // NumLock
        // 0x46 ScrollLock: no Mac equivalent
        [0x47] = kVK_ANSI_Keypad7,
        [0x48] = kVK_ANSI_Keypad8,
        [0x49] = kVK_ANSI_Keypad9,
        [0x4A] = kVK_ANSI_KeypadMinus,
        [0x4B] = kVK_ANSI_Keypad4,
        [0x4C] = kVK_ANSI_Keypad5,
        [0x4D] = kVK_ANSI_Keypad6,
        [0x4E] = kVK_ANSI_KeypadPlus,
        [0x4F] = kVK_ANSI_Keypad1,
        [0x50] = kVK_ANSI_Keypad2,
        [0x51] = kVK_ANSI_Keypad3,
        [0x52] = kVK_ANSI_Keypad0,
        [0x53] = kVK_ANSI_KeypadDecimal,
        [0x56] = kVK_ISO_Section,      // ISO extra key (between LShift and Z)
        [0x57] = kVK_F11,
        [0x58] = kVK_F12,
    };

    static const uint8_t ext[256] = {
        [0x1C] = kVK_ANSI_KeypadEnter,
        [0x1D] = kVK_RightControl,
        [0x35] = kVK_ANSI_KeypadDivide,
        [0x36] = kVK_RightShift,
        // 0x37 PrintScreen: no direct Mac equivalent
        [0x38] = kVK_RightOption,
        [0x47] = kVK_Home,
        [0x48] = kVK_UpArrow,
        [0x49] = kVK_PageUp,
        [0x4B] = kVK_LeftArrow,
        [0x4D] = kVK_RightArrow,
        [0x4F] = kVK_End,
        [0x50] = kVK_DownArrow,
        [0x51] = kVK_PageDown,
        [0x52] = kVK_Help,             // Insert → Help
        [0x53] = kVK_ForwardDelete,
        [0x5B] = kVK_Command,          // LWin
        [0x5C] = kVK_RightCommand,     // RWin
    };

    // Uninitialized entries default to 0, which is kVK_ANSI_A — use 0xFF as sentinel
    // The designated initializer approach leaves gaps as 0, so we need a separate check.
    // Rebuild using 0xFF as the default by checking known valid entries only.
    uint8_t sc = scancode & 0xFF;
    uint8_t key = (scancode & 0x100) ? ext[sc] : base[sc];
    // A result of 0 could mean kVK_ANSI_A (valid) or an unmapped gap.
    // Re-check: only sc values explicitly listed above are valid.
    // Since 0 == kVK_ANSI_A and scan code 0x1E maps to it, we can't use 0 as sentinel.
    // Instead, maintain a validity bitmask.
    static const bool base_valid[256] = {
        [0x01]=1,[0x02]=1,[0x03]=1,[0x04]=1,[0x05]=1,[0x06]=1,[0x07]=1,[0x08]=1,
        [0x09]=1,[0x0A]=1,[0x0B]=1,[0x0C]=1,[0x0D]=1,[0x0E]=1,[0x0F]=1,[0x10]=1,
        [0x11]=1,[0x12]=1,[0x13]=1,[0x14]=1,[0x15]=1,[0x16]=1,[0x17]=1,[0x18]=1,
        [0x19]=1,[0x1A]=1,[0x1B]=1,[0x1C]=1,[0x1D]=1,[0x1E]=1,[0x1F]=1,[0x20]=1,
        [0x21]=1,[0x22]=1,[0x23]=1,[0x24]=1,[0x25]=1,[0x26]=1,[0x27]=1,[0x28]=1,
        [0x29]=1,[0x2A]=1,[0x2B]=1,[0x2C]=1,[0x2D]=1,[0x2E]=1,[0x2F]=1,[0x30]=1,
        [0x31]=1,[0x32]=1,[0x33]=1,[0x34]=1,[0x35]=1,[0x36]=1,[0x37]=1,[0x38]=1,
        [0x39]=1,[0x3A]=1,[0x3B]=1,[0x3C]=1,[0x3D]=1,[0x3E]=1,[0x3F]=1,[0x40]=1,
        [0x41]=1,[0x42]=1,[0x43]=1,[0x44]=1,[0x45]=1,[0x47]=1,[0x48]=1,[0x49]=1,
        [0x4A]=1,[0x4B]=1,[0x4C]=1,[0x4D]=1,[0x4E]=1,[0x4F]=1,[0x50]=1,[0x51]=1,
        [0x52]=1,[0x53]=1,[0x56]=1,[0x57]=1,[0x58]=1,
    };
    static const bool ext_valid[256] = {
        [0x1C]=1,[0x1D]=1,[0x35]=1,[0x36]=1,[0x38]=1,
        [0x47]=1,[0x48]=1,[0x49]=1,[0x4B]=1,[0x4D]=1,
        [0x4F]=1,[0x50]=1,[0x51]=1,[0x52]=1,[0x53]=1,
        [0x5B]=1,[0x5C]=1,
    };

    bool valid = (scancode & 0x100) ? ext_valid[sc] : base_valid[sc];
    return valid ? key : 0xFF;
}
