#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "chunked_transporter.hpp"
#include "console_logger.hpp"
#include "dispatcher.hpp"
#include "logger.hpp"
#include "multiplexer.hpp"
#include "result.hpp"
#include "serial_hal.hpp"
#include "serial_transporter.hpp"

int main() {
        logging::Logger::set(std::make_unique<logging::ConsoleLogger>());
        static constexpr auto DEVICE = "/dev/ttyACM0";
        static constexpr uint16_t MTU = 255;
        static constexpr uint16_t MAX_TRIES = 1;

        serial::SerialHal serial_hal(DEVICE);
        transport::SerialTransporter serial_base(serial_hal, MTU);

        using SerialChunked =
            transport::ChunkedTransporter<transport::SerialTransporter>;
        SerialChunked chunked_serial(serial_base, MAX_TRIES);

        std::unordered_map<transport::TransporterId,
                           std::reference_wrapper<transport::BaseTransporter>>
            transporters{{0x00, chunked_serial}};

        transport::Multiplexer<transport::SerialTransporter> multiplexer(
            serial_base, std::move(transporters));
        auto channel = multiplexer.get_channel(0x00).value();
        transport::Dispatcher dispatcher(channel);

        while (true) {
                serial_hal.loop();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
}
