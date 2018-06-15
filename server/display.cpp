//
//
//

#include <chrono>
#include <stdexcept>
#include <thread>

#include "matrix.hpp"

#include "display.hpp"
#include "idle-request.hpp"
#include "log-wrapper.hpp"
#include "pipe.hpp"


namespace led_d
{
  display_t::display_t ()
    : m_go_ahead (true),
      m_request_iterator (m_request_map.end ())
  {
  }

  display_t::~display_t ()
  {
    // render device closes itself
  }

  void display_t::start (const arg_t &arg, serial_t &serial)
  {
    //
    try {
      m_pipe_ptr = std::make_unique<pipe_t>(serial);
    }
    catch (std::exception &e) {
      log_t::buffer_t msg ("Failed to create render device : ");
      msg << e.what ();
      log_t::error (msg);
      return;
    }

    if (m_render.init (arg) == false) {
      log_t::error ("Failed to init render-er");
      return;
    }

    m_arg = arg;
    //
    request_t request;
    //
    while (m_go_ahead == true) {
      if ((next (request) == true)
          && (prepare (request) == true)) {
        show (request);
      } else {
        idle_request_t idle_request;
        if (prepare (idle_request) == true)
          show (idle_request);
        else
          std::this_thread::sleep_for (std::chrono::milliseconds (100));
      }
    }
  }

  void display_t::stop ()
  {
    m_go_ahead = false;
    m_condition.notify_one ();
  }

  void display_t::update (const request_t &request, response_t &response)
  {
    if (request.action == request_t::action_idle) {
      response.status = response_t::status_ok;
      response.reason = "idle action";
      return;
    }

    lock_t lock (m_mutex);

    request_map_t::iterator iter (m_request_map.find (request.tag));
    if (iter == m_request_map.end ()) {
      if (request.action == request_t::action_insert) {
        m_request_iterator =
          (m_request_map.insert
           (request_map_t::value_type
            (request.tag, request_ptr_t (new request_t (request))))).first;
        response.status = response_t::status_ok;
        response.reason = "New entry is inserted";
      } else if (request.action == request_t::action_erase) {
        log_t::buffer_t buf;
        buf << "Can't erase tag \"" << request.tag << "\" related info";
        log_t::info (buf);
        response.status = response_t::status_error;
        response.reason = buf.str ();
      }
    } else {                    // tag exists in the map
      if (request.action == request_t::action_insert) {
        iter->second = request_ptr_t (new request_t (request));
        response.reason = "Info map entry is updated";
        m_request_iterator = iter;
      } else if (request.action == request_t::action_erase) {
        m_request_map.erase (iter);
        response.reason = "Info map entry is erased";
        m_request_iterator = m_request_map.end ();
      }
      response.status = response_t::status_ok;
    }
  }

  bool display_t::next (request_t &request)
  {
    lock_t lock (m_mutex);
    
    if (m_request_map.empty () == true)
      return false;

    if (m_request_iterator == m_request_map.end ())
      m_request_iterator = m_request_map.begin ();

    request = *(m_request_iterator->second);
    
    ++m_request_iterator;

    return true;
  }

  bool display_t::prepare (request_t &request) const
  {
    if (request.action != request_t::action_insert)
      return false;

    if (request.duration > request_t::max_duration)
      request.duration = request_t::max_duration;

    return true;
  }

  void display_t::show (const request_t &request)
  {
    core::matrix_t matrix;
    if (m_render.pixelize (matrix, request.info, request.format) == false) {
      log_t::buffer_t buf;
      buf << "Failed to pixelize info related to \"" << request.tag << "\"";
      log_t::error (buf);
      return;
    }

    if (m_pipe_ptr->render (matrix) == false) {
      log_t::buffer_t buf;
      buf << "Driver failed to render info related to \"" << request.tag << "\"";
      // fixme
      log_t::error (buf);
      return;
    }
    
    /*fixme: pass to hw library*/
    // matrix.dump ();
    //
    // fixme : pass data to hardware dependent library
    //
    // debug
    // log_t::buffer_t buf;
    // buf << " info : " << request.info;
    // log_t::complain (LOG_INFO, buf.str ());
    
    lock_t lock (m_mutex);
    m_condition.wait_for (lock, std::chrono::milliseconds (request.duration));
  }

} // namespace led_d
