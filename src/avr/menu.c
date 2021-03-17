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

#define SELECT_DELAY 3
#define APPLY_DELAY 1

#define APPLY_INDENT 7

#define VALUE_ROTOR ROTOR_0
#define PARAM_ROTOR ROTOR_1
#define APPLY_ROTOR PARAM_ROTOR

#define FLAG_BRIGNHTNESS (1 << 0)
#define FLAG_BRIGNHTNESS_SENT (1 << 1)
#define FLAG_PLAYLIST (1 << 2)
#define FLAG_PLAYLIST_SENT (1 << 3)
#define FLAG_TRACK (1 << 4)
#define FLAG_TRACK_SENT (1 << 5)
#define FLAG_VOLUME (1 << 6)
#define FLAG_VOLUME_SENT (1 << 7)

#define FLAG_MASK_BRIGNHTNESS (FLAG_BRIGNHTNESS | FLAG_BRIGNHTNESS_SENT)
#define FLAG_MASK_PLAYLIST (FLAG_PLAYLIST | FLAG_PLAYLIST_SENT)
#define FLAG_MASK_TRACK (FLAG_TRACK | FLAG_TRACK_SENT)
#define FLAG_MASK_VOLUME (FLAG_VOLUME | FLAG_VOLUME_SENT)

enum {
  PARAM_APPLY,
  PARAM_BRIGHTNESS,             /* select brightness */
  PARAM_CANCEL,                 /* cancel param change */
  PARAM_NEXT,                   /* next track */
  PARAM_PLAYLIST,               /* select/load playlist */
  PARAM_POWER_OFF,              /* 'Off' */
  PARAM_POWER_ON,               /* 'On' */
  PARAM_PREVIOUS,               /* previous track */
  PARAM_REBOOT,                 /* reboot pi */
  PARAM_TRACK,                  /* select radio station (or mp3) */
  PARAM_VOLUME,                 /* tune volume */
};

enum {
  STATE_IDLE,
  STATE_PARAM,
  STATE_VALUE,
  STATE_APPLY,
};

static uint8_t backup_mode = MODE_MENU;
static uint8_t param_flag = 0;

enum {
  VALUE_BRIGHTNESS,
  VALUE_PLAYLIST,
  VALUE_TRACK,
  VALUE_VOLUME,
  VALUE_MAX,                    /* keep last! */
};
static uint8_t param_max[VALUE_MAX];
static uint8_t param_min[VALUE_MAX];
static uint8_t param_old[VALUE_MAX];
static uint8_t param_new[VALUE_MAX];

static const uint8_t param_array_radio[] =
  {PARAM_TRACK, PARAM_VOLUME, PARAM_PLAYLIST, PARAM_NEXT,
   PARAM_PREVIOUS, PARAM_BRIGHTNESS, PARAM_REBOOT, PARAM_POWER_OFF};
static const uint8_t param_array_watch[] = {PARAM_BRIGHTNESS, PARAM_POWER_ON};
static const uint8_t param_array_apply[] = {PARAM_CANCEL, PARAM_APPLY};
/* ! see param_array_apply */
#define PARAM_ID_CANCEL 0
#define PARAM_ID_APPLY 1

static const uint8_t *param_array = param_array_watch;
static uint8_t param_id = 0;
static uint8_t param_id_max = sizeof (param_array_watch) / sizeof (uint8_t);
/* primary param is param_array[param_id] */
static uint8_t secondary_param = PARAM_CANCEL;

static uint8_t state = STATE_IDLE;

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

static void send_message_3 (uint8_t parameter, uint8_t sign, uint8_t abs)
{
  if (mode_is_connnected () == 0)
    return;

  encode_msg_3 (MSG_ID_PARAM_SET, SERIAL_ID_TO_IGNORE, parameter, sign, abs);
}

static void query_source ()
{
  const uint8_t param = param_array[param_id];
  if ((param != PARAM_TRACK)
      && (param != PARAM_VOLUME)
      && (param != PARAM_PLAYLIST))
    return;

  const uint8_t mask = (param == PARAM_TRACK) ? FLAG_MASK_TRACK
    : (param == PARAM_VOLUME) ? FLAG_MASK_VOLUME : FLAG_MASK_PLAYLIST;

  if ((param_flag & mask) != 0)
    return;

  const uint8_t msg_body = (param == PARAM_TRACK) ? PARAMETER_TRACK
    : (param == PARAM_VOLUME) ? PARAMETER_VOLUME : PARAMETER_PLAYLIST;

  send_message_1 (MSG_ID_PARAM_QUERY, msg_body);

  param_flag |= (param == PARAM_TRACK) ? FLAG_TRACK_SENT
    : (param == PARAM_VOLUME) ? FLAG_VOLUME_SENT : FLAG_PLAYLIST_SENT;
}

