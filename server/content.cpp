//
//
//

#include <chrono>
#include <functional>
#include <stdexcept>
#include <thread>


#include "content.hpp"
#include "idle-request.hpp"
#include "log-wrapper.hpp"
#include "matrix.hpp"
#include "mcu-encode.hpp"
#include "serial-id.hpp"
#include "type-def.hpp"



namespace led_d
{

  namespace {
    std::chrono::seconds show_delay (5);
    std::size_t max_spi_queue_size = 50;
    std::size_t space_length = 4;

    // fixme: matrix should hold chars?
    // char_t get_char (const unix::matrix_t::column_t &column)
    // {
    //   char_t res = 0, mask = 1;
    //   for (std::size_t bit = 0; bit < unix::matrix_t::column_size; ++bit) {
    //     if (column.test (bit) == true)
    //       res |= mask;
    //     mask <<= 1;
    //   }

    //   return res;
    // }

  } // namespace anonymous

  content_t::content_t (const std::string &default_font,
                        asio::io_context &io_context,
                        mcu_queue_t &to_spi_queue)
    : m_default_font (default_font),
      m_context (io_context),
      m_timer (m_context, show_delay),
      m_to_spi_queue (to_spi_queue),
      m_go (true),
      m_request_iterator (m_request_map.end ()),
      m_render (default_font)
  {
  }

  content_t::~content_t ()
  {
    // render device closes itself
  }

  void content_t::start ()
  {
    m_timer.expires_at (std::chrono::steady_clock::now () + show_delay);
    m_timer.async_wait (std::bind (&content_t::cycle, this, std::placeholders::_1));
  }

  void content_t::stop ()
  {
    m_timer.cancel ();
  }

  void content_t::update (const request_t &request, response_t &response)
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

  void content_t::cycle (const asio::error_code &error)
  {
    if ((error)
        || (m_to_spi_queue.size () > max_spi_queue_size))
      return;

    request_t request;
    if ((next (request) == true)
        && (prepare (request) == true)) {
      show (request);
    } else {
      idle_request_t idle_request;
      if (prepare (idle_request) == true)
        show (idle_request);
    }
  }

  bool content_t::next (request_t &request)
  {
    lock_t lock (m_mutex);

    if (m_request_map.empty () == true)
      return false;

    if (m_request_iterator == m_request_map.end ())
      m_request_iterator = m_request_map.begin ();

    request = *(m_request_iterator->second);

    ++m_request_iterator;

    // {
    //   log_t::buffer_t buf;
    //   buf << "content::next request info : " << request.info;
    //   log_t::info (buf);
    // }

    return true;
  }

  bool content_t::prepare (request_t &request) const
  {
    if (request.action != request_t::action_insert)
      return false;

    if (request.duration > request_t::max_duration)
      request.duration = request_t::max_duration;

    return true;
  }

  void content_t::show (const request_t &request)
  {
    matrix_t matrix;
    if (m_render.pixelize (matrix, request.info, request.format) == false) {
      log_t::buffer_t buf;
      buf << "Failed to pixelize info related to \"" << request.tag << "\"";
      log_t::error (buf);
      return;
    }

    for (std::size_t i = 0; i < space_length; ++i)
      m_to_spi_queue.push
        (mcu::encode::join (serial::get (), MSG_ID_MONO_LED, 0));

    for (std::size_t i = 0; i < matrix.size (); ++i)
      m_to_spi_queue.push
        (mcu::encode::join
         // fixme
         (serial::get (), MSG_ID_MONO_LED, 1/*get_char (matrix.get_column (i))*/));

    {
      // fixme: debug
      log_t::buffer_t buf;
      buf << "request info: " << request.info;
      log_t::info (buf);
    }
  }

#if 0
  void content_t::start ()
  {
    m_context.
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
      std::size_t queue_size = m_pipe_ptr->queue_size ();
      if (queue_size > max_queue_size) {
          std::this_thread::sleep_for (std::chrono::milliseconds (500));
          continue;
      }
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

  void content_t::stop ()
  {
    m_go_ahead = false;
    m_condition.notify_one ();
  }

  void content_t::update (const request_t &request, response_t &response)
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

  bool content_t::next (request_t &request)
  {
    lock_t lock (m_mutex);

    if (m_request_map.empty () == true)
      return false;

    if (m_request_iterator == m_request_map.end ())
      m_request_iterator = m_request_map.begin ();

    request = *(m_request_iterator->second);

    ++m_request_iterator;

    // {
    //   log_t::buffer_t buf;
    //   buf << "content::next request info : " << request.info;
    //   log_t::info (buf);
    // }

    return true;
  }

  bool content_t::prepare (request_t &request) const
  {
    if (request.action != request_t::action_insert)
      return false;

    if (request.duration > request_t::max_duration)
      request.duration = request_t::max_duration;

    return true;
  }

  void content_t::show (const request_t &request)
  {
    unix::matrix_t matrix;
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
    {
      // debug
      log_t::buffer_t buf;
      buf << "request info: " << request.info;
      log_t::info (buf);
    }

    // lock_t lock (m_mutex);
    // m_condition.wait_for (lock, std::chrono::milliseconds (request.duration));
  }
#endif

} // namespace led_d
