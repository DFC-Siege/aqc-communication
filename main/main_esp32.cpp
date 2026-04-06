#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>

#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/uart_types.h>

#include "base_transporter.hpp"
#include "chunked_transporter.hpp"
#include "console_logger.hpp"
#include "direct_transporter.hpp"
#include "dispatcher.hpp"
#include "i_logger.hpp"
#include "logger.hpp"
#include "multiplexer.hpp"
#include "result.hpp"
#include "serial_hal.hpp"
#include "serial_transporter.hpp"
#include "serializer.hpp"

static constexpr auto TAG = "main";

enum Channel : transport::TransporterId {
        Chunked,
        Direct,
};

enum Command : transport::CommandId {
        SCD,
};

struct SCDData {
        uint16_t co2 = 0;
        float temperature = 0.0f;
        float humidity = 0.0f;
        std::string error;

        serializer::Data serialize() const {
                serializer::Writer w;
                w.write(co2);
                w.write(temperature);
                w.write(humidity);
                w.write(error);
                return std::move(w.buf);
        }

        static result::Result<SCDData> deserialize(serializer::DataView buf) {
                serializer::Reader r{buf};
                return result::ok(SCDData{
                    .co2 = TRY(r.read<uint16_t>()),
                    .temperature = TRY(r.read<float>()),
                    .humidity = TRY(r.read<float>()),
                    .error = TRY(r.read_string()),
                });
        }
};

extern "C" void app_main() {
        auto logger = std::make_unique<logging::ConsoleLogger>();
        logger->set_level(logging::LogLevel::Info);
        logging::Logger::set(std::move(logger));

        static constexpr auto MTU = 17;
        static constexpr auto MAX_TRIES = 3;
        static constexpr auto TIMEOUT = std::chrono::milliseconds(1000);

        static constexpr auto BAUDRATE = 115200;
        static constexpr auto BUFFER_SIZE = 1024;
        static constexpr auto UART = UART_NUM_1;
        static constexpr auto TX_PIN = 7;
        static constexpr auto RX_PIN = 6;
        serial::SerialHal serial_hal(UART, TX_PIN, RX_PIN, BAUDRATE,
                                     BUFFER_SIZE);
        transport::SerialTransporter serial_transporter(serial_hal, MTU);
        transport::Multiplexer multiplexer(serial_transporter);

        using MuxChannel =
            transport::Multiplexer<transport::SerialTransporter>::InnerChannel;
        using ChunkedMuxChannel = transport::ChunkedTransporter<MuxChannel>;
        using DirectMuxChannel = transport::DirectTransporter<MuxChannel>;

        auto &inner_chunked_channel =
            multiplexer.create_inner_channel(Channel::Chunked);
        auto chunked = std::make_unique<ChunkedMuxChannel>(
            inner_chunked_channel, MAX_TRIES, TIMEOUT);

        auto &inner_direct_channel =
            multiplexer.create_inner_channel(Channel::Direct);
        auto direct = std::make_unique<DirectMuxChannel>(inner_direct_channel);

        transport::Dispatcher<transport::BaseTransporter> dispatcher;
        dispatcher.register_transporter(Channel::Chunked, std::move(chunked));
        dispatcher.register_transporter(Channel::Direct, std::move(direct));

        dispatcher.register_handler(
            Command::SCD, [](result::Result<transport::Data> result) {
                    if (result.failed()) {
                            logging::logger().println(logging::LogLevel::Error,
                                                      TAG, result.error());
                            return;
                    }
                    auto buffer = result.value();
                    const auto deserialize_result =
                        SCDData::deserialize(buffer);
                    if (deserialize_result.failed()) {
                            logging::logger().println(
                                logging::LogLevel::Error, TAG,
                                deserialize_result.error());
                            return;
                    }
                    const auto &data = deserialize_result.value();
                    logging::logger().println_fmt(
                        logging::LogLevel::Info,
                        "SCD co2: {} temp: {:.2f} humidity: {:.2f} error: {}",
                        data.co2, data.temperature, data.humidity, data.error);
            });

        xTaskCreate(
            [](void *arg) {
                    auto *hal = static_cast<serial::SerialHal *>(arg);
                    while (true) {
                            hal->loop();
                    }
            },
            "serial", 8192, &serial_hal, 5, nullptr);

        vTaskDelete(nullptr);

        while (true) {
                vTaskDelay(pdMS_TO_TICKS(1));
        }
}
