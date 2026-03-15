#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include <IOKit/IOKitLib.h>

int main() {
    io_service_t service = IOServiceGetMatchingService(
        kIOMainPortDefault, IOServiceMatching("IOHIDSystem"));
    
    io_connect_t connect;
    IOServiceOpen(service, mach_task_self(), kIOHIDParamConnectType, &connect);
    IOObjectRelease(service);

    IOServiceClose(connect);
}
