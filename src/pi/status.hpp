/*
 *
 */

#ifndef STATUS_HPP
#define STATUS_HPP

#include <string>

#include "command-id.hpp"

namespace led_d
{

  class status_t
  {
  public:
    status_t (command_id::value_t id, int value, const std::string &out);
    ~status_t () = default;

    command_id::value_t id () const {return m_id;}
    int value () const {return m_value;}
    const std::string& out () const {return m_out;}

    bool stream () const {return command_id::stream (m_id);}

    static int good () {return 0;}
    static int bad ();
    static int timeout ();

  private:

    command_id::value_t m_id;
    int m_value;                // status value
    std::string m_out;          // command output
    
  };

} // led_d

#endif
