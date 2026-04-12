#pragma once

#include "result.hpp"
#include "transport_data.hpp"

namespace handlers {
class BaseHandler {
        virtual void handle(result::Result<transport::Data> result) = 0;
};
} // namespace handlers
