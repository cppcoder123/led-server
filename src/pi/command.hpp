/*
 * Command to invoke in bash shell
 */
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <memory>
#include <limits>
#include <list>
#include <string>

#include "command-id.hpp"
#include "popen.hpp"

namespace led_d
{
  class command_t
  {
  public:
    using popen_ptr_t = std::shared_ptr<popen_t>;
    using timeout_t = int;

    command_t () = delete;
    command_t (command_id_t id, std::string body, timeout_t time_out);
    // command_t (command_id_t primary_id,
    // command_id_t secondary_id, std::string body);
    ~command_t () = default;

    bool stream () const;
    bool wrap () const {return !stream ();}

    // bool simple () const {return m_primary_id == m_secondary_id;}

    command_id_t id () const {return m_id;}
    const std::string& body () const {return m_body;}
    timeout_t timeout () const {return m_timeout;}

    static timeout_t infinity_timeout ();

    void popen (popen_ptr_t popen_ptr);
    popen_ptr_t popen () const {return m_popen;}

    void result (const std::string &info);
    std::string result () const;

  private:

    command_id_t m_id;
    // command_id_t m_primary_id;
    // command_id_t m_secondary_id;
    std::string m_body;
    timeout_t m_timeout;

    popen_ptr_t m_popen;

    std::list<std::string> m_result_list;
  };
}

#endif
