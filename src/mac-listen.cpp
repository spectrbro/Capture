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

CGEventSourceRef g_eventSource = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
CGEventFlags g_modifiers = 0x0;

float cur_x = 0;
float cur_y = 0;

bool left_mouse_held = false;

CFAbsoluteTime g_lastClickTime = 0;
int g_clickCount = 0;

void postEvent(CGPoint point, CGEventType type, CGMouseButton button, int click)
{
    CGEventRef e = CGEventCreateMouseEvent(g_eventSource, type, point, button);
    CGEventSetIntegerValueField(e, kCGMouseEventClickState, click);
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
            left_mouse_held = true;
            g_clickCount = (CFAbsoluteTimeGetCurrent() - g_lastClickTime < 0.5) ? g_clickCount + 1 : 1;
            g_lastClickTime = CFAbsoluteTimeGetCurrent();
            postEvent(point, kCGEventLeftMouseDown, kCGMouseButtonLeft, g_clickCount);
            break;
        case -1:
            left_mouse_held = false;
            postEvent(point, kCGEventLeftMouseUp, kCGMouseButtonLeft, g_clickCount);
            break;
        case 2: postEvent(point,kCGEventRightMouseDown, kCGMouseButtonRight, 1); break;
        case -2: postEvent(point, kCGEventRightMouseUp, kCGMouseButtonRight, 1); break;
        default: break;
    }
    if (packet->x != 0 || packet->y != 0)
        postEvent(point, left_mouse_held ? kCGEventLeftMouseDragged : kCGEventMouseMoved, kCGMouseButtonLeft, 0);
}

void handleMouseKeys(const decltype(Packet::data.mouse_keys)* packet)
{
    CGPoint point = {cur_x, cur_y};

    switch (packet->button) {
        case 3: postEvent(point, kCGEventOtherMouseDown, kCGMouseButtonCenter, 1); break;
        case -3: postEvent(point, kCGEventOtherMouseUp, kCGMouseButtonCenter, 1); break;
        case 4: postEvent(point, kCGEventOtherMouseDown, (CGMouseButton)3, 1); break;
        case -4: postEvent(point, kCGEventOtherMouseUp, (CGMouseButton)3, 1); break;
        case 5: postEvent(point, kCGEventOtherMouseDown, (CGMouseButton)4, 1); break;
        case -5: postEvent(point, kCGEventOtherMouseUp, (CGMouseButton)4, 1); break;
        default: break;
    }
}

void handleMouseScroll(const decltype(Packet::data.mouse_wheel)* packet)
{
    CGEventRef e = CGEventCreateScrollWheelEvent(g_eventSource, kCGScrollEventUnitLine, 1, packet->delta / 40);
    CGEventPost(kCGHIDEventTap, e);
    CFRelease(e);
}

void handleKeyboard(const decltype(Packet::data.keyboard)* packet)
{
    uint32_t key = translateKey(packet->keycode);
    if (key == 0xFF) return;
    bool down = packet->down;

    // update modifier state
    switch(key) {
        case kVK_Shift:        case kVK_RightShift:
            down ? (g_modifiers |= kCGEventFlagMaskShift)       : (g_modifiers &= ~kCGEventFlagMaskShift);     break;
        case kVK_Control:      case kVK_RightControl:
            down ? (g_modifiers |= kCGEventFlagMaskControl)     : (g_modifiers &= ~kCGEventFlagMaskControl);   break;
        case kVK_Option:       case kVK_RightOption:
            down ? (g_modifiers |= kCGEventFlagMaskAlternate)   : (g_modifiers &= ~kCGEventFlagMaskAlternate); break;
        case kVK_Command:      case kVK_RightCommand:
            down ? (g_modifiers |= kCGEventFlagMaskCommand)     : (g_modifiers &= ~kCGEventFlagMaskCommand);   break;
        case kVK_Function:
            down ? (g_modifiers |= kCGEventFlagMaskSecondaryFn) : (g_modifiers &= ~kCGEventFlagMaskSecondaryFn); break;
        case kVK_CapsLock:
            if (down) g_modifiers ^= kCGEventFlagMaskAlphaShift; break;
    }

    CGEventRef e = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)key, down);

    switch(key) {
        case kVK_CapsLock:
        case kVK_Shift:        case kVK_RightShift:
        case kVK_Control:      case kVK_RightControl:
        case kVK_Option:       case kVK_RightOption:
        case kVK_Command:      case kVK_RightCommand:
        case kVK_Function:
            CGEventSetType(e, kCGEventFlagsChanged);
            CGEventSetFlags(e, g_modifiers);
            break;
        case kVK_ANSI_Keypad0: case kVK_ANSI_Keypad1: case kVK_ANSI_Keypad2:
        case kVK_ANSI_Keypad3: case kVK_ANSI_Keypad4: case kVK_ANSI_Keypad5:
        case kVK_ANSI_Keypad6: case kVK_ANSI_Keypad7: case kVK_ANSI_Keypad8:
        case kVK_ANSI_Keypad9: case kVK_ANSI_KeypadDecimal:
        case kVK_ANSI_KeypadPlus: case kVK_ANSI_KeypadMinus:
        case kVK_ANSI_KeypadMultiply: case kVK_ANSI_KeypadDivide:
        case kVK_ANSI_KeypadEnter: case kVK_ANSI_KeypadClear:
            CGEventSetFlags(e, g_modifiers | kCGEventFlagMaskNumericPad);
            break;
        case kVK_LeftArrow: case kVK_RightArrow:
        case kVK_UpArrow:   case kVK_DownArrow:
            CGEventSetFlags(e, g_modifiers | kCGEventFlagMaskNumericPad | kCGEventFlagMaskSecondaryFn); // Arrow events for some reason append to the mask of arrows (it seems): kCGEventFlagMaskNumericPad kCGEventFlagMaskSecondaryFn
            break;
        default:
            CGEventSetFlags(e, g_modifiers);
            break;
    }

    CGEventPost(kCGHIDEventTap, e);
    CFRelease(e);
}


void socketCallback(CFSocketRef s, CFSocketCallBackType type, CFDataRef address, const void* data, void* info)
{
    if (CFDataGetLength((CFDataRef)data) < (CFIndex)sizeof(Packet)) return;
    const Packet* packet = reinterpret_cast<const Packet*>(CFDataGetBytePtr((CFDataRef)data));
    switch (packet->type) {
        case Packet::MOUSE: handleMouseMove(&packet->data.mouse_event); break;
        case Packet::MOUSE_KEYS: handleMouseKeys(&packet->data.mouse_keys); break;
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
