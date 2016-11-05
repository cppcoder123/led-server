//
//
//
#include <functional>

#include "libdaemon/dlog.h"
#include "libdaemon/dpid.h"

#include "log-wrapper.hpp"

namespace led_info_d
{
  void log_wrapper_t::init (bool foreground, char *argv0)
  {
    daemon_pid_file_ident = daemon_log_ident = daemon_ident_from_argv0 (argv0);

    if (foreground == false)
      log_t::init (std::bind (&daemon_log, LOG_ERR, std::placeholders::_1),
                   std::bind (&daemon_log, LOG_INFO, std::placeholders::_1));

  }
  
} // namespace led_info_d
