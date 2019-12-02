//
//
//

#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <string>
#include <utility>

namespace led_d
{

  class content_t
  {
  public:

    content_t ();
    ~content_t ();

    void in (std::string info);

    using out_info_t = std::pair<std::string/*info*/, std::string/*format*/>;
    out_info_t out ();
      
  };
  
} // led_d

#endif
