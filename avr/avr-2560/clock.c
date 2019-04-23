/*
 *
 *
 */

#include "clock.h"
#include "flush.h"
#include "render.h"
#include "timer.h"

volatile uint8_t v_hour;
volatile uint8_t v_min;
volatile uint8_t v_sec;

/*return 1 if we need to refresh display, 0 otherwise*/
static uint8_t time_advance ()
{
  if (v_sec == 59)
    v_sec = 0;
  else {
    ++v_sec;
    return 0;
  }

  if (v_min == 59)
    v_min = 0;
  else {
    ++v_min;
    return 1;
  }

  if (v_hour == 23)
    v_hour = 0;
  else
    ++v_hour;

  return 1;
}

static void update_screen ()
{
  /* 5 * 5 columns = 25, but we need 32 => add zeros, 4 here and 3 at the end*/
  render_direct (0, 4);
  uint8_t symbol = (v_hour / 10);
  symbol = (symbol) ? RENDER_SPACE : render_id (symbol);
  render (symbol);
  render (render_id (v_hour % 10));
  render (RENDER_COLON);
  render (render_id (v_min / 10));
  render (render_id (v_min % 10));
  render_direct (0, 3);

  /*fixme: Should we place it outside of this function ?*/
  flush_enable_clear ();
}

static void advance ()
{
  if (time_advance () != 0) {
    /*fixme: convert time to pixels & put them into flush*/;
    update_screen ();
  }
}

void clock_sync (uint8_t hour, uint8_t min, uint8_t sec)
{
  v_hour = hour;
  v_min = min;
  v_sec = sec;

  render_clear ();

  update_screen ();

  timer_enable (TIMER_ONE_PER_SECOND, &advance);
}
