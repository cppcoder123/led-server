//
//
//

#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <list>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <utility>

namespace led_d
{

  class content_t
  {
  public:

    content_t () = delete;
    content_t (const std::list<std::string> &regexp_list);
    ~content_t ();

    void in (std::string info);

    using out_info_t = std::pair<std::string/*info*/, std::string/*format*/>;
    out_info_t out ();

  private:

    std::string replace (const std::string &src);

    std::list<std::string> m_sys_info;

    using map_t = std::map<std::string/*info src*/, std::string/*info*/>;
    map_t m_info;
    map_t::iterator m_iterator;

    using regex_ptr = std::shared_ptr<std::regex>;
    using find_replace_t = std::pair<regex_ptr/*find*/, std::string/*replace*/>;
    using regex_list_t = std::list<find_replace_t>;
    regex_list_t m_regex_list;

    static const std::regex m_regex;
  };
  
} // led_d

#endif
