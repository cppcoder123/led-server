/*
 *
 */

#include <stdint.h>
#include <util/atomic.h>

#include "unix/constant.h"

#include "at.h"
#include "clock.h"
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

#define PARAM_FLAG_ALARM (1 << 0)
#define PARAM_FLAG_BRIGNHTNESS (1 << 1)
#define PARAM_FLAG_CLOCK (1 << 2)
#define PARAM_FLAG_VOLUME (1 << 3)
#define PARAM_FLAG_TRACK (1 << 4)
#define PARAM_FLAG_VOLUME_SENT (1 << 5)
#define PARAM_FLAG_TRACK_SENT (1 << 6)

#define VALUE_SPACE 3

enum {
  PARAM_ALARM_H,
  PARAM_ALARM_M,
  PARAM_BRIGHTNESS,
  PARAM_CLOCK_H,
  PARAM_CLOCK_M,
  PARAM_TRACK,                  /* select radio station (or mp3) */
  PARAM_VOLUME,                 /* tune volume */
  PARAM_ALARM_ENABLE,
  PARAM_VALUE_MAX = PARAM_ALARM_ENABLE,
  PARAM_ALARM_DISABLE,
  PARAM_CANCEL,                 /* cancel param change */
  PARAM_POWER,                  /* 'On' or 'Off' */
};

static const uint8_t param_change_array[] =
  {PARAM_CANCEL, PARAM_VOLUME, PARAM_TRACK,
   PARAM_ALARM_ENABLE, PARAM_ALARM_DISABLE,
   PARAM_ALARM_H, PARAM_ALARM_M,
   PARAM_CLOCK_H, PARAM_CLOCK_M,
   PARAM_BRIGHTNESS, PARAM_POWER};

static uint8_t restore_mode = MODE_MENU;

static uint8_t delta = MIDDLE;
static uint8_t param = PARAM_POWER;
static uint8_t param_flag = 0;
static uint8_t param_value[PARAM_VALUE_MAX];

static uint8_t param_value_valid (uint8_t param)
{
  if (param >= PARAM_VALUE_MAX)
    return 1;

  uint8_t mask = 0;
  switch (param) {
  case PARAM_VOLUME:
    mask = PARAM_FLAG_VOLUME;
    break;
  case PARAM_TRACK:
    mask = PARAM_FLAG_TRACK;
    break;
  case PARAM_CLOCK_H:
  case PARAM_CLOCK_M:
    mask = PARAM_FLAG_CLOCK;
    break;
  case PARAM_ALARM_H:
  case PARAM_ALARM_M:
    mask = PARAM_FLAG_ALARM;
    break;
  case PARAM_BRIGHTNESS:
    mask = PARAM_FLAG_BRIGNHTNESS;
    break;
  default:
    break;
  }

  return (param_flag & mask) ? 1 : 0;
}

static uint8_t is_delta_needed ()
{
  return (param == PARAM_VOLUME) ? 1 : 0;
}

static uint8_t is_source_needed ()
{
  return (param == PARAM_VOLUME) ? 1 : 0;
}

static uint8_t is_destination_needed ()
{
  return ((param == PARAM_TRACK)
          || (param == PARAM_CLOCK_H)
          || (param == PARAM_CLOCK_M)
          || (param == PARAM_ALARM_H)
          || (param == PARAM_ALARM_M)
          || (param == PARAM_BRIGHTNESS)) ? 1 : 0;
}

static void render_delta (uint8_t negative, uint8_t abs,
                          uint8_t *data, uint8_t *position)
{
  if (negative != 0)
    render_symbol (FONT_MINUS, data, position);
  else
    render_symbol (FONT_PLUS, data, position);

  render_number (abs, RENDER_LEADING_DISABLE, data, position);
}

static uint8_t is_sum_fits (uint8_t a, uint8_t b)
{
  uint8_t a_ = MAX - a;
  return (a_ >= b) ? 1 : 0;
}

static uint8_t get_destination (uint8_t negate, uint8_t abs, uint8_t old)
{
  return (negate != 0) ? ((old > abs) ? (old - abs) : 0)
    : ((is_sum_fits (old, abs) != 0) ? (old + abs) : MAX);
}

static void render_destination (uint8_t negative, uint8_t abs,
                                uint8_t *data, uint8_t *position)
{
  uint8_t dst = get_destination (negative, abs, param_value[param]);

  render_number (dst, RENDER_LEADING_DISABLE, data, position);
}

