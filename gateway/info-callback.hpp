//
//
//

#ifndef INFO_CALLBACK_HPP
#define INFO_CALLBACK_HPP

#include <functional>
#include <string>
#include <vector>

namespace led_info_d
{

  using callback_t = std::function<bool (std::string&/*info*/)>;

  using callback_vector_t = std::vector<callback_t>;
  
} // namespace led_info_d

#endif
