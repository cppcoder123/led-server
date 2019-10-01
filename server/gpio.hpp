/*
 *
 */
#ifndef GPIO_HPP
#define GPIO_HPP

#include <gpiod.h>

#include <functional>
#include <condition_variable>
#include <mutex>

#include "unix/condition-queue.hpp"

namespace led_d
{

  class gpio_t
  {
  public:

    using queue_t = unix::condition_queue_t
      <char,
       std::reference_wrapper<std::mutex>,
       std::reference_wrapper<std::condition_variable>>;

    static constexpr char interrupt_rised = 'r';
    static constexpr char interrupt_cleared = 'c';

    gpio_t (queue_t &gpio_queue);
    ~gpio_t () {};

    void start ();
    void stop ();

    // bool is_irq_raised ();      // fixme: remove me

    gpiod_chip* get_chip () {return m_chip;}

    static constexpr const char* get_consumer () {return "led-d";}

  private:

    gpiod_chip *m_chip;

    gpiod_line *m_enable;
    gpiod_line *m_irq;

    queue_t &m_queue;

  };

} // namespace led_d

#endif
