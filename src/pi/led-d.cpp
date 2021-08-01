//
//
//

#include <signal.h>

#include <functional>

#include "asio.hpp"

#include "util/final-action.hpp"
#include "util/log.hpp"

#include "arg.hpp"
#include "base.hpp"

static void signal_handler (asio::io_context &context,
                            const asio::error_code &errc, int)
{
  if (errc) {
    log_t::error ("led-d: Error during signal handling");
    return;
  }

  context.stop ();
}

int main (int argc, char **argv)
{
  using namespace led_d;

  arg_t arg;
  if (arg_t::init (arg, argc, argv) == false)
    return 101;

  {
    log_t::buffer_t buf;
    buf << "Starting daemon ...";
    log_t::info (buf);
  }

  int status = 0;
  try {
    base_t base (arg);
    auto &context = base.get_context ();

    asio::signal_set signal_set (context);
    signal_set.add (SIGINT);
    signal_set.add (SIGQUIT);
    signal_set.add (SIGHUP);
    signal_set.add (SIGTERM);

    if (base.start () == false)
      throw std::runtime_error ("Failed to create internal structure");

    signal_set.async_wait
      (std::bind (&signal_handler, std::ref (context),
                  std::placeholders::_1, std::placeholders::_2));

    util::final_action_t cleanup
      ([&base, &signal_set] () {base.stop (); signal_set.cancel ();});

    context.run ();

    {
      log_t::buffer_t buf;
      buf << "Daemon is stopped, exiting";
      log_t::info (buf);
    }
  }
  catch (std::exception &e) {
    status = 128;
    log_t::buffer_t buf;
    buf << "Error during start-up: \"" << e.what () << "\"";
    log_t::error (buf);
  }

  return status;
}
