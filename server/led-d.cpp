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
  using namespace led_d;

  arg_t arg;
  if (arg_t::init (arg, argc, argv) == false)
    return 101;

  log::init (arg.foreground, argv[0]);

  if (arg.kill == true) {
    int status = 0;
    if ((status = daemon_pid_file_kill_wait (SIGINT, 5)) < 0)
      log_t::info ("Failed to kill the daemon");
    return (status < 0) ? 102 : 0;
  }

  log_t::buffer_t msg;

  pid_t pid = 0;
  if ((pid = daemon_pid_file_is_running ()) >= 0) {
    msg << "Daemon is running on PID file " << pid;
    log_t::error (msg);
    return 103;
  }

  msg << "Launching process in "
      << ((arg.foreground) ? "foreground" : "background")
      << " with port " << arg.port;
  log_t::info (msg);
  log_t::clear (msg);

  int status = 0;
  try {
    daemon_t daemon (arg);
    auto &context = daemon.get_context ();

    unix::launch_t launch
      (arg.foreground,
       std::bind (&daemon_t::start, &daemon),
       std::bind (&daemon_t::stop, &daemon),
       [&context] {context.run ();},
       context, 500/*2 times per second*/);

    if (launch.start () == false) {
      log_t::buffer_t buf;
      buf << "led-d: Failed to start daemon";
      log_t::error (buf);
    }

    // log_t::buffer_t buf;
    // buf << "Daemon is stopped, exiting";
    // log_t::info (buf);
  }
  catch (std::exception &e) {
    status = 128;
    msg << "Error during start-up: \"" << e.what () << "\"";
    log_t::error (msg);
  }

  return status;
}
