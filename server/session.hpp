//
//
//
#ifndef LED_D_SESSION_HPP
#define LED_D_SESSION_HPP

#include <memory>
#include <mutex>
#include <string>

#include "asio.hpp"

#include "type-def.hpp"

namespace led_d
{
  class session_t : public std::enable_shared_from_this<session_t>
  {
    
  public:
    
    session_t (asio::ip::tcp::socket socket, unix_queue_t &queue)
      : m_socket (std::move (socket)),
        m_queue (queue)
    {
      m_raw_read_buf[0] = 0;
      m_raw_write_buf[0] = 0;
    }
    ~session_t () {}

    void start ();

    void send (const std::string &info);
    
  private:

    void do_read ();
    void do_write ();

    asio::ip::tcp::socket m_socket;


    static const unsigned m_max_size = 1024;
    char m_raw_read_buf[m_max_size + 1];
    char m_raw_write_buf[m_max_size + 1];

    std::string m_read_buf, m_write_buf;

    //typedef std::mutex mutex_t;
    //typedef std::lock_guard<mutex_t> guard_t;

    //mutex_t m_write_mutex;

    unix_queue_t &m_queue;
  };

  using session_ptr_t = std::shared_ptr<session_t>;
  
} // namespace led_d

#endif
