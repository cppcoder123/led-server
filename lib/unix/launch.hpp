//
// Handle exiting signals signals
//
// fixme: rename to 'launch.hpp' later
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
#include "dsignal.h"

#include "asio/asio.hpp"

#include "log.hpp"

namespace unix
{

  class launch_t
  {

  public:

    using start_t = std::function<bool (void)>;
    using stop_t = std::function<void (void)>;

    launch_t (bool foreground,
              start_t start_function,
              stop_t stop_function,
              asio::io_context &context,
              unsigned int timer_interval/*milliseconds*/);
    ~launch_t () {}

    bool start ();

  private:

    void stop (bool clean_pid_file, bool invoke_stop_callback);

    bool foreground ();
    bool background ();

    bool signal_init ();

    bool need_exit ();
    void check_signal (const asio::error_code &error);

    bool m_foreground;
    start_t m_start_function;
    stop_t m_stop_function;

    asio::io_context &m_context;
    asio::steady_timer m_timer;
    const std::chrono::milliseconds m_delay;
  };


  inline launch_t::launch_t (bool fground,
                             start_t start_function,
                             stop_t stop_function,
                             asio::io_context &context,
                             unsigned int timer_interval)
    : m_foreground (fground),
      m_start_function (start_function),
      m_stop_function (stop_function),
      m_context (context),
      m_timer (context),
      m_delay (timer_interval)
  {
  }

  inline bool launch_t::start ()
  {
    bool status = (m_foreground == true) ? foreground () : background ();
    if (status == false)
      return false;

    check_signal (asio::error_code {});

    return true;
  }

  inline bool launch_t::foreground ()
  {
    if (m_start_function () == false)
      return false;

    if (signal_init () == false)
      return false;

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

      if (signal_init () == false) {
        daemon_retval_send (2);
        stop (true, false);
        return false;
      }

      if (m_start_function () == false) {
        daemon_retval_send (3);
        stop (true, false);
        return false;
      }

      daemon_retval_send (0);

      log_t::info ("Daemon successfully started");

      return true;
    }
  }

  void launch_t::stop (bool pid_file, bool stop_callback)
  {
    log_t::info ("Exiting...");

    m_timer.cancel ();

    if (stop_callback == true)
      m_stop_function ();

    daemon_signal_done ();

    if ((m_foreground == false)
        && (pid_file == true))
      daemon_pid_file_remove ();
  }

  bool launch_t::signal_init ()
  {
    bool status = true;

    if (daemon_signal_init (SIGINT, SIGQUIT, SIGHUP, SIGTERM, 0) < 0) {
      log_t::buffer_t msg;
      msg << "Failed to register signal handlers - " << strerror (errno);
      log_t::error (msg.str ());
      status = false;
    }

    return status;
  }

  bool launch_t::need_exit ()
  {
    int fd = daemon_signal_fd ();
    fd_set sig_set;

    FD_ZERO (&sig_set);
    FD_SET (fd, &sig_set);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if (select (FD_SETSIZE, &sig_set, 0, 0, &timeout) < 0) {
      if (errno == EINTR)
        return false;
      log_t::buffer_t msg;
      msg << "select (): " << strerror (errno);
      log_t::error (msg);
      return true;
    }

    if (FD_ISSET (fd, &sig_set)) {
      int sig = SIGINT;

      while ((sig = daemon_signal_next ()) > 0) {
        switch (sig) {
        case SIGINT:
        case SIGQUIT:
        case SIGTERM:
          log_t::info ("SIGINT/SIGQUIT/SIGTERM has arrived");
          return true;
          break;
        case SIGHUP:
          log_t::info ("SIGHUP has arrived");
          //daemon_exec ("/", 0, "/bin/ls", "ls", 0);
          break;
        default:
          log_t::info ("Unknown signal has arrived");
          break;
        }
      }
    }

    return false;
  }

  void launch_t::check_signal (const asio::error_code &error)
  {
    if (error) {
      // asio error ?
      stop (true, true);
      return;
    }

    if (need_exit () == false) {
      // just reschedule
      m_timer.expires_at (std::chrono::steady_clock::now () + m_delay);
      m_timer.async_wait
        (std::bind
         (&launch_t::check_signal, this, std::placeholders::_1));
    } else
      stop (true, true);
  }

}// namespace unix

#endif
