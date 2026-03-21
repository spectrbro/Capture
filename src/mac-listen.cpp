#include <IOKit/hidsystem/IOHIDLib.h>
#include <IOKit/hidsystem/IOLLEvent.h>
#include <IOKit/IOKitLib.h>

int main() {
    io_service_t service = IOServiceGetMatchingService(kIOMainPortDefault, IOServiceMatching("IOHIDSystem"));
}
