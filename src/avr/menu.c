/*
 *
 */

#include <stdint.h>
#include <util/atomic.h>

#include "unix/constant.h"

#include "at.h"
#include "debug.h"
#include "encode.h"
#include "font.h"
#include "flush.h"
#include "menu.h"
#include "mode.h"
#include "power.h"
#include "render.h"
#include "rotor.h"

/* 5 seconds */
#define MENU_DELAY 5

#define PARAM_ID ROTOR_2

#define MIDDLE 0x7F
#define MAX 0xFF

#define DATA_SIZE FLUSH_STABLE_SIZE

#define PARAM_FLAG_VOLUME (1 << 0)
#define PARAM_FLAG_TRACK (1 << 1)

#define VALUE_SPACE 3

enum {
  PARAM_POWER,                  /* 'On' or 'Off' */
  PARAM_TRACK,                  /* select radio station (or mp3) */
  PARAM_VOLUME,                 /* tune volume */
  PARAM_CANCEL,                 /* cancel param change */
  PARAM_LAST = PARAM_CANCEL,    /* keep last */
};

static uint8_t restore_mode = MODE_MENU;

static uint8_t delta = MIDDLE;
static uint8_t param = PARAM_POWER;
static uint8_t param_flag = 0;
static uint8_t param_value[PARAM_LAST];

static uint8_t param_value_valid (uint8_t param)
{
  if ((param != PARAM_TRACK)
      && (param != PARAM_VOLUME))
    return 0;

  uint8_t mask = (param == PARAM_VOLUME) ? PARAM_FLAG_VOLUME : PARAM_FLAG_TRACK;

  return (param_flag & mask) ? 1 : 0;
}

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
      uint8_t track[] = {FONT_C, FONT_h};
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

    if (param_value_valid (param) != 0) {
      for (uint8_t i = 0; i < VALUE_SPACE; ++i)
        render_empty_column (data, &position);
      render_number (param_value[param],
                     RENDER_LEADING_DISABLE, data, &position);
    }
  }

  for (uint8_t i = position; i < DATA_SIZE; ++i)
    render_empty_column (data, &position);

  flush_stable_display (data);
}

static void send_message_1 (uint8_t msg_id, uint8_t payload_1)
{
  if (mode_is_connnected () != 0)
    encode_msg_1 (msg_id, SERIAL_ID_TO_IGNORE, payload_1);
}

static void change_param (uint8_t action)
{
  uint8_t old_param = param;

  if ((action == ROTOR_CLOCKWISE)
      && (param < PARAM_LAST))
    ++param;
  else if ((action == ROTOR_COUNTER_CLOCKWISE)
           && (param > 0))
    --param;

  if ((old_param != param)
      && ((param == PARAM_VOLUME)
          || (param == PARAM_TRACK))
      && (param_value_valid (param) == 0))
    send_message_1
      (MSG_ID_PARAM_QUERY,
       (param == PARAM_VOLUME) ? PARAMETER_VOLUME : PARAMETER_TRACK);

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

static void send_message_0 (uint8_t msg_id)
{
  if (mode_is_connnected () != 0)
    encode_msg_0 (msg_id, SERIAL_ID_TO_IGNORE);
}

static void send_param_change (uint8_t parameter)
{
  uint8_t positive = 0, out_delta = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    positive = (delta >= MIDDLE) ? PARAMETER_POSITIVE : PARAMETER_NEGATIVE;
    out_delta = (positive == PARAMETER_POSITIVE)
      ? (delta - MIDDLE) : (MIDDLE - delta);
  }
  debug_3 (DEBUG_MENU, 55, parameter, positive, out_delta);
  if (mode_is_connnected () != 0)
    encode_msg_3
      (MSG_ID_PARAM_SET, SERIAL_ID_TO_IGNORE, parameter, positive, out_delta);
}

static void stop ()
{
  /* fixme send new value*/
  switch (param) {
  case PARAM_POWER:
    if (mode_is_connnected () == 0)
      power_on ();
    else
      encode_msg_0 (MSG_ID_POWEROFF, SERIAL_ID_TO_IGNORE);
    break;
  case PARAM_TRACK:
    if (param_value_valid (PARAM_TRACK) != 0)
      send_param_change (PARAMETER_TRACK);
    break;
  case PARAM_VOLUME:
    if (param_value_valid (PARAM_VOLUME) != 0)
      send_param_change (PARAMETER_VOLUME);
    break;
  default:
    break;
  }

  flush_shift_drain_stop ();
  mode_set (restore_mode);
  send_message_0 (MSG_ID_RESUME);
}

static void reset ()
{
  delta = MIDDLE;
  param = PARAM_CANCEL;
  param_flag = 0;
  for (uint8_t i = 0; i < PARAM_LAST; ++i)
    param_value[i] = 0;
}

static void start (uint8_t id, uint8_t action)
{
  if (action == ROTOR_PUSH)
    return;

  if (at_empty (AT_MENU) != 0) {
    reset ();
    at_schedule (AT_MENU, MENU_DELAY, &stop);
    restore_mode = mode_get ();
    mode_set (MODE_MENU);
    send_message_0 (MSG_ID_SUSPEND);
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
  restore_mode = MODE_MENU;

  reset ();
}

uint8_t menu_parameter_value (uint8_t parameter, uint8_t value)
{
  if ((parameter != PARAMETER_VOLUME)
      && (parameter != PARAMETER_TRACK))
    return 0;

  uint8_t id = (parameter == PARAMETER_VOLUME) ? PARAM_VOLUME : PARAM_TRACK;
  param_value[id] = value;

  param_flag |= (parameter == PARAMETER_VOLUME)
    ? PARAM_FLAG_VOLUME : PARAM_FLAG_TRACK;

  at_postpone (AT_MENU);

  if (id == param)
    render ();

  return 1;
}
