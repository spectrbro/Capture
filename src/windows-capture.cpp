#include <cstdint>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#include "protocol.hpp"

SOCKET sock;
sockaddr_in dest{};

HWND g_hwnd = nullptr;
HHOOK keyHook = nullptr;
HHOOK mouseHook = nullptr;

void sendPacket(const Packet& p)
{
    sendto(sock, reinterpret_cast<const char*>(&p), sizeof(p), 0,
           reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
}

LRESULT CALLBACK KeyHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0) return CallNextHookEx(nullptr, nCode, wParam, lParam);
    KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    uint16_t scancode = static_cast<uint16_t>(kb->scanCode); //Scan codes only ever use the lower 8 bits (`0x00`–`0xFF`)
    bool extended = (kb->flags & LLKHF_EXTENDED) != 0;
    bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    if (kb->vkCode == VK_PRIOR) { if (down) PostMessage(g_hwnd, WM_APP, 0, 0); return 1; }
    sendPacket({Packet::KEYBOARD, {.keyboard = {static_cast<uint16_t>(scancode | (extended ? 0x100 : 0)), down}}});
    return 1;
}

LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0) return CallNextHookEx(nullptr, nCode, wParam, lParam);
    return 1; // always swallow when hook installed
}

void install()
{
    RAWINPUTDEVICE rid{};
    rid.usUsagePage = 0x01;
    rid.usUsage     = 0x02;
    rid.dwFlags     = RIDEV_NOLEGACY | RIDEV_CAPTUREMOUSE | RIDEV_INPUTSINK;
    rid.hwndTarget  = g_hwnd;
    RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
    
    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyHook, GetModuleHandle(nullptr), 0);
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHook, GetModuleHandle(nullptr), 0);
}

void uninstall()
{
    RAWINPUTDEVICE rid{};
    rid.usUsagePage = 0x01;
    rid.usUsage     = 0x02;
    rid.dwFlags     = RIDEV_REMOVE;
    rid.hwndTarget  = nullptr;
    RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
    
    UnhookWindowsHookEx(keyHook);
    UnhookWindowsHookEx(mouseHook);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_INPUT)
    {
        UINT size = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
        RAWINPUT raw;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));
        switch (raw.header.dwType)
        {
            case RIM_TYPEMOUSE:
            {
                auto dx = [&]{ return static_cast<float>(raw.data.mouse.lLastX); };
                auto dy = [&]{ return static_cast<float>(raw.data.mouse.lLastY); };
                auto wheel_delta = [&]{ return static_cast<int16_t>(raw.data.mouse.usButtonData); };
                switch (raw.data.mouse.usButtonFlags)
                {
                    case RI_MOUSE_LEFT_BUTTON_DOWN: sendPacket({Packet::MOUSE, {.mouse_event = {dx(), dy(), 1}}});  break;
                    case RI_MOUSE_LEFT_BUTTON_UP: sendPacket({Packet::MOUSE, {.mouse_event = {dx(), dy(), -1}}}); break;
                    case RI_MOUSE_RIGHT_BUTTON_DOWN: sendPacket({Packet::MOUSE, {.mouse_event = {dx(), dy(), 2}}});  break;
                    case RI_MOUSE_RIGHT_BUTTON_UP: sendPacket({Packet::MOUSE, {.mouse_event = {dx(), dy(), -2}}}); break;
                    case RI_MOUSE_WHEEL: sendPacket({Packet::MOUSE_WHEEL, {.mouse_wheel = {wheel_delta()}}}); break;
                    default: { float x = dx(), y = dy(); if (x != 0 || y != 0) sendPacket({Packet::MOUSE, {.mouse_event = {x, y, 0}}}); break; }
                }
                break;
            }
            default:
                break;
        }
    }
    if (msg == WM_APP)
    {
        uninstall();
        RegisterHotKey(hwnd, 1, 0, VK_PRIOR);
        return 0;
    }
    if (msg == WM_HOTKEY)
    {
        UnregisterHotKey(hwnd, 1);
        install();
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(9000);
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    inet_pton(AF_INET, "10.10.10.230", &dest.sin_addr);
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "RawInput";
    RegisterClassEx(&wc);

    g_hwnd = CreateWindowEx(0, "RawInput", nullptr, WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    ShowWindow(g_hwnd, SW_SHOW);
    SetForegroundWindow(g_hwnd);
    
    install();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    uninstall();
    DestroyWindow(g_hwnd);
    UnregisterClass("RawInput", GetModuleHandle(nullptr));
    WSACleanup();
    closesocket(sock);
    return 0;
}
