//
//
//
#ifndef UTIL_FINAL_ACTION_HPP
#define UTIL_FINAL_ACTION_HPP

#include <functional>

namespace util
{
  class final_action_t
  {
  public:
    using action_t = std::function<void ()>;

    final_action_t (action_t cb) : m_action (cb) {}
    ~final_action_t () { m_action ();}

    void relax () {m_action = [](){};}

  private:
    action_t m_action;
  };

} // namespace util

#endif
