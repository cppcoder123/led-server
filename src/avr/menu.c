/*
 *
 */

#include <stdint.h>

#include "const/constant.h"

#include "at.h"
#include "boot.h"
#include "debug.h"
#include "encode.h"
#include "font.h"
#include "flush.h"
#include "menu.h"
#include "mode.h"
#include "render.h"
#include "rotor.h"
#include "watch.h"

/* 5 seconds */
#define MENU_DELAY 5

/* left knob */
#define PARAM_ROTOR ROTOR_1
#define VOLUME_ROTOR PARAM_ROTOR

#define PARAM_FLAG_ALARM (1 << 0)
#define PARAM_FLAG_BRIGNHTNESS (1 << 1)
#define PARAM_FLAG_VOLUME (1 << 2)
#define PARAM_FLAG_TRACK (1 << 3)
#define PARAM_FLAG_VOLUME_SENT (1 << 4)
#define PARAM_FLAG_TRACK_SENT (1 << 5)

#define DELTA_MIN 0
#define DELTA_MIDDLE 0x7F
#define DELTA_MAX 0xFF

/* chunk min should be more than zero */
#define CHUNK_MIN 11
#define CHUNK_MAX 14
#define CHUNK_MIDDLE ((CHUNK_MIN + CHUNK_MAX) / 2) 

enum {
  PARAM_ALARM_HOUR,
  PARAM_ALARM_MINUTE,
  PARAM_BRIGHTNESS,
  PARAM_TRACK,                  /* select radio station (or mp3) */
  PARAM_VOLUME,                 /* tune volume */
  PARAM_CANCEL,                 /* cancel param change */
  PARAM_VALUE_MAX = PARAM_CANCEL,
  PARAM_ALARM,                  /* enable or disable */
  PARAM_POWER_OFF,              /* 'Off' */
  PARAM_POWER_ON,               /* 'On' */
  PARAM_REBOOT,                 /* reboot pi */
};

enum {
  WAY_UNKNOWN,                  /* initial value */
  WAY_SIMPLE,                   /* channel & volume */
  WAY_COMPLEX,                  /* the rest of params */
};

static const uint8_t change_param_array_radio[] =
  {PARAM_CANCEL, PARAM_ALARM, PARAM_ALARM_HOUR,
   PARAM_ALARM_MINUTE, PARAM_BRIGHTNESS, PARAM_REBOOT, PARAM_POWER_OFF};
static const uint8_t change_param_array_watch[] =
  {PARAM_CANCEL, PARAM_ALARM, PARAM_ALARM_HOUR,
   PARAM_ALARM_MINUTE, PARAM_BRIGHTNESS, PARAM_POWER_ON};

static uint8_t backup_mode = MODE_MENU;
static uint8_t chunk = CHUNK_MIDDLE;
static uint8_t delta = DELTA_MIDDLE;
static uint8_t param = PARAM_POWER_ON;
static uint8_t param_flag = 0;
static uint8_t param_max[PARAM_VALUE_MAX];
static uint8_t param_min[PARAM_VALUE_MAX];
static uint8_t param_value[PARAM_VALUE_MAX];
static uint8_t way = WAY_UNKNOWN;

/*
 * General purpose functions
 */
static uint8_t is_sum_fits (uint8_t a, uint8_t b)
{
  uint8_t a_ = 0xFF - a;
  return (a_ >= b) ? 1 : 0;
}

/*
 * Delta related
 */
static uint8_t delta_abs (uint8_t *positive)
{
  *positive = (delta >= DELTA_MIDDLE) ? 1 : 0;
  return (*positive != 0) ? (delta - DELTA_MIDDLE) : (DELTA_MIDDLE - delta);
}

static void delta_reset ()
{
  delta = DELTA_MIDDLE;
  /* chunk = CHUNK_MIDDLE; */
}

/*
 * Send
 */
static void send_message_0 (uint8_t msg_id)
{
  if (mode_is_connnected () != 0)
    encode_msg_0 (msg_id, SERIAL_ID_TO_IGNORE);
}

static void send_message_1 (uint8_t msg_id, uint8_t payload_1)
{
  if (mode_is_connnected () != 0)
    encode_msg_1 (msg_id, SERIAL_ID_TO_IGNORE, payload_1);
}

