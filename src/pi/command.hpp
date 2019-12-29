/*
 * Command to invoke in bash shell
 */
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <functional>
#include <memory>
#include <limits>
#include <list>
#include <string>

#include "asio/asio.hpp"

#include "command-id.hpp"
#include "popen.hpp"

namespace led_d
{
  class command_t
  {
  public:
    using popen_ptr_t = std::shared_ptr<popen_t>;
    using timeout_t = int;
    using timer_t = asio::steady_timer;
    using timer_ptr_t = std::shared_ptr<timer_t>;

    command_t () = delete;
    command_t (command_id_t id, std::string body, timeout_t time_out);
    ~command_t ();

    bool stream () const;
    bool semi_stream () const;
    bool wrap () const;

    command_id_t id () const {return m_id;}
    const std::string& body () const {return m_body;}
    timeout_t timeout () const {return m_timeout;}

    void cancel_timeout ();

    static timeout_t infinity ();
    static timeout_t three_seconds () {return 3;}

    void popen (popen_ptr_t popen_ptr);
    popen_ptr_t popen () const {return m_popen;}

    void timer (timer_ptr_t timer) {m_timer = timer;}

    void result (const std::string &info);
    std::string result () const;

  private:

    command_id_t m_id;
    std::string m_body;
    timeout_t m_timeout;

    timer_ptr_t m_timer;
    popen_ptr_t m_popen;

    std::list<std::string> m_result_list;
  };
}

#endif
