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

#include "bash-queue.hpp"

namespace led_d
{
  class session_t : public std::enable_shared_from_this<session_t>
  {

  public:

    session_t (asio::ip::tcp::socket socket, bash_queue_t &queue);
    ~session_t ();

    void start ();

    void write (const std::string &info);

    using disconnect_t = std::function<void (void)>;
    void set_disconnect (disconnect_t cb);

  private:

    void write_cb ();
    void read_cb (std::string &info);
    void disconnect_cb ();

    asio::ip::tcp::socket m_socket;

    bash_queue_t &m_queue;

    static const unsigned m_max_size = 1024;
    unix::socket_rw_t<m_max_size> m_rw;

    std::list<std::string> m_postponed_write;

    disconnect_t m_disconnect;
  };

  using session_ptr_t = std::shared_ptr<session_t>;

} // namespace led_d

#endif