static void split_delta (uint8_t *negate, uint8_t *abs)
{
  *negate = (delta < MIDDLE) ? 1 : 0;
  *abs = (*negate != 0) ? (MIDDLE - delta) : (delta - MIDDLE);
}

static void render_label (uint8_t *data, uint8_t *position)
{
  switch (param) {
  case PARAM_ALARM_DISABLE:
    {
      uint8_t tag[] =
        {FONT_A, FONT_l, FONT_MINUS, FONT_D, FONT_i,
         FONT_s, FONT_a, FONT_b, FONT_l, FONT_e};
      render_word (tag, sizeof (tag) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_ALARM_ENABLE:
    {
      uint8_t tag[] =
        {FONT_A, FONT_l, FONT_MINUS, FONT_E, FONT_n, FONT_a, FONT_b, FONT_l, FONT_e};
      render_word (tag, sizeof (tag) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_ALARM_H:
    {
      uint8_t tag[] =
        {FONT_A, FONT_l, FONT_MINUS, FONT_H};
      render_word (tag, sizeof (tag) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_ALARM_M:
    {
      uint8_t tag[] =
        {FONT_A, FONT_l, FONT_MINUS, FONT_M};
      render_word (tag, sizeof (tag) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_BRIGHTNESS:
    {
      uint8_t tag[] =
        {FONT_B, FONT_r, FONT_i, FONT_g, FONT_h, FONT_MINUS, FONT_s};
      render_word (tag, sizeof (tag) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_CLOCK_H:
    {
      uint8_t tag[]
        = {FONT_C, FONT_l, FONT_MINUS, FONT_H};
      render_word (tag, sizeof (tag) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_CLOCK_M:
    {
      uint8_t tag[] =
        {FONT_C, FONT_l, FONT_MINUS, FONT_M};
      render_word (tag, sizeof (tag) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_CANCEL:
    {
      uint8_t cancel[] = {FONT_C, FONT_a, FONT_n, FONT_c, FONT_e, FONT_l};
      render_word (cancel, sizeof (cancel) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_POWER:
    {
      uint8_t on[] = {FONT_O, FONT_n};
      uint8_t off[]  = {FONT_O, FONT_f, FONT_f};
      if (mode_is_connnected () != 0)
        render_word (off, sizeof (off) / sizeof (uint8_t), data, position);
      else
        render_word (on, sizeof (on) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_TRACK:
    {
      uint8_t track[]
        = {FONT_C, FONT_h, FONT_a, FONT_n, FONT_n};
      render_word (track, sizeof (track) / sizeof (uint8_t), data, position);
    }
    break;
  case PARAM_VOLUME:
    {
      uint8_t vol[] = {FONT_V, FONT_o};
      render_word (vol, sizeof (vol) / sizeof (uint8_t), data, position);
    }
    break;
  default:
    break;
  }
}

static void render ()
{
  uint8_t data[DATA_SIZE];
  uint8_t position = 0;
  data[position++] = 0;
  data[position++] = 0;

  render_label (data, &position);

  uint8_t negate, abs;
  split_delta (&negate, &abs);

  if (is_delta_needed () != 0) {
    render_symbol (FONT_COLON, data, &position);
    render_delta (negate, abs, data, &position);
  }
  if (param_value_valid (param) != 0) {
    if (is_source_needed () != 0) {
      render_symbol (FONT_COLON, data, &position);
      render_number (param_value[param],
                     RENDER_LEADING_DISABLE, data, &position);
    } else if (is_destination_needed () != 0) {
      render_symbol (FONT_COLON, data, &position);
      render_destination (negate, abs, data, &position);
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

static void query_param ()
{
  switch (param) {
  case PARAM_ALARM_H:
  case PARAM_ALARM_M:
    {
      uint8_t hour, min;
      clock_alarm_get (&hour, &min);
      /* debug_2 (DEBUG_MENU, 99, hour, min); */
      param_value[PARAM_ALARM_H] = hour;
      param_value[PARAM_ALARM_M] = min;
      param_flag |= PARAM_FLAG_ALARM;
    }
    break;
  case PARAM_BRIGHTNESS:
    {
      uint8_t brightness = 0;
      flush_brightness_get (&brightness);
      /* debug_1 (DEBUG_MENU, 22, brightness); */
      param_value[PARAM_BRIGHTNESS] = brightness;
      param_flag |= PARAM_FLAG_BRIGNHTNESS;
    }
    break;
  case PARAM_CLOCK_H:
  case PARAM_CLOCK_M:
    {
      uint8_t hour, min;
      clock_get (&hour, &min);
      param_value[PARAM_CLOCK_H] = hour;
      param_value[PARAM_CLOCK_M] = min;
      param_flag |= PARAM_FLAG_CLOCK;
    }
    break;
  case PARAM_TRACK:
    if ((param_flag & PARAM_FLAG_TRACK_SENT) == 0) {
      send_message_1 (MSG_ID_PARAM_QUERY, PARAMETER_TRACK);
      param_flag |= PARAM_FLAG_TRACK_SENT;
    }
    break;
  case PARAM_VOLUME:
    if ((param_flag & PARAM_FLAG_VOLUME_SENT) == 0) {
      send_message_1 (MSG_ID_PARAM_QUERY, PARAMETER_VOLUME);
      param_flag |= PARAM_FLAG_VOLUME_SENT;
    }
    break;
  default:
    break;
  }
}

static void change_param (uint8_t action)
{
  uint8_t old_param = param;
  const uint8_t length = sizeof (param_change_array) / sizeof (uint8_t);
  const uint8_t max_id = length - 1;

  uint8_t id = 0;
  for (id = 0; id < length; ++id)
    if (param == param_change_array[id])
      break;

  if (action == ROTOR_CLOCKWISE) {
    if (id < max_id)
      ++id;
    else
      id = 0;
  } else if (action == ROTOR_COUNTER_CLOCKWISE) {
    if (id > 0)
      --id;
    else
      id = max_id;
  }

  param = param_change_array[id];

  if (old_param != param)
    query_param ();

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
  if (mode_is_connnected () == 0)
    return;

  uint8_t positive = 0, out_delta = 0;
  ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
    positive = (delta >= MIDDLE) ? PARAMETER_POSITIVE : PARAMETER_NEGATIVE;
    out_delta = (positive == PARAMETER_POSITIVE)
      ? (delta - MIDDLE) : (MIDDLE - delta);
  }
  encode_msg_3
    (MSG_ID_PARAM_SET, SERIAL_ID_TO_IGNORE, parameter, positive, out_delta);
}

static uint8_t apply_delta_max (uint8_t old, uint8_t max_value)
{
  uint8_t negate, abs;
  split_delta (&negate, &abs);

  uint8_t dst = get_destination (negate, abs, old);

  return (dst > max_value) ? max_value : dst;
}

static void stop ()
{
  /* fixme send new value*/
  if (param_value_valid (param) == 0)
    return;

  switch (param) {
  case PARAM_ALARM_DISABLE:
    clock_alarm_engage_set (0);
    break;
  case PARAM_ALARM_ENABLE:
    clock_alarm_engage_set (1);
    break;
  case PARAM_ALARM_H:
    clock_alarm_set
      (apply_delta_max (param_value[PARAM_ALARM_H], CLOCK_HOUR_MAX),
       param_value[PARAM_ALARM_M]);
    break;
  case PARAM_ALARM_M:
    clock_alarm_set
      (param_value[PARAM_ALARM_H],
       apply_delta_max (param_value[PARAM_ALARM_M], CLOCK_MINUTE_MAX));
    break;
  case PARAM_BRIGHTNESS:
      flush_brightness_set
        (apply_delta_max (param_value[PARAM_BRIGHTNESS], FLUSH_BRIGHTNESS_MAX));
    break;
  case PARAM_CLOCK_H:
    clock_set
      (apply_delta_max (param_value[PARAM_CLOCK_H], CLOCK_HOUR_MAX),
       param_value[PARAM_CLOCK_M]);
    break;
  case PARAM_CLOCK_M:
    clock_set
      (param_value[PARAM_CLOCK_H],
       apply_delta_max (param_value[PARAM_CLOCK_M], CLOCK_MINUTE_MAX));
    break;
  case PARAM_POWER:
    if (mode_is_connnected () == 0)
      power_on ();
    else
      encode_msg_0 (MSG_ID_POWEROFF, SERIAL_ID_TO_IGNORE);
    break;
  case PARAM_TRACK:
    send_param_change (PARAMETER_TRACK);
    break;
  case PARAM_VOLUME:
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
  param = param_change_array[0];
  param_flag = 0;
  for (uint8_t i = 0; i < PARAM_VALUE_MAX; ++i)
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
