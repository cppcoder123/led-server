//
//
//
#ifndef UNIX_LOG_HPP
#define UNIX_LOG_HPP

#include <functional>

#include "dlog.h"
#include "dpid.h"

#include "log-core.hpp"

using log_t = unix::log::core_t;

namespace log
{

  void init (bool foreground, char *argv0);

  //
  //
  //

  inline void init (bool foreground, char *argv0)
  {
    daemon_pid_file_ident = daemon_log_ident = daemon_ident_from_argv0 (argv0);

    if (foreground == false)
      log_t::init (std::bind (&daemon_log, LOG_ERR, std::placeholders::_1),
                   std::bind (&daemon_log, LOG_INFO, std::placeholders::_1));
    
  }

} // namespace log_t

#endif
