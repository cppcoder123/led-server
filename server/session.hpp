//
//
//
#ifndef LED_D_SESSION_HPP
#define LED_D_SESSION_HPP

#include <list>
#include <memory>
#include <mutex>
#include <string>

#include "asio.hpp"

#include "unix/socket_rw.hpp"

#include "type-def.hpp"

namespace led_d
{
  class session_t : public std::enable_shared_from_this<session_t>
  {

  public:

    session_t (asio::ip::tcp::socket socket, unix_queue_t &queue);
    ~session_t () {}

    void start ();

    void write (const std::string &info);

  private:

    void write_cb ();
    void read_cb (std::string &info);

    asio::ip::tcp::socket m_socket;

    unix_queue_t &m_queue;

    static const unsigned m_max_size = 1024;
    unix::socket_rw_t<m_max_size> m_rw;

    std::list<std::string> m_postponed_write;
  };

  using session_ptr_t = std::shared_ptr<session_t>;

} // namespace led_d

#endif
