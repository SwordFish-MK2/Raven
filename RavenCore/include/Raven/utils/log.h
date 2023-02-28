#ifndef _RAVEN_UTILS_LOG_H_
#define _RAVEN_UTILS_LOG_H_

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace Raven {
class Log {
 public:
  static void init();

  inline static std::shared_ptr<spdlog::logger> getLogger() { return logger; }

 private:
  static std::shared_ptr<spdlog::logger> logger;
};
#define RERROR(...) ::Raven::Log::getLogger()->error(__VA_ARGS__)
#define RTRACE(...) ::Raven::Log::getLogger()->trace(__VA_ARGS__)
#define RWARN(...) ::Raven::Log::getLogger()->warn(__VA_ARGS__)
#define RINFO(...) ::Raven::Log::getLogger()->info(__VA_ARGS__)

}  // namespace Raven

#endif