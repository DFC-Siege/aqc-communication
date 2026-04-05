#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
#include <unordered_map>

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

static constexpr auto TAG = "main";

enum Channel : transport::TransporterId {
        Chunked,
        Direct,
};

enum Command : transport::CommandId {
        Ping,
};

int main(int argc, char *argv[]) {
        auto logger = std::make_unique<logging::ConsoleLogger>();
        logger->set_level(logging::LogLevel::Info);
        logging::Logger::set(std::move(logger));

        if (argc < 2) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          "usage: communication <device>");
                return 1;
        }

        const auto DEVICE = argv[1];
        static constexpr uint16_t MTU = 255;
        static constexpr uint16_t MAX_TRIES = 1;

        serial::SerialHal serial_hal(DEVICE);
        transport::SerialTransporter serial_transporter(serial_hal, MTU);
        transport::Multiplexer multiplexer(serial_transporter);

        using MuxChannel =
            transport::Multiplexer<transport::SerialTransporter>::InnerChannel;
        using ChunkedMuxChannel = transport::ChunkedTransporter<MuxChannel>;
        using DirectMuxChannel = transport::DirectTransporter<MuxChannel>;

        auto &inner_chunked_channel =
            multiplexer.create_inner_channel(Channel::Chunked);
        auto chunked = std::make_unique<ChunkedMuxChannel>(
            inner_chunked_channel, MAX_TRIES);

        auto &inner_direct_channel =
            multiplexer.create_inner_channel(Channel::Direct);
        auto direct = std::make_unique<DirectMuxChannel>(inner_direct_channel);

        transport::Dispatcher<transport::BaseTransporter> dispatcher;
        dispatcher.register_transporter(Channel::Chunked, std::move(chunked));
        dispatcher.register_transporter(Channel::Direct, std::move(direct));

        dispatcher.register_handler(
            Command::Ping, [](result::Result<transport::Data> result) {
                    if (result.failed()) {
                            logging::logger().println(logging::LogLevel::Error,
                                                      TAG, result.error());
                            return;
                    }
                    const auto data = std::move(result).value();
                    logging::logger().println(
                        logging::LogLevel::Info, TAG,
                        std::string(data.begin(), data.end()));
            });

        static constexpr std::string_view msg =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
            "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
            "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor "
            "in reprehenderit in voluptate velit esse cillum dolore eu fugiat "
            "nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
            "sunt in culpa qui officia deserunt mollit anim id est laborum. "
            "Sed ut perspiciatis unde omnis iste natus error sit voluptatem "
            "accusantium doloremque laudantium, totam rem aperiam, eaque ipsa "
            "quae ab illo inventore veritatis et quasi architecto beatae vitae "
            "dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas "
            "sit aspernatur aut odit aut fugit, sed quia consequuntur magni "
            "dolores eos qui ratione voluptatem sequi nesciunt. Neque porro "
            "quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, "
            "adipisci velit, sed quia non numquam eius modi tempora incidunt "
            "ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim "
            "ad minima veniam, quis nostrum exercitationem ullam corporis "
            "suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur. "
            "Quis autem vel eum iure reprehenderit qui in ea voluptate velit "
            "esse quam nihil molestiae consequatur, vel illum qui dolorem eum ";
        const auto send_result =
            dispatcher.send(Channel::Chunked, Command::Ping,
                            transport::Data(msg.begin(), msg.end()));
        if (send_result.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          send_result.error());
        }

        static constexpr std::string_view msg2 = "yee";
        const auto send_result2 =
            dispatcher.send(Channel::Direct, Command::Ping,
                            transport::Data(msg2.begin(), msg2.end()));
        if (send_result2.failed()) {
                logging::logger().println(logging::LogLevel::Error, TAG,
                                          send_result2.error());
        }

        while (true) {
                serial_hal.loop();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
}
