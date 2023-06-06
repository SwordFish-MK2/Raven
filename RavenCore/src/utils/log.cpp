#include <Raven/utils/log.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace Raven {
std::shared_ptr<spdlog::logger> Log::logger;

void Log::init() {
  spdlog::set_pattern("%^[%T] %n: %v$");
  logger = spdlog::stdout_color_mt("Raven");
  logger->set_level(spdlog::level::trace);
}
}  // namespace Raven