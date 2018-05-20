//
//
//
#include <functional>

#include "launch.hpp"

#include "arg.hpp"
#include "daemon.hpp"
#include "init.hpp"
#include "log-wrapper.hpp"

int main (int argc, char **argv)
{
  led_info_d::arg_t arg;
  if (led_info_d::arg_t::init (arg, argc, argv) == false)
    return 101;

  led_info_d::log_wrapper_t::init (arg.foreground, argv[0]);

  if (arg.kill == true) {
    if (daemon_pid_file_kill_wait (SIGINT, 5) < 0) {
      led_info_d::log_t::info ("Failed to kill the daemon");
      return 102;
    }
    return 0;
  }

  pid_t pid = daemon_pid_file_is_running ();
  if (pid >= 0) {
    led_info_d::log_t::buffer_t msg;
    msg << "Daemon is running on PID file " << pid;
    led_info_d::log_t::error (msg);
    return 103;
  }

  led_info_d::daemon_t daemon (arg);
  led_info_d::init_t::init (daemon);

  led_info_d::log_t::buffer_t buf;
  buf << "Launching process in " << ((arg.foreground) ? "foreground" : "background");
  led_info_d::log_t::info (buf);

  core::launch_t launch
    (std::bind (&led_info_d::daemon_t::start, &daemon),
     std::bind (&led_info_d::daemon_t::stop, &daemon));

  return (arg.foreground == true)
    ? launch.foreground () : launch.background ();
}
