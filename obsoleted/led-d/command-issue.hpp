/*
 *
 */
#ifndef COMMAND_ISSUE_HPP
#define COMMAND_ISSUE_HPP

#include <memory>
#include <string>

#include "command.hpp"
#include "command-queue.hpp"

namespace led_d
{
  inline void command_issue (command_id_t id, std::string text,
                             command_t::timeout_t timeout,
                             command_queue_t &command_queue)
  {
    auto command = std::make_shared<command_t>(id, text, timeout);
    command_queue.push (command);
  }
} // led_d

#endif
