//
// launch daemon in background or foreground mode
//
#ifndef CORE_LAUNCH_HPP
#define CORE_LAUNCH_HPP

#include <errno.h>
#include <signal.h>
#include <string.h>

#include <functional>

#include "dexec.h"
#include "dfork.h"
#include "dpid.h"
#include "dsignal.h"

#include "log.hpp"


namespace core
{

  class launch_t
  {

  public:

    typedef std::function<int (void)> start_t;
    typedef std::function<void (void)> stop_t;

    launch_t (start_t start_function, stop_t stop_function);
    ~launch_t () {}

    
    int foreground ();
    int background ();

  private:

    int cleanup (bool fground);
    bool signal_init ();
    void wait_for_signal ();

    start_t m_start_function;
    stop_t m_stop_function;
  };


  inline launch_t::launch_t (start_t start_function,
                             stop_t stop_function)
    : m_start_function (start_function),
      m_stop_function (stop_function)
  {
  }

  inline int launch_t::foreground ()
  {
    if (m_start_function () != 0)
      return 201;

    if (signal_init () == false)
      return 202;

    wait_for_signal ();

    return cleanup (true);
  }

  inline int launch_t::background ()
  {
    daemon_retval_init ();
    pid_t pid = 0;
    if ((pid = daemon_fork ()) < 0) {
      // failed to fork
      daemon_retval_done ();
      return 111;
    } else if (pid != 0) { // the parent
      int status = 0;
      // wait 20 seconds
      if ((status = daemon_retval_wait (20)) < 0) {
        log_t::error
          ("Failed to receive return value from daemon process");
        return 112;
      }
      log_t::buffer_t msg;
      msg << "Daemon returned " << status << " as return value ";
      if (status == 0)
        log_t::info (msg);
      else
        log_t::error (msg);
      return status;              // parent exit
    } else { // the daemon
      if (daemon_pid_file_create () < 0) {
        log_t::buffer_t msg;
        msg << "Failed to create PID file - " << strerror (errno);
        log_t::error (msg);
        daemon_retval_send (1);
        return cleanup (false);
      }
      
      if (signal_init () == false) {
        daemon_retval_send (2);
        return cleanup (false);
      }
      
      if (m_start_function () == false) {
        daemon_retval_send (3);
        return cleanup (false);
      }
      
      daemon_retval_send (0);

      log_t::info ("Successfully started");
      
      wait_for_signal ();
      
      return cleanup (false);
    }
  }

  int launch_t::cleanup (bool fground)
  {
    log_t::info ("Exiting...");
    m_stop_function ();
    daemon_signal_done ();

    if (fground == false)
      daemon_pid_file_remove ();

    return 0;
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

  void launch_t::wait_for_signal ()
  {
    bool quit = false;
    int fd = daemon_signal_fd ();
    fd_set initial_set;

    FD_ZERO (&initial_set);
    FD_SET (fd, &initial_set);

    while (quit == false) {
      fd_set current_set = initial_set;
      if (select (FD_SETSIZE, &current_set, 0, 0, 0) < 0) {
        if (errno == EINTR)
          continue;
        log_t::buffer_t msg;
        msg << "select (): " << strerror (errno);
        log_t::error (msg);
        break;
      }
      if (FD_ISSET (fd, &current_set)) {
        int sig = SIGINT;
        
        if ((sig = daemon_signal_next ()) <= 0) { //  '<=' or '<'
          log_t::error ("daemon_signal_next () failed.");
          break;
        }

        switch (sig) {
        case SIGINT:
        case SIGQUIT:
        case SIGTERM:
          log_t::info ("SIGINT/SIGQUIT/SIGTERM has arrived");
          quit = true;
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
  }

    
} // namespace core

#endif
