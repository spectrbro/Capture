#include <ApplicationServices/ApplicationServices.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <algorithm>
#include <iostream>
#include "protocol.hpp"
#include "translate.hpp"

CFSocketRef cfSock;
sockaddr_in sockAddress;
CGRect screen = CGDisplayBounds(CGMainDisplayID());

CGEventSourceRef g_eventSource = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);

float cur_x = 0;
float cur_y = 0;

bool left_mouse_held = false;

void postEvent(CGPoint point, CGEventType type, CGMouseButton button)
{
    CGEventRef e = CGEventCreateMouseEvent(g_eventSource, type, point, button);
    CGEventPost(kCGHIDEventTap, e);
    CFRelease(e);
}

void handleMouseMove(const decltype(Packet::data.mouse_event)* packet)
{
    cur_x += packet->x;
    cur_y += packet->y;
    cur_x = std::max(0.0f, std::min(cur_x, (float)screen.size.width-1));
    cur_y = std::max(0.0f, std::min(cur_y, (float)screen.size.height-1));
    CGPoint point = {cur_x, cur_y};

    switch (packet->button) {
        case 1:
        {
            left_mouse_held = true;
            postEvent(point, kCGEventLeftMouseDown, kCGMouseButtonLeft);
            break;
        }
        case -1:
        {
            left_mouse_held = false;
            postEvent(point, kCGEventLeftMouseUp, kCGMouseButtonLeft);
            break;
        }
        case 2:
        {
            postEvent(point,kCGEventRightMouseDown, kCGMouseButtonRight);
            break;
        }
        case -2:
        {
            postEvent(point, kCGEventRightMouseUp, kCGMouseButtonRight);
            break;
        }
        default:
            break;
    }
    if (packet->x != 0 || packet->y != 0)
        postEvent(point, left_mouse_held ? kCGEventLeftMouseDragged : kCGEventMouseMoved, kCGMouseButtonLeft);
}

void handleMouseScroll(const decltype(Packet::data.mouse_wheel)* packet)
{
    CGEventRef e = CGEventCreateScrollWheelEvent(g_eventSource, kCGScrollEventUnitLine, 1, packet->delta / 120);
    CGEventPost(kCGHIDEventTap, e);
    CFRelease(e);
}

void handleKeyboard(const decltype(Packet::data.keyboard)* packet)
{
    uint32_t key = translateKey(packet->keycode);
    if (key == 0xFF) return;

    CGEventRef e = CGEventCreateKeyboardEvent(g_eventSource, (CGKeyCode)key, packet->down);
    CGEventPost(kCGHIDEventTap, e);
    CFRelease(e);
}

void socketCallback(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void* data, void* info)
{
    if (CFDataGetLength((CFDataRef)data) < (CFIndex)sizeof(Packet)) return;
    const Packet* packet = reinterpret_cast<const Packet*>(CFDataGetBytePtr((CFDataRef)data));
    switch (packet->type) {
        case Packet::MOUSE: handleMouseMove(&packet->data.mouse_event); break;
        case Packet::KEYBOARD: handleKeyboard(&packet->data.keyboard); break;
        case Packet::MOUSE_WHEEL: handleMouseScroll(&packet->data.mouse_wheel); break;
    }
}

void signalHandler(int)
{
    CFRunLoopStop(CFRunLoopGetCurrent());
}

int main()
{
    cfSock = CFSocketCreate(kCFAllocatorDefault, AF_INET, SOCK_DGRAM, IPPROTO_UDP, kCFSocketDataCallBack, socketCallback, nullptr);
    sockAddress.sin_family = AF_INET;
    sockAddress.sin_port = htons(9000);
    sockAddress.sin_addr.s_addr = INADDR_ANY;

    int ipbind = bind(CFSocketGetNative(cfSock),(struct sockaddr*)&sockAddress, sizeof(sockAddress));
    if (ipbind < 0) {std::cout << "Failed to bind socket" << std::endl; return 0;}

    CGEventRef tmp = CGEventCreate(nullptr);
    CGPoint current = CGEventGetLocation(tmp);
    cur_x = (float)current.x;
    cur_y = (float)current.y;
    CFRelease(tmp);

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    CFRunLoopSourceRef source = CFSocketCreateRunLoopSource(kCFAllocatorDefault, cfSock, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopDefaultMode);
    CFRunLoopRun();

    CFRunLoopSourceInvalidate(source);
    CFRelease(source);
    CFSocketInvalidate(cfSock);
    CFRelease(cfSock);
}