static void state_set (uint8_t new_state)
{
  /*
   * we need to adjust param_array if we are moving
   *  a. from idle => param/value
   *  b. param/value => apply
   *  c. apply => param/value
   */

  const uint8_t connected = mode_is_connnected ();

  if ((state == STATE_IDLE)
      && ((new_state == STATE_PARAM) || (new_state == STATE_VALUE))) {
    param_id = 0;
    if (connected != 0) {
      param_array = param_array_radio;
      param_id_max = sizeof (param_array_radio) / sizeof (uint8_t);
      query_source ();
    } else {
      param_array = param_array_watch;
      param_id_max = sizeof (param_array_watch) / sizeof (uint8_t);
    }
  } else if (((state == STATE_PARAM) || (state == STATE_VALUE))
             && (new_state == STATE_APPLY)) {
    secondary_param = param_array[param_id];
    param_id = PARAM_ID_APPLY;
    param_array = param_array_apply;
    param_id_max = sizeof (param_array_apply) / sizeof (uint8_t);
  } else if ((state == STATE_APPLY)
             && ((new_state == STATE_PARAM) || (new_state == STATE_VALUE))) {
    /* param_id should have right value */
    if (connected != 0) {
      param_array = param_array_radio;
      param_id_max = sizeof (param_array_radio) / sizeof (uint8_t);
      query_source ();
    } else {
      param_array = param_array_watch;
      param_id_max = sizeof (param_array_watch) / sizeof (uint8_t);
    }
  }

  state = new_state;
}

static void reset ()
{
  backup_mode = MODE_MENU;

  param_max[VALUE_BRIGHTNESS] = 0xF;
  param_min[VALUE_BRIGHTNESS] = 0;
  param_max[VALUE_PLAYLIST] = param_min[VALUE_PLAYLIST] = 0;
  param_max[VALUE_TRACK] = param_min[VALUE_TRACK] = 0;
  param_max[VALUE_VOLUME] = param_min[VALUE_VOLUME] = 0;

  param_old[VALUE_BRIGHTNESS]
    = param_new[VALUE_BRIGHTNESS] = flush_brightness_get ();
  param_old[VALUE_PLAYLIST] = param_new[VALUE_PLAYLIST] = 0;
  param_old[VALUE_TRACK] = param_new[VALUE_TRACK] = 0;
  param_old[VALUE_VOLUME] = param_new[VALUE_VOLUME] = 0;

  param_flag = FLAG_MASK_BRIGNHTNESS;

  param_array = param_array_apply;
  param_id = PARAM_ID_CANCEL;
  secondary_param = PARAM_CANCEL;

  state = STATE_IDLE;
}

static void start (uint8_t rotor_id)
{
  reset ();
  backup_mode = mode_get ();
  mode_set (MODE_MENU);
  send_message_0 (MSG_ID_SUSPEND);
  flush_shift_drain_start ();

  state_set ((rotor_id == PARAM_ROTOR) ? STATE_PARAM : STATE_VALUE);
}

static void get_sign_abs (uint8_t src, uint8_t dst, uint8_t *sign, uint8_t *abs)
{
  *sign = PARAMETER_POSITIVE;

  if (src == dst) {
    *abs = 0;
  } else if (src < dst) {
    *abs = dst - src;
  } else {                      /* src > dst */
    *sign = PARAMETER_NEGATIVE;
    *abs = src - dst;
  }
}