static void send_param_change (uint8_t parameter)
{
  if (mode_is_connnected () == 0)
    return;

  uint8_t positive, abs = delta_abs (&positive);

  encode_msg_3
    (MSG_ID_PARAM_SET, SERIAL_ID_TO_IGNORE, parameter, positive, abs);
}

/*
 * Value
 */
static uint8_t value_is_valid ()
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
  case PARAM_ALARM_HOUR:
  case PARAM_ALARM_MINUTE:
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

static uint8_t value_derive ()
{
  if (value_is_valid () == 0)
    return 0;

  uint8_t positive, abs = delta_abs (&positive);

  uint8_t old = param_value[param];
  uint8_t raw =  (positive != 0)
    ? ((is_sum_fits (old, abs) != 0) ? (old + abs) : 0xFF)
    : ((old > abs) ? (old - abs) : 0);

  if (raw < param_min[param])
    return param_min[param];
  if (raw > param_max[param])
    return param_max[param];

  return raw;
}

static void value_query ()
{
  switch (param) {
  case PARAM_ALARM_HOUR:
  case PARAM_ALARM_MINUTE:
    {
      uint8_t hour, min;
      watch_alarm_get (&hour, &min);
      /* debug_2 (DEBUG_MENU, 99, hour, min); */
      param_value[PARAM_ALARM_HOUR] = hour;
      param_value[PARAM_ALARM_MINUTE] = min;
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

static void value_update ()
{
  switch (param) {
    case PARAM_ALARM:
      if (watch_alarm_state () == 0)
        watch_alarm_control (1);
      else
        watch_alarm_control (0);
      break;
    case PARAM_ALARM_HOUR:
      watch_alarm_set (value_derive (), param_value[PARAM_ALARM_MINUTE]);
      break;
    case PARAM_ALARM_MINUTE:
      watch_alarm_set
        (param_value[PARAM_ALARM_HOUR], value_derive ());
      break;
    case PARAM_BRIGHTNESS:
      flush_brightness_set (value_derive ());
      break;
    case PARAM_POWER_OFF:
      send_message_0 (MSG_ID_POWEROFF);
      break;
    case PARAM_POWER_ON:
      boot_pi ();
      break;
    case PARAM_REBOOT:
      send_message_0 (MSG_ID_REBOOT);
      /* debug_0 (DEBUG_MENU, 123); */
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
}

/*
 * Render
 */
static uint8_t render_delta_needed ()
{
  return (param == PARAM_VOLUME) ? 1 : 0;
}

static uint8_t render_source_needed ()
{
  return (param == PARAM_VOLUME) ? 1 : 0;
}

static uint8_t render_destination_needed ()
{
  return ((param == PARAM_TRACK)
          || (param == PARAM_ALARM_HOUR)
          || (param == PARAM_ALARM_MINUTE)
          || (param == PARAM_BRIGHTNESS)) ? 1 : 0;
}

static void render_delta (uint8_t positive, uint8_t abs, struct buf_t *buf)
{
  if (positive != 0)
    render_symbol (buf, FONT_PLUS);
  else
    render_symbol (buf, FONT_MINUS);

  render_number (buf, abs, RENDER_LEADING_DISABLE);
}

static void render_label (struct buf_t *buf)
{
  switch (param) {
  case PARAM_ALARM:
    if (watch_alarm_state () != 0) {
      uint8_t tag[] =
        {FONT_A, FONT_l, FONT_MINUS, FONT_D, FONT_i,
         FONT_s, FONT_a, FONT_b, FONT_l, FONT_e};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    } else {
      uint8_t tag[] =
        {FONT_A, FONT_l, FONT_MINUS, FONT_E, FONT_n, FONT_a, FONT_b, FONT_l, FONT_e};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_ALARM_HOUR:
    {
      uint8_t tag[] =
        {FONT_A, FONT_l, FONT_MINUS, FONT_H};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_ALARM_MINUTE:
    {
      uint8_t tag[] =
        {FONT_A, FONT_l, FONT_MINUS, FONT_M};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_BRIGHTNESS:
    {
      uint8_t tag[] =
        {FONT_B, FONT_r, FONT_i, FONT_g, FONT_h, FONT_MINUS, FONT_s};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_CANCEL:
    {
      uint8_t cancel[] = {FONT_C, FONT_a, FONT_n, FONT_c, FONT_e, FONT_l};
      render_word (buf, cancel, sizeof (cancel) / sizeof (uint8_t));
    }
    break;
  case PARAM_POWER_OFF:
    {
      uint8_t off[]  = {FONT_O, FONT_f, FONT_f};
      render_word (buf, off, sizeof (off) / sizeof (uint8_t));
    }
    break;
  case PARAM_POWER_ON:
    {
      uint8_t on[] = {FONT_O, FONT_n};
      render_word (buf, on, sizeof (on) / sizeof (uint8_t));
    }
    break;
  case PARAM_REBOOT:
    {
      uint8_t tag[] = {FONT_R, FONT_e, FONT_b, FONT_o, FONT_o, FONT_t};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_TRACK:
    {
      uint8_t track[]
        = {FONT_C, FONT_h, FONT_a, FONT_n, FONT_n};
      render_word (buf, track, sizeof (track) / sizeof (uint8_t));
    }
    break;
  case PARAM_VOLUME:
    {
      uint8_t vol[] = {FONT_V, FONT_o};
      render_word (buf, vol, sizeof (vol) / sizeof (uint8_t));
    }
    break;
  default:
    break;
  }
}

/* static void debug_position (struct render_t *buf, uint8_t mark) */
/* { */
/*   debug_2 (DEBUG_MENU, 111, *(buf->position), mark); */
/* } */

static void render ()
{
  struct buf_t buf;
  buf_init (&buf);

  /* debug_position (&buf, 1); */

  buf_byte_fill (&buf, 0);
  /* debug_position (&buf, 2); */
  
  render_label (&buf);
  /* debug_position (&buf, 3); */

  uint8_t positive, abs = delta_abs (&positive);

  if (render_delta_needed () != 0) {
    render_symbol (&buf, FONT_COLON);
    /* debug_position (&buf, 4); */
    render_delta (positive, abs, &buf);
    /* debug_position (&buf, 5); */
  }
  if (value_is_valid () != 0) {
    if (render_source_needed () != 0) {
      render_symbol (&buf, FONT_COLON);
      /* debug_position (&buf, 6); */
      render_number (&buf, param_value[param], RENDER_LEADING_DISABLE);
      /* debug_position (&buf, 7); */
    } else if (render_destination_needed () != 0) {
      render_symbol (&buf, FONT_COLON);
      /* debug_position (&buf, 8); */
      uint8_t dst = value_derive ();
      render_number (&buf, dst, RENDER_LEADING_DISABLE);
      /* debug_position (&buf, 9); */
    }
  }

  render_tail (&buf);
  /* debug_position (&buf, 10); */

  flush_stable_display (&buf);
}

/*
 * Change (param or its value)
 */

static uint8_t change (uint8_t old, uint8_t action, uint8_t min, uint8_t max)
{
  /* debug_2 (DEBUG_MENU, 88, new, chunk); */

  if (++chunk < CHUNK_MAX)
    return old;

  uint8_t new = old;

  if ((action == ROTOR_CLOCKWISE)
      && (new < max))
    ++new;
  else if ((action == ROTOR_COUNTER_CLOCKWISE)
           && (new > min))
    --new;

  chunk = CHUNK_MIN;
  /* debug_2 (DEBUG_MENU, 99, new, chunk); */

  return new;
}

static void change_param_set (uint8_t new_param)
{
  param = new_param;
  delta_reset ();

  if (value_is_valid () == 0)
    value_query ();

  render ();
}

static void change_param (uint8_t action)
{
  const uint8_t tmp = (mode_is_connnected () != 0)
    ? sizeof (change_param_array_radio) : sizeof (change_param_array_watch);
  const uint8_t max_id = (tmp / sizeof (uint8_t)) - 1;
  const uint8_t *param_array = (mode_is_connnected () != 0)
    ? change_param_array_radio : change_param_array_watch;

  uint8_t id = 0;
  for (id = 0; id <= max_id; ++id)
    if (param == param_array[id])
      break;

  uint8_t new_id = change (id, action, 0, max_id);

  if (new_id != id)
    change_param_set (param_array[new_id]);
}

static void change_delta (uint8_t action)
{
  uint8_t backup_delta = delta;

  delta = change (delta, action, DELTA_MIN, DELTA_MAX);

  if (delta == backup_delta)
    return;

  if (value_is_valid () == 0)
    return;

  uint8_t positive, abs = delta_abs (&positive);

  uint8_t delta_valid = 1;
  if (positive != 0) {
    /* check high limit */
    if ((is_sum_fits (abs, param_value[param]) == 0)
        || ((param_value[param] + abs) > param_max[param]))
      delta_valid = 0;
  } else {
    /* check low limit */
    if ((abs > param_value[param])
        || ((param_value[param] - abs) < param_min[param]))
      delta_valid = 0;
  }

  if (delta_valid == 0)
    delta = backup_delta;

  render ();
}

/* exit from menu handling */
static void stop ()
{
  if (value_is_valid () != 0)
    value_update ();

  flush_shift_drain_stop ();
  mode_set (backup_mode);
  send_message_0 (MSG_ID_RESUME);
}

/* reset state before starting menu handling */
static void reset ()
{
  delta_reset ();
  param = change_param_array_radio[0];
  param_flag = 0;
  for (uint8_t i = 0; i < PARAM_VALUE_MAX; ++i)
    param_value[i] = 0;
  way = WAY_UNKNOWN;
}

/* start menu handling */
void menu_handle_rotor (uint8_t id, uint8_t action)
{
  /* debug_2 (DEBUG_MENU, 111, id, action); */

  if (at_empty (AT_MENU) != 0) {
    /* debug_0 (DEBUG_MENU, 11); */
    reset ();
    way = ((mode_is_connnected () != 0)
           && (action != ROTOR_PUSH)) ? WAY_SIMPLE : WAY_COMPLEX;
    backup_mode = mode_get ();
    mode_set (MODE_MENU);
    send_message_0 (MSG_ID_SUSPEND);
    flush_shift_drain_start ();
    at_schedule (AT_MENU, MENU_DELAY, &stop);
    return;
  }

  /* debug_0 (DEBUG_MENU, 22); */

  if (action == ROTOR_PUSH) {
    /* debug_0 (DEBUG_MENU, 33); */
    at_cancel (AT_MENU);
    stop ();
    return;
  }

  at_postpone (AT_MENU);

  if (way == WAY_SIMPLE) {
    /* debug_0 (DEBUG_MENU, 44); */
    uint8_t new_param = (id == VOLUME_ROTOR) ? PARAM_VOLUME : PARAM_TRACK;
    if (new_param != param)
      change_param_set (new_param);
    change_delta (action);
    return;
  }

  /* here we should have complex way */
  if (id == PARAM_ROTOR)
    change_param (action);
  else
    change_delta (action);
}

void menu_init ()
{
  /* rotor_register (&start); */
  backup_mode = MODE_MENU;

  param_min[PARAM_ALARM_HOUR] = 0;
  param_min[PARAM_BRIGHTNESS] = 0;
  param_min[PARAM_ALARM_MINUTE] = 0;

  param_max[PARAM_ALARM_HOUR] = WATCH_HOUR_MAX;
  param_max[PARAM_BRIGHTNESS] = 0xF;
  param_max[PARAM_ALARM_MINUTE] = WATCH_MINUTE_MAX;

  reset ();
}

uint8_t menu_parameter_value (uint8_t parameter, uint8_t value,
                              uint8_t min, uint8_t max)
{
  if ((parameter != PARAMETER_VOLUME)
      && (parameter != PARAMETER_TRACK))
    return 0;

  uint8_t id = (parameter == PARAMETER_VOLUME) ? PARAM_VOLUME : PARAM_TRACK;
  param_value[id] = value;
  param_min[id] = min;
  param_max[id] = max;

  param_flag |= (parameter == PARAMETER_VOLUME)
    ? PARAM_FLAG_VOLUME : PARAM_FLAG_TRACK;

  at_postpone (AT_MENU);

  if (id == param) {
    uint8_t new_value = value_derive ();
    if ((new_value < param_min[id])
        || (new_value > param_max[id]))
      delta_reset ();

    render ();
  }

  return 1;
}
