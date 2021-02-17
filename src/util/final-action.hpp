//
//
//
#ifndef UTIL_FINAL_ACTION_HPP
#define UTIL_FINAL_ACTION_HPP

namespace util
{
  template <typename action_t>
  struct final_action_t
  {
    final_action_t (action_t action) : m_engage (true), m_action (action) {}
    ~final_action_t () {if (m_engage == true) m_action ();}

    void relax () {m_engage = false;}

  private:
    bool m_engage;
    action_t m_action;
  };

  template <typename action_t>
  final_action_t<action_t> make_final_action (action_t action)
  {
    return final_action_t<action_t>(action);
  }
  
} // namespace util

#endif
