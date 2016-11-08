//
//
//
#include <iostream>

#include "ledhw/ht1632c.hpp"

#include "window.hpp"

namespace ledhw
{
  typedef std::lock_guard<std::mutex> guard_t;
  typedef std::unique_lock<std::mutex> lock_t;

  const std::chrono::milliseconds ht1632c_t::m_fixed_delay (10000);
  const std::chrono::milliseconds ht1632c_t::m_scroll_delay (1000);
  const std::chrono::milliseconds ht1632c_t::m_scroll_shift_delay (300);
  //const std::chrono::milliseconds ht1632c_t::m_scroll_shift_delay (0);

  
  ht1632c_t::ht1632c_t ()
    : m_go_ahead (true)
  {
  }

  bool ht1632c_t::start ()
  {
    m_thread = std::thread (&ht1632c_t::load, this);

    return true;
  }
  
  void ht1632c_t::stop ()
  {
    m_go_ahead = false;
    m_condition.notify_one ();
    m_thread.join ();
  }
  
  bool ht1632c_t::render (const libled::matrix_t &matrix)
  {
    guard_t g (m_mutex);
    m_buffer = matrix;

    m_condition.notify_one ();
    
    return true;
  }

  void ht1632c_t::load ()
  {
    while (m_go_ahead == true) {
      update ();
      bool scroll (m_matrix.size () > m_fixed_limit);
      
      if (scroll == true)
        render_scroll ();
      else
        render_fixed ();

      {
        lock_t lock (m_mutex);
        if ((m_buffer.empty () == true)
            && (m_go_ahead == true))
          m_condition.wait_for
            (lock, (scroll == true) ? m_scroll_delay : m_fixed_delay);
      }
    }
  }

  void ht1632c_t::render_fixed ()
  {
    clear ();
    //
    window_t window (m_matrix);
    for (std::size_t i = 0; i < m_fixed_limit; ++i) {
      const column_t &column (window.get_column (i));
      render_column (column, i);
    }
  }

  void ht1632c_t::render_scroll ()
  {
    clear ();
    //
    window_t
      window (m_matrix, m_fixed_limit), window_prev (m_matrix, m_fixed_limit);
    std::size_t max_shift (m_matrix.size () + 2 * m_fixed_limit);
    for (std::size_t i = 0; i < max_shift; ++i) {
      window.left_shift ();
      for (std::size_t j = 0; j < m_fixed_limit; ++j) {
        const column_t &column (window.get_column (j));
        const column_t &prev_column (window_prev.get_column (j));
        if (column != prev_column)
          render_column (column, j);
      }
      window_prev.left_shift ();
      {
        lock_t lock (m_mutex);
        m_condition.wait_for (lock, m_scroll_shift_delay);
      }
    }
  }

  void ht1632c_t::render_column (const column_t &column, std::size_t position)
  {
    // fixme
    for (std::size_t i = 0; i < column.size (); ++i)
      std::cout << ((column.test (i) == true) ? 'O' : ' ');
    std::cout << std::endl;
  }

  void ht1632c_t::update ()
  {
    guard_t guard (m_mutex);

    if (m_buffer.empty () == true)
      return;

    m_matrix = m_buffer;
    m_buffer.clear ();
  }
  
} // namespace ledhw
