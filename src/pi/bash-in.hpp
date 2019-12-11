//
//
//
#ifndef BASH_IN_HPP
#define BASH_IN_HPP

#include <list>
#include <memory>

#include "asio/asio.hpp"

#include "bash-queue.hpp"
#include "popen.hpp"

namespace led_d
{
  class bash_in_t
  {

  public:

    bash_in_t (asio::io_context &io_context, bash_queue_t &queue);
    ~bash_in_t () {};

    void start ();
    void stop ();

    // enum source_t {
    //   MPD,
    //   SYS,
    //   CLOCK,
    //   SOURCE_SIZE               // do not use
    // };
    // bool kick (source_t src);

  private:

    // void handle_popen (popen_ptr_t mpd, const asio::error_code &errc);

    asio::io_context &m_context;
    bash_queue_t &m_queue;

    using popen_ptr_t = std::shared_ptr<popen_t>;
    std::list<popen_ptr_t> m_in_list;
    // using source_vector_t = std::vector<popen_ptr_t>;
    // source_vector_t m_source_vector;
  };
} // namespace led_d

#endif
