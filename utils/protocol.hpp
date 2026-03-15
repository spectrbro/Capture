#pragma once
#include <cstdint>

struct Packet {
    enum Type : uint8_t {
        MOUSE,
        KEYBOARD,
        MOUSE_WHEEL
    } type;

    union {
        struct { float x, y; int8_t button; } mouse_event;
        struct { uint16_t keycode; bool down; } keyboard;
        struct { int16_t delta; } mouse_wheel;
    } data;
};
