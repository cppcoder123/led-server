/*
 *
 */

#include <stdint.h>

#include "unix/constant.h"

#include "at.h"
#include "encode.h"
#include "font.h"
#include "flush.h"
#include "menu.h"
#include "mode.h"
#include "render.h"
#include "rotor.h"

/* 5 seconds */
#define MENU_DELAY 5

#define PARAM_ID ROTOR_2

#define MIDDLE 0x7F
#define MAX 0xFF

#define DATA_SIZE FLUSH_STABLE_SIZE

enum {
  PARAM_POWER,                  /* 'On' or 'Off' */
  PARAM_TRACK,                  /* select radio station (or mp3) */
  PARAM_VOLUME,                 /* tune volume */
  PARAM_CANCEL,                 /* cancel param change */
  PARAM_LAST = PARAM_CANCEL,    /* keep last */
};

static uint8_t restore_mode = MODE_IDLE;

static uint8_t param = PARAM_POWER;
static uint8_t delta = MIDDLE;

static void render ()
{
  uint8_t data[DATA_SIZE];
  uint8_t position = 0;
  data[position++] = 0;
  data[position++] = 0;

  switch (param) {
  case PARAM_POWER:
    {
      uint8_t on[] = {FONT_O, FONT_n};
      uint8_t off[]  = {FONT_O, FONT_f, FONT_f};
      if (mode_is_connnected () != 0)
        render_word (off, sizeof (off) / sizeof (uint8_t), data, &position);
      else
        render_word (on, sizeof (on) / sizeof (uint8_t), data, &position);
    }
    break;
  case PARAM_CANCEL:
    {
      uint8_t cancel[] = {FONT_C, FONT_a, FONT_n, FONT_c, FONT_e, FONT_l};
      render_word (cancel, sizeof (cancel) / sizeof (uint8_t), data, &position);
    }
    break;
  case PARAM_TRACK:
    {
      uint8_t track[] = {FONT_T, FONT_r};
      render_word (track, sizeof (track) / sizeof (uint8_t), data, &position);
    }
    break;
  case PARAM_VOLUME:
    {
      uint8_t vol[] = {FONT_V, FONT_o};
      render_word (vol, sizeof (vol) / sizeof (uint8_t), data, &position);
    }
    break;
  default:
    break;
  }

  if ((param == PARAM_TRACK)
      || (param == PARAM_VOLUME)) {

    uint8_t info = 0;
    if (delta < MIDDLE) {
      render_symbol(FONT_MINUS, data, &position);
      info = MIDDLE - delta;
    } else {
      render_symbol (FONT_PLUS, data, &position);
      info = delta - MIDDLE;
    }

    render_number (info, RENDER_LEADING_DISABLE, data, &position);
  }

  for (uint8_t i = position; i < DATA_SIZE; ++i)
    data[i] = 0;

  flush_stable_display (data);
}

static void change_param (uint8_t action)
{
  if ((action == ROTOR_CLOCKWISE)
      && (param < PARAM_LAST))
    ++param;
  else if ((action == ROTOR_COUNTER_CLOCKWISE)
           && (param > 0))
    --param;

  render ();
}

static void change_delta (uint8_t action)
{
  if ((action == ROTOR_CLOCKWISE)
      && (delta < MAX))
    ++delta;
  else if ((action == ROTOR_COUNTER_CLOCKWISE)
           && (delta > 0))
    --delta;

  render ();
}

static void suspend ()
{
  if (mode_is_connnected () != 0)
    encode_msg_0 (MSG_ID_SUSPEND, SERIAL_ID_TO_IGNORE);
}

static void resume ()
{
  if (mode_is_connnected () != 0)
    encode_msg_0 (MSG_ID_RESUME, SERIAL_ID_TO_IGNORE);
}

static void stop ()
{
  /* fixme send new value*/

  flush_shift_drain_stop ();
  mode_set (restore_mode);
  resume ();
}

static void start (uint8_t id, uint8_t action)
{
  if (action == ROTOR_PUSH)
    return;

  if (at_empty (AT_MENU) != 0) {
    param = PARAM_POWER;
    delta = MIDDLE;
    at_schedule (AT_MENU, MENU_DELAY, &stop);
    restore_mode = mode_get ();
    mode_set (MODE_IDLE);
    suspend ();
    flush_shift_drain_start ();
  } else {
    at_postpone (AT_MENU);
  }

  if (id == PARAM_ID)
    change_param (action);
  else
    change_delta (action);
}

void menu_init ()
{
  rotor_register (&start);
  restore_mode = MODE_IDLE;

  param = PARAM_POWER;
  delta = MIDDLE;
}
