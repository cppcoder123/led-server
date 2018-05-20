//
//
//
#ifndef LED_INFO_D_PRIORITY_HPP
#define LED_INFO_D_PRIORITY_HPP

namespace led_info_d
{
  struct priority_t
  {
    enum id_t {
      high,
      medium,
      low
    };
  };

  typedef priority_t::id_t priority_id_t;
  
} // namespace led_info_d

#endif