/* exit from menu handling */
static void stop ()
{
  const uint8_t param =
    ((state == STATE_IDLE)
     || ((state == STATE_APPLY)
         && (param_id == PARAM_ID_CANCEL))) ? PARAM_CANCEL
    : (state == STATE_APPLY) ? secondary_param
    : param_array[param_id];

  uint8_t abs = 0;
  uint8_t sign = PARAMETER_POSITIVE;

  switch (param) {
  case PARAM_BRIGHTNESS:
    if (param_old[VALUE_BRIGHTNESS] != param_new[VALUE_BRIGHTNESS]) {
      flush_brightness_set (param_new[VALUE_BRIGHTNESS]);
    }
    break;
  case PARAM_CANCEL:
    break;
  case PARAM_NEXT:
    send_message_3 (PARAMETER_TRACK, PARAMETER_POSITIVE, 1);
    break;
  case PARAM_PLAYLIST:
    if ((param_flag & FLAG_MASK_PLAYLIST) == FLAG_MASK_PLAYLIST) {
      get_sign_abs (param_old[VALUE_PLAYLIST],
                    param_new[VALUE_PLAYLIST], &sign, &abs);
      if (abs != 0)
        send_message_3 (PARAMETER_PLAYLIST, sign, abs);
    }
    break;
  case PARAM_PREVIOUS:
    send_message_3 (PARAMETER_TRACK, PARAMETER_NEGATIVE, 1);
    break;
  case PARAM_TRACK:
    if ((param_flag & FLAG_MASK_TRACK) == FLAG_MASK_TRACK) {
      get_sign_abs (param_old[VALUE_TRACK],
                    param_new[VALUE_TRACK], &sign, &abs);
      send_message_3 (PARAMETER_TRACK, sign, abs);
    }
    break;
  case PARAM_VOLUME:
    if ((param_flag & FLAG_MASK_VOLUME) == FLAG_MASK_VOLUME) {
      get_sign_abs (param_old[VALUE_VOLUME],
                    param_new[VALUE_VOLUME], &sign, &abs);
      send_message_3 (PARAMETER_VOLUME, sign, abs);
    }
    break;
  case PARAM_POWER_OFF:
    send_message_0 (MSG_ID_POWEROFF);
    break;
  case PARAM_POWER_ON:
    boot_pi ();
    break;
  case PARAM_REBOOT:
    send_message_0 (MSG_ID_REBOOT);
    break;
  default:
    break;
  }

  flush_shift_drain_stop ();
  mode_set (backup_mode);
  send_message_0 (MSG_ID_RESUME);

  reset ();
}

static void schedule (uint8_t delay)
{
  at_cancel (AT_MENU);
  at_schedule (AT_MENU, delay, &stop);
}

static void select_param (uint8_t action)
{
  if (action == ROTOR_CLOCKWISE) {
    if (param_id < param_id_max - 1) {
      ++param_id;
    }
  } else {                      /* counter-clockwise */
    if (param_id > 0) {
      --param_id;
    }
  }
  query_source ();
}

static void select_value (uint8_t action)
{
  const uint8_t param = param_array[param_id];

  if ((param != PARAM_TRACK)
      && (param != PARAM_VOLUME)
      && (param != PARAM_BRIGHTNESS)
      && (param != PARAM_PLAYLIST))
    return;

  const uint8_t mask = (param == PARAM_TRACK) ? FLAG_TRACK
    : (param == PARAM_VOLUME) ? FLAG_VOLUME
    : (param == PARAM_BRIGHTNESS) ? FLAG_BRIGNHTNESS
    : FLAG_PLAYLIST;

  if ((param_flag & mask) == 0)
    /* range is invalid */
    return;

  const uint8_t id = (param == PARAM_TRACK) ? VALUE_TRACK
    : (param == PARAM_VOLUME) ? VALUE_VOLUME
    : (param == PARAM_BRIGHTNESS) ? VALUE_BRIGHTNESS
    : VALUE_PLAYLIST;

  if (action == ROTOR_CLOCKWISE) {
    if (param_new[id] < param_max[id])
      ++(param_new[id]);
  } else {                      /* counter-clockwise */
    if (param_new[id] > param_min[id])
      --(param_new[id]);
  }
}

static void select_apply (uint8_t action)
{
  /* clockwise => apply, counter-clockwise cancel */
  if (action == ROTOR_CLOCKWISE)
    param_id = PARAM_ID_APPLY;
  else
    param_id = PARAM_ID_CANCEL;
}

