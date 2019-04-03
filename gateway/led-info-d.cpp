//
//
//
#include <functional>

#include "unix/launch.hpp"
#include "unix/log.hpp"

#include "arg.hpp"
#include "daemon.hpp"

int main (int argc, char **argv)
{
  led_info_d::arg_t arg;
  if (led_info_d::arg_t::init (arg, argc, argv) == false)
    return 101;

  log::init (arg.foreground, argv[0]);

  if (arg.kill == true) {
    if (daemon_pid_file_kill_wait (SIGINT, 5) < 0) {
      log_t::info ("Failed to kill the daemon");
      return 102;
    }
    return 0;
  }

  pid_t pid = daemon_pid_file_is_running ();
  if (pid >= 0) {
    log_t::buffer_t msg;
    msg << "Daemon is running on PID file " << pid;
    log_t::error (msg);
    return 103;
  }

  asio::io_context context;
  led_info_d::daemon_t daemon (arg, context);

  log_t::buffer_t buf;
  buf << "Launching process in " << ((arg.foreground) ? "foreground" : "background");
  log_t::info (buf);

  unix::launch_t launch
    (arg.foreground,
     std::bind (&led_info_d::daemon_t::start, &daemon),
     [&context] () {context.stop ();},
     [&context] () {context.run ();},
     context, 500);

  if (launch.start () == false) {
    log_t::buffer_t msg;
    msg << "Led gateway: Failed to start";
    log_t::error (msg);
  }
    
  return 0;
}
