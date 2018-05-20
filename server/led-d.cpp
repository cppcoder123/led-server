//
// 
//

#include <functional>

#include "launch.hpp"

#include "arg.hpp"
#include "daemon.hpp"
#include "log-wrapper.hpp"

int main (int argc, char **argv)
{
  led_d::arg_t arg;
  if (led_d::arg_t::init (arg, argc, argv) == false)
    return 101;

  led_d::log_wrapper_t::init (arg.foreground, argv[0]);

  if (arg.kill == true) {
    int status = 0;
    if ((status = daemon_pid_file_kill_wait (SIGINT, 5)) < 0)
      led_d::log_t::info ("Failed to kill the daemon");
    return (status < 0) ? 102 : 0;
  }

  pid_t pid = 0;
  if ((pid = daemon_pid_file_is_running ()) >= 0) {
    led_d::log_t::buffer_t msg;
    msg << "Daemon is running on PID file " << pid;
    led_d::log_t::error (msg);
    return 103;
  }

  led_d::log_t::buffer_t buf;
  buf << "Launching process in " << ((arg.foreground) ? "foreground" : "background")
      << " with port " << arg.port;
  led_d::log_t::info (buf);

  led_d::daemon_t daemon;
  core::launch_t launch
    (std::bind (&led_d::daemon_t::start, &daemon, std::ref(arg)),
     std::bind (&led_d::daemon_t::stop, &daemon));
  
  return (arg.foreground == true)
    ? launch.foreground () : launch.background ();
}
