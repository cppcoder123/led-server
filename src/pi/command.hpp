/*
 * Command to invoke in bash shell
 */
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <memory>
#include <string>

#include "command-id.hpp"
#include "popen.hpp"

namespace led_d
{
  class command_t
  {
  public:
    using popen_ptr_t = std::shared_ptr<popen_t>;

    command_t () = delete;
    command_t (command_id_t id, std::string body);
    // command_t (command_id_t primary_id,
    // command_id_t secondary_id, std::string body);
    ~command_t () = default;

    bool stream () const;
    bool wrap () const {return !stream ();}

    // bool simple () const {return m_primary_id == m_secondary_id;}

    const std::string& body () const {return m_body;}

    void popen (popen_ptr_t popen_ptr);

  private:

    command_id_t m_id;
    // command_id_t m_primary_id;
    // command_id_t m_secondary_id;
    std::string m_body;
  };
}

#endif
