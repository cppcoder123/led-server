//
// Not used
//
#ifndef LEDHW_HT1632C_HPP
#define LEDHW_HT1632C_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "matrix.hpp"

#include "hw.hpp"

namespace ledhw
{

  class ht1632c_t : public hw_t
  {

  public:

    ht1632c_t ();
    ~ht1632c_t () {}

    bool start ();
    void stop ();

    bool render (const libled::matrix_t &matrix);
    bool brigtness (int level) {/*fixme*/return true;}

  private:

    void load ();               // what thread is doing

    void render_scroll ();
    void render_fixed ();

    typedef libled::matrix_t::column_t column_t;
    void render_column (const column_t &column, std::size_t position);
    
    void update ();

    void clear () {/*fixme*/}
    
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::thread m_thread;
    bool m_go_ahead;

    libled::matrix_t m_buffer, m_matrix;

    static const std::size_t m_fixed_limit = 8; // more => need to scroll (configurable?)

    static const std::chrono::milliseconds m_fixed_delay; // ~10 sec
    static const std::chrono::milliseconds m_scroll_delay; // ~1 sec
    static const std::chrono::milliseconds m_scroll_shift_delay; // ~0.3 sec
    
    // typedef std::lock_guard<std::mutex> guard_t;
    // typedef std::unique_lock<std::mutex> lock_t;

  };
  
} // namespace ledhw

#endif
