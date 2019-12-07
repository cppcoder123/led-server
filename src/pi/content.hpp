//
//
//

#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <list>
#include <regex>
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

  private:

    std::list<std::string> m_info_list;

    static const std::regex m_regex;
  };
  
} // led_d

#endif
