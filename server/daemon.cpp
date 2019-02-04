//
//
//

#include "unix/network-codec.hpp"
#include "unix/refsymbol.hpp"
#include "unix/request.hpp"
#include "unix/response.hpp"

#include "daemon.hpp"
#include "log-wrapper.hpp"
#include "network.hpp"
#include "session.hpp"

namespace led_d
{

  daemon_t::daemon_t ()
    : m_network (m_asio_context, m_network_queue),
      m_content (m_asio_context),
      m_spi (m_spi_queue),
      m_handle (m_network_queue, m_spi_queue, m_content, m_spi)
  {
  }

  daemon_t::~daemon_t ()
  {
  }

  int daemon_t::start (const arg_t &arg)
  {
    try {
      m_network_thread = std::thread (&network_t::start, &m_network);
      m_handle_thread = std::thread (&handle_t::start, &m_handle);

      m_spi_thread = std::thread (&spi_t::start, &m_spi);
    }

    catch (std::exception &e) {
      log_t::error (std::string ("Daemon: ") + e.what ());
      return 11;
    }

    return 0;
  }

  void daemon_t::stop ()
  {
    m_network.stop ();
    m_handle.stop ();
    m_spi.stop ();

    m_network_thread.join ();
    m_handle_thread.join ();
    m_spi_thread.join ();

    // m_asio_service.stop ();
    // m_network_thread.join ();
  }

  // fixme: move this into network
  // void daemon_t::network_load (const arg_t &arg)
  // {
  //   {
  //     log_t::buffer_t buf;
  //     buf << "Daemon: Starting network with port " << arg.port;
  //     log_t::info (buf);
  //   }

  //   try {
  //     network_t network (m_asio_service, arg.port, m_message_queue);
  //     m_asio_service.run ();
  //   }
  //   catch (std::exception &e) {
  //     log_t::buffer_t buf;
  //     buf << "Daemon: network exception - " << e.what ();
  //     log_t::error (buf);
  //   }

  //   {
  //     log_t::buffer_t buf;
  //     buf << "Daemon: Network service is stopped";
  //     log_t::info (buf);
  //   }
  // }
  
  // void daemon_t::update_load ()
  // {
  //   using refsymbol_t = core::refsymbol_t;
  //   using request_t = core::request_t;
  //   using response_t = core::response_t;
  //   using request_codec_t = core::network::codec_t<refsymbol_t, request_t>;
  //   using response_codec_t = core::network::codec_t<refsymbol_t, response_t>;

  //   //
  //   while (m_update_go == true) {
  //     auto optional_message = m_message_queue.pop ();
  //     if (optional_message.has_value () == false)
  //       continue;
  //     message_ptr_t &message_ptr (*optional_message);
  //     request_t request;
  //     response_t response;

  //     std::string buffer;
  //     // 
  //     if (request_codec_t::decode (message_ptr->info, request) == false) {
  //       response.status = 1;
  //       response.reason = "Failed to decode request message";
  //       log_t::error (response.reason);
  //       if (response_codec_t::encode (response, buffer) == true)
  //         message_ptr->sender->send (buffer);
  //       continue;
  //     }

  //     m_content.update (request, response);
  //     if (response_codec_t::encode (response, buffer) == true)
  //       message_ptr->sender->send (buffer);
  //   }
  // }
  
} // namespace led_d
