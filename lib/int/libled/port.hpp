//
// Default port value for led7-d and led7 apps
//
#ifndef LIBLED_PORT_HPP
#define LIBLED_PORT_HPP

namespace libled
{

  struct port_t
  {

    typedef unsigned short int value_t;
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
