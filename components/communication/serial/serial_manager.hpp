#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace Communication {
struct Listener {
        uint32_t id;
        std::function<void(std::string)> function;
};

class SerialManager {
      public:
        SerialManager();
        void send(std::string value);
        uint32_t add_listener(std::function<void(std::string)> function);
        void remove_listener(uint32_t id);
        void update();

      private:
        static constexpr auto BAUDRATE = 115200;
        static constexpr auto TX_PIN = 7;
        static constexpr auto RX_PIN = 6;
        static constexpr auto BUF_SIZE = 1024;
        std::vector<Listener> listeners;
        uint32_t last_id = 0;
};
} // namespace Communication
