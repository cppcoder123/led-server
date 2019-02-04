//
//
//
#ifndef CORE_FINAL_ACTION_HPP
#define CORE_FINAL_ACTION_HPP

namespace core
{
  template <typename action_t>
  struct final_action_t
  {
    final_action_t (action_t action) : m_action (action) {}
    ~final_action_t () {m_action ();}

  private:
    action_t m_action;
  };

  template <typename action_t>
  final_action_t<action_t> make_final_action (action_t action)
  {
    return final_action_t<action_t>(action);
  }
  
} // namespace core

#endif
