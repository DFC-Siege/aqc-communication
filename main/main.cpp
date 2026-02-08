#include <cstdio>

#include "serial/serial_manager.hpp"

extern "C" {
void app_main(void) {
        Communication::SerialManager serial_manager;
}
}
