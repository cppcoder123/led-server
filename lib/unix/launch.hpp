//
// Handle exiting signals signals
//
//
#ifndef LAUNCH_HPP
#define LAUNCH_HPP

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>

#include <chrono>
#include <functional>

#include "dexec.h"
#include "dfork.h"
#include "dpid.h"

#include "asio/asio.hpp"

#include "log.hpp"

namespace unix
{

  class launch_t
  {

  public:

    using start_t = std::function<bool (void)>;
    using stop_t = std::function<void (void)>;
    using run_t = std::function<void (void)>;

    launch_t (bool foreground,
              start_t start_function,
              stop_t stop_function,
              run_t run_function,
              asio::io_context &context);
    ~launch_t () {}

    bool start ();

  private:

    void stop (bool clean_pid_file, bool invoke_stop_callback);

    bool foreground ();
    bool background ();

    void signal_handler (const asio::error_code &errc, int sig_number);

    bool m_foreground;
    start_t m_start_function;
    stop_t m_stop_function;
    run_t m_run_function;

    asio::signal_set m_signal_set;
  };


  inline launch_t::launch_t (bool fground,
                             start_t start_function,
                             stop_t stop_function,
                             run_t run_function,
                             asio::io_context &context)
    : m_foreground (fground),
      m_start_function (start_function),
      m_stop_function (stop_function),
      m_run_function (run_function),
      m_signal_set (context)
  {
    m_signal_set.add (SIGINT);
    m_signal_set.add (SIGQUIT);
    m_signal_set.add (SIGHUP);
    m_signal_set.add (SIGTERM);

    m_signal_set.async_wait
      (std::bind (&launch_t::signal_handler, this,
                  std::placeholders::_1, std::placeholders::_2));
  }

  inline bool launch_t::start ()
  {
    return (m_foreground == true) ? foreground () : background ();
  }

  inline bool launch_t::foreground ()
  {
    if (m_start_function () == false)
      return false;

    m_run_function ();

    return true;
  }

  inline bool launch_t::background ()
  {
    daemon_retval_init ();
    pid_t pid = 0;
    if ((pid = daemon_fork ()) < 0) {
      // failed to fork
      daemon_retval_done ();
      return false;
    } else if (pid != 0) { // the parent
      int status = 0;
      // wait 20 seconds
      if ((status = daemon_retval_wait (20)) < 0) {
        log_t::error
          ("Failed to receive return value from daemon process");
        return false;
      }
      log_t::buffer_t msg;
      msg << "Daemon returned " << status << " as return value ";
      if (status == 0)
        log_t::info (msg);
      else
        log_t::error (msg);
      return (status == 0) ? true : false; // parent exit
    } else { // the daemon
      if (daemon_pid_file_create () < 0) {
        log_t::buffer_t msg;
        msg << "Failed to create PID file - " << strerror (errno);
        log_t::error (msg);
        daemon_retval_send (1);
        stop (false, false);
        return false;
      }

      if (m_start_function () == false) {
        daemon_retval_send (3);
        stop (true, false);
        return false;
      }

      daemon_retval_send (0);

      log_t::info ("Daemon successfully started");

      m_run_function ();

      return true;
    }
  }

  void launch_t::stop (bool pid_file, bool stop_callback)
  {
    log_t::info ("Exiting...");

    if (stop_callback == true)
      m_stop_function ();

    m_signal_set.cancel ();

    if ((m_foreground == false)
        && (pid_file == true))
      daemon_pid_file_remove ();
  }

  void launch_t::signal_handler (const asio::error_code &errc,
                                 int /*sig_number*/)
  {
    if (errc) {
      log_t::error ("launch: Error during signal handling");
      return;
    }

    stop (true, true);
  }

}// namespace unix

#endif