static void render_label (struct buf_t *buf, uint8_t param)
{
  switch (param) {
  case PARAM_BRIGHTNESS:
    {
      uint8_t tag[] = {FONT_B, FONT_r};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_TRACK:
    {
      uint8_t tag[] = {FONT_C, FONT_h};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_VOLUME:
    {
      uint8_t tag[] = {FONT_V, FONT_o};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_NEXT:
    {
      uint8_t tag[] = {FONT_N, FONT_e, FONT_x, FONT_t};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_PLAYLIST:
    {
      uint8_t tag[] = {FONT_P, FONT_L};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_PREVIOUS:
    {
      uint8_t tag[] = {FONT_P, FONT_r, FONT_e,
                       FONT_v, FONT_i, FONT_o, FONT_u, FONT_s};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_REBOOT:
    {
      uint8_t tag[] = {FONT_R, FONT_e, FONT_b, FONT_o, FONT_o, FONT_t};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
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
  case PARAM_APPLY:
    {
      uint8_t tag[] = {FONT_A, FONT_p, FONT_p, FONT_l, FONT_y};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
     }
    break;
  case PARAM_CANCEL:
    {
      uint8_t tag[] = {FONT_C, FONT_a, FONT_n, FONT_c, FONT_e, FONT_l};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  default:
    break;
  }
}

static void render_source (struct buf_t *buf, uint8_t param)
{
  if ((param != PARAM_TRACK)
      && (param != PARAM_VOLUME)
      && (param != PARAM_BRIGHTNESS)
      && (param != PARAM_PLAYLIST))
    return;

  render_symbol (buf, FONT_COLON);

  const uint8_t mask = (param == PARAM_BRIGHTNESS) ? FLAG_BRIGNHTNESS
    : (param == PARAM_TRACK) ? FLAG_TRACK
    : (param == PARAM_VOLUME) ? FLAG_VOLUME
    : FLAG_PLAYLIST;

  if ((param_flag & mask) == 0)
    render_symbol (buf, FONT_STAR);
  else {
    const uint8_t id = (param == PARAM_BRIGHTNESS) ? VALUE_BRIGHTNESS
      : (param == PARAM_TRACK) ? VALUE_TRACK
      : (param == PARAM_VOLUME) ? VALUE_VOLUME
      : VALUE_PLAYLIST;
    render_number (buf, param_old[id], RENDER_LEADING_DISABLE);
  }
}

static void render_destination (struct buf_t *buf, uint8_t param)
{
  if ((param != PARAM_TRACK)
      && (param != PARAM_VOLUME)
      && (param != PARAM_BRIGHTNESS)
      && (param != PARAM_PLAYLIST))
    return;

  const uint8_t mask = (param == PARAM_BRIGHTNESS) ? FLAG_BRIGNHTNESS
    : (param == PARAM_TRACK) ? FLAG_TRACK
    : (param == PARAM_VOLUME) ? FLAG_VOLUME
    : FLAG_PLAYLIST;

  if ((param_flag & mask) == 0)
    /* destination is invalid */
    return;

  render_symbol (buf, FONT_COLON);

  const uint8_t id = (param == PARAM_TRACK) ? VALUE_TRACK
    : (param == PARAM_VOLUME) ? VALUE_VOLUME
    : (param == PARAM_BRIGHTNESS) ? VALUE_BRIGHTNESS
    : VALUE_PLAYLIST;
  render_number (buf, param_new[id], RENDER_LEADING_DISABLE);
}

static void render ()
{
  if (state == STATE_IDLE)
    return;

  struct buf_t buf;
  buf_init (&buf);

  buf_byte_fill (&buf, 0);

  uint8_t param = param_array[param_id];
  render_label (&buf, param);

  if ((state == STATE_PARAM)
      || (state == STATE_VALUE)) {
    render_source (&buf, param);
    render_destination (&buf, param);
  }

  render_tail (&buf);
  flush_stable_display (&buf);
}

void menu_handle_rotor (uint8_t id, uint8_t action)
{
  if (action == ROTOR_PUSH)
    return;

  switch (state) {
  case STATE_IDLE:
    start (id);
    schedule (SELECT_DELAY);
    break;
  case STATE_PARAM:
    if (id == PARAM_ROTOR) {
      select_param (action);
    } else {
      state_set (STATE_VALUE);
    }
    schedule (SELECT_DELAY);
    break;
  case STATE_VALUE:
    if (id == VALUE_ROTOR) {
      select_value (action);
      schedule (SELECT_DELAY);
    } else {
      state_set (STATE_APPLY);
      schedule (APPLY_DELAY);
    }
    break;
  case STATE_APPLY:
    if (id == APPLY_ROTOR) {
      select_apply (action);
      schedule (APPLY_DELAY);
    } else {
      state_set (STATE_VALUE);
      schedule (SELECT_DELAY);
    }
    break;
  default:
    break;
  }

  render ();
}

void menu_init ()
{
  backup_mode = MODE_MENU;

  reset ();
}

uint8_t menu_parameter_value (uint8_t parameter, uint8_t value,
                              uint8_t min, uint8_t max)
{
  if ((parameter != PARAMETER_VOLUME)
      && (parameter != PARAMETER_TRACK)
      && (parameter != PARAMETER_PLAYLIST))
    return 0;

  uint8_t id = (parameter == PARAMETER_VOLUME) ? VALUE_VOLUME
    : (parameter == PARAMETER_TRACK) ? VALUE_TRACK
    : VALUE_PLAYLIST;
  param_old[id] = value;
  param_new[id] = value;
  param_min[id] = min;
  param_max[id] = max;

  param_flag |= (parameter == PARAMETER_VOLUME) ? FLAG_VOLUME
    : (parameter == PARAMETER_TRACK) ? FLAG_TRACK
    : FLAG_PLAYLIST;

  schedule (SELECT_DELAY);

  id = (parameter == PARAMETER_VOLUME) ? PARAM_VOLUME
    : (parameter == PARAMETER_TRACK) ? PARAM_TRACK
    : PARAM_PLAYLIST;

  if (id == param_array[param_id])
    render ();

  return 1;
}
