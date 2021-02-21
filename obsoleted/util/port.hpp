//
// Default port value for led-d
//
#ifndef UNIX_PORT_HPP
#define UNIX_PORT_HPP

namespace unix
{

  struct port_t
  {

    using value_t = unsigned short int;
    //
    static value_t default_port ();
  };

  //
  //
  //

  inline port_t::value_t port_t::default_port ()
  {
    // "The Dynamic and/or Private Ports are those from 49152 through 65535"
    return 51234;
  }
  
}

#endif
