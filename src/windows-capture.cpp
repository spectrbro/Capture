#include <cstdint>
#include <cstdlib>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#include <algorithm>
#include "protocol.hpp"

typedef unsigned __int64 QWORD;

SOCKET sock;
sockaddr_in dest{};

HWND g_hwnd = nullptr;
HHOOK keyHook = nullptr;
HHOOK mouseHook = nullptr;

static UINT  g_bufferSize = 64 * sizeof(RAWINPUT);
static void* g_pBuffer    = nullptr;

void SendPacket(const Packet& p)
{
    sendto(sock, reinterpret_cast<const char*>(&p), sizeof(p), 0,
           reinterpret_cast<sockaddr*>(&dest), sizeof(dest));
}

LRESULT CALLBACK KeyHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0) return CallNextHookEx(nullptr, nCode, wParam, lParam);
    KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    uint16_t scancode = static_cast<uint16_t>(kb->scanCode);
    bool extended = (kb->flags & LLKHF_EXTENDED) != 0;
    bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    if (kb->vkCode == VK_PRIOR) { if (down) PostMessage(g_hwnd, WM_APP, 0, 0); return 1; }
    uint16_t key = scancode | (extended ? 0x100 : 0);
    SendPacket({Packet::KEYBOARD, {.keyboard = {key, down}}});
    return 1;
}

LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0) return CallNextHookEx(nullptr, nCode, wParam, lParam);
    return 1; // always swallow when hook installed
}

void Install()
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

void Uninstall()
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

void ProcessInput(const RAWINPUT* raw)
{
    if (raw->header.dwType != RIM_TYPEMOUSE) return;

    auto dx          = [&]{ return static_cast<float>(raw->data.mouse.lLastX); };
    auto dy          = [&]{ return static_cast<float>(raw->data.mouse.lLastY); };
    auto wheel_delta = [&]{ return static_cast<int16_t>(raw->data.mouse.usButtonData); };
    uint16_t flags   = raw->data.mouse.usButtonFlags;

    if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)  SendPacket({Packet::MOUSE, {.mouse_event = {dx(), dy(),  1}}});
    if (flags & RI_MOUSE_LEFT_BUTTON_UP)    SendPacket({Packet::MOUSE, {.mouse_event = {dx(), dy(), -1}}});
    if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN) SendPacket({Packet::MOUSE, {.mouse_event = {dx(), dy(),  2}}});
    if (flags & RI_MOUSE_RIGHT_BUTTON_UP)   SendPacket({Packet::MOUSE, {.mouse_event = {dx(), dy(), -2}}});
    if (flags & RI_MOUSE_BUTTON_3_DOWN)     SendPacket({Packet::MOUSE_KEYS, {.mouse_keys = { 3}}});
    if (flags & RI_MOUSE_BUTTON_3_UP)       SendPacket({Packet::MOUSE_KEYS, {.mouse_keys = {-3}}});
    if (flags & RI_MOUSE_BUTTON_4_DOWN)     SendPacket({Packet::MOUSE_KEYS, {.mouse_keys = { 4}}});
    if (flags & RI_MOUSE_BUTTON_4_UP)       SendPacket({Packet::MOUSE_KEYS, {.mouse_keys = {-4}}});
    if (flags & RI_MOUSE_BUTTON_5_DOWN)     SendPacket({Packet::MOUSE_KEYS, {.mouse_keys = { 5}}});
    if (flags & RI_MOUSE_BUTTON_5_UP)       SendPacket({Packet::MOUSE_KEYS, {.mouse_keys = {-5}}});
    if (flags & RI_MOUSE_WHEEL)             SendPacket({Packet::MOUSE_WHEEL, {.mouse_wheel = {wheel_delta()}}});

    if (!flags) {
        float x = dx(), y = dy();
        if (x != 0 || y != 0) SendPacket({Packet::MOUSE, {.mouse_event = {x, y, 0}}});
    }
}

void DrainRawInputQueue()
{
    for (;;)
    {
        UINT bufferSize = g_bufferSize;
        UINT count = GetRawInputBuffer((RAWINPUT*)g_pBuffer, &bufferSize, sizeof(RAWINPUTHEADER));
        if (count == 0) break;
        if (count == (UINT)-1)
        {
            g_bufferSize = std::max(bufferSize, g_bufferSize * 2);
            g_pBuffer = realloc(g_pBuffer, g_bufferSize);
            if (!g_pBuffer) break;
            continue;
        }
        RAWINPUT* r = (RAWINPUT*)g_pBuffer;
        for (UINT i = 0; i < count; ++i, r = NEXTRAWINPUTBLOCK(r))
            ProcessInput(r);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_INPUT)
    {
        UINT bufferSize = g_bufferSize;
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, g_pBuffer, &bufferSize, sizeof(RAWINPUTHEADER)) != (UINT)-1)
            ProcessInput((RAWINPUT*)g_pBuffer);
        DrainRawInputQueue();
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    if (msg == WM_APP)
    {
        Uninstall();
        RegisterHotKey(hwnd, 1, 0, VK_PRIOR);
        return 0;
    }
    if (msg == WM_HOTKEY)
    {
        UnregisterHotKey(hwnd, 1);
        Install();
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    g_pBuffer = malloc(g_bufferSize);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(9000);
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    inet_pton(AF_INET, "10.10.10.230", &dest.sin_addr);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RawInput";
    RegisterClassEx(&wc);

    g_hwnd = CreateWindowEx(0, "RawInput", nullptr, WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInstance, nullptr);

    Install();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    Uninstall();
    DestroyWindow(g_hwnd);
    UnregisterClass("RawInput", GetModuleHandle(nullptr));
    WSACleanup();
    closesocket(sock);
    free(g_pBuffer);
    return 0;
}
