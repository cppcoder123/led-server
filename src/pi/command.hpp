/*
 * Command to invoke in bash shell
 */
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

namespace led_d
{
  class command_t
  {
  public:

    enum id_t {
      ID_MPC_PLAY,
    };

    command_t () = delete;
    command_t (id_t id, std::string body);
    command_t (id_t primary_id, id_t secondary_id, std::string body);
    ~command_t () = default;

    bool simple () const {return m_primary_id == m_secondary_id;}

    const std::string& body () const {return m_body;}

  private:

    id_t m_primary_id;
    id_t m_secondary_id;
    std::string m_body;
  };
}

#endif
