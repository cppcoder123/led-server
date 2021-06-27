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

#define DELAY_APPLY 1
#define DELAY_READ 8
#define DELAY_SELECT 5
#define DELAY_TAG 2

#define APPLY_INDENT 7

#define ROTOR_VOLUME ROTOR_4
#define ROTOR_TRACK ROTOR_3
#define ROTOR_PLAYLIST ROTOR_2
#define ROTOR_PARAM ROTOR_1
#define ROTOR_APPLY ROTOR_0

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
  /* designated rotor knob */
  PARAM_TRACK,                  /* select radio station (or mp3) */
  PARAM_VOLUME,                 /* tune volume */
  PARAM_PLAYLIST,               /* select/load playlist */
  /* param list in radio mode*/
  PARAM_POWER_OFF,              /* 'Off' */
  PARAM_PREVIOUS,               /* previous track */
  PARAM_NEXT,                   /* next track */
  PARAM_STOP,                   /* 'mpc stop' */
  PARAM_PLAY,                   /* 'mpc play' */
  PARAM_REBOOT,                 /* reboot pi */
  /* all rotors in watch mode*/
  PARAM_BRIGHTNESS,             /* select brightness */
  /* param list in watch mode*/
  PARAM_POWER_ON,               /* 'On' */
  /* param values in 'apply' */
  PARAM_CANCEL,                 /* cancel param change */
  PARAM_APPLY,
};

/* adjustable params in radio mode */
#define PARAM_VALUE_START PARAM_TRACK
#define PARAM_VALUE_FINISH PARAM_PLAYLIST

/* boolean params in radio mode */
#define PARAM_LIST_START PARAM_POWER_OFF
#define PARAM_LIST_FINISH PARAM_REBOOT

/* adjustable params in watch mode */
#define PARAM_WATCH_START PARAM_BRIGHTNESS
#define PARAM_WATCH_FINISH PARAM_BRIGHTNESS

/* boolean params in watch mode */
#define PARAM_ON_START PARAM_POWER_ON
#define PARAM_ON_FINISH PARAM_POWER_ON

/* boolean apply/cancel */
#define PARAM_APPLY_START PARAM_CANCEL
#define PARAM_APPLY_FINISH PARAM_APPLY

enum {
  STATE_APPLY,
  STATE_IDLE,
  STATE_PARAM,
  STATE_TAG,
  STATE_VALUE,
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

static uint8_t param_id = PARAM_WATCH_START;
static uint8_t param_id_min = PARAM_WATCH_START;
static uint8_t param_id_max = PARAM_WATCH_FINISH;
/* primary param is param_array[param_id] */
static uint8_t secondary_param = PARAM_CANCEL;

static uint8_t state = STATE_IDLE;

static uint8_t tag_param = PARAM_CANCEL;
static uint8_t tag_value = 0;

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

static void send_message_2 (uint8_t msg_id,
                            uint8_t payload_1, uint8_t payload_2)
{
  if (mode_is_connnected () != 0)
    encode_msg_2 (msg_id, SERIAL_ID_TO_IGNORE, payload_1, payload_2);
}

static void send_message_3 (uint8_t parameter, uint8_t sign, uint8_t abs)
{
  if (mode_is_connnected () == 0)
    return;

  encode_msg_3 (MSG_ID_PARAM_SET, SERIAL_ID_TO_IGNORE, parameter, sign, abs);
}

static void query_source ()
{
  /* const uint8_t param = param_array[param_id]; */
  if ((param_id != PARAM_TRACK)
      && (param_id != PARAM_VOLUME)
      && (param_id != PARAM_PLAYLIST))
    return;

  const uint8_t mask = (param_id == PARAM_TRACK) ? FLAG_MASK_TRACK
    : (param_id == PARAM_VOLUME) ? FLAG_MASK_VOLUME : FLAG_MASK_PLAYLIST;

  if ((param_flag & mask) != 0)
    return;

  const uint8_t msg_body = (param_id == PARAM_TRACK) ? PARAMETER_TRACK
    : (param_id == PARAM_VOLUME) ? PARAMETER_VOLUME : PARAMETER_PLAYLIST;

  send_message_1 (MSG_ID_QUERY_NUMBER, msg_body);

  param_flag |= (param_id == PARAM_TRACK) ? FLAG_TRACK_SENT
    : (param_id == PARAM_VOLUME) ? FLAG_VOLUME_SENT : FLAG_PLAYLIST_SENT;
}

static void set_param_range (uint8_t new_state)
{
  uint8_t is_connected = mode_is_connnected ();

  switch (new_state) {
  case STATE_APPLY:
    param_id_min = PARAM_APPLY_START;
    param_id_max = PARAM_APPLY_FINISH;
    break;
  case STATE_PARAM:
  case STATE_VALUE:
    param_id_min = (is_connected != 0) ? PARAM_LIST_START : PARAM_ON_START;
    param_id_max = (is_connected != 0) ? PARAM_LIST_FINISH : PARAM_ON_FINISH;
    break;
  default:
    break;
  }

  if (new_state != STATE_VALUE)
    param_id = param_id_min;
}

static void set_state (uint8_t new_state)
{
  if (state == new_state)
    return;

  if (state == STATE_TAG) {
    send_message_0 (MSG_ID_SUSPEND);
    flush_shift_drain_start ();
    mode_set (MODE_MENU);
  } else if (new_state == STATE_TAG) {
    flush_shift_drain_stop ();
    send_message_0 (MSG_ID_RESUME);
    mode_set (MODE_RADIO);
  }

  /*
   * we need to adjust param_id_min/max if we are moving
   *  a. from idle/apply => param/value
   *  b. param/value => apply
   */
  if (((state == STATE_IDLE) || (state == STATE_APPLY))
      && ((new_state == STATE_PARAM) || (new_state == STATE_VALUE))) {
    /* param_id = 0; */
    set_param_range (new_state);
  } else if (((state == STATE_PARAM) || (state == STATE_VALUE))
             && (new_state == STATE_APPLY)) {
    secondary_param = param_id;
    set_param_range (new_state);
    /* param_id = PARAM_APPLY; */
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

  param_id = PARAM_TRACK;
  secondary_param = PARAM_TRACK;

  state = STATE_IDLE;

  tag_param = PARAM_TRACK;
  tag_value = 0;
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

static void start (uint8_t rotor_id)
{
  /* current state is idle we can't use 'APPLY' ignore it */
  if (rotor_id == ROTOR_APPLY)
    return;

  reset ();
  backup_mode = mode_get ();
  mode_set (MODE_MENU);
  send_message_0 (MSG_ID_SUSPEND);
  flush_shift_drain_start ();
}

/* exit from menu handling */
static void stop ()
{
  if (state == STATE_TAG)
    set_state (STATE_VALUE);

  const uint8_t param =
    ((state == STATE_IDLE)
     || ((state == STATE_APPLY) && (param_id == PARAM_CANCEL))) ? PARAM_CANCEL
    : (state == STATE_APPLY) ? secondary_param
    : param_id;

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
  case PARAM_PLAY:
    send_message_3 (PARAMETER_GO_AHEAD, PARAMETER_POSITIVE, PARAMETER_POSITIVE);
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
  case PARAM_STOP:
    send_message_3 (PARAMETER_GO_AHEAD, PARAMETER_NEGATIVE, PARAMETER_NEGATIVE);
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

static void tag_request ()
{
  if (state != STATE_VALUE)
    return;
  if (param_id != tag_param)
    return;
  uint8_t value_id = (param_id == PARAM_PLAYLIST) ? VALUE_PLAYLIST : VALUE_TRACK;
  if (tag_value != param_new[value_id])
    return;

  uint8_t parameter = (param_id == PARAM_PLAYLIST)
    ? PARAMETER_PLAYLIST : PARAMETER_TRACK;
  send_message_2 (MSG_ID_QUERY_NAME, parameter, tag_value);
}

static void schedule_tag ()
{
  if (state != STATE_VALUE)
    return;

  if ((param_id != PARAM_PLAYLIST)
      && (param_id != PARAM_TRACK))
    return;

  at_cancel (AT_MENU_TAG);

  tag_param = param_id;
  uint8_t value_id = (param_id == PARAM_PLAYLIST) ? VALUE_PLAYLIST : VALUE_TRACK;
  tag_value = param_new[value_id];

  at_schedule (AT_MENU_TAG, DELAY_TAG, &tag_request);
}

static void select_param (uint8_t action)
{
  if ((action == ROTOR_CLOCKWISE)
      && (param_id < param_id_max))
    ++param_id;
  else if ((action == ROTOR_COUNTER_CLOCKWISE)
           && (param_id > param_id_min))
    --param_id;

  /* query_source (); */
}

static void select_value (uint8_t action)
{
  if ((param_id != PARAM_TRACK)
      && (param_id != PARAM_VOLUME)
      && (param_id != PARAM_BRIGHTNESS)
      && (param_id != PARAM_PLAYLIST))
    return;

  const uint8_t mask = (param_id == PARAM_TRACK) ? FLAG_TRACK
    : (param_id == PARAM_VOLUME) ? FLAG_VOLUME
    : (param_id == PARAM_BRIGHTNESS) ? FLAG_BRIGNHTNESS
    : FLAG_PLAYLIST;

  if ((param_flag & mask) == 0)
    /* range is invalid */
    return;

  const uint8_t id = (param_id == PARAM_TRACK) ? VALUE_TRACK
    : (param_id == PARAM_VOLUME) ? VALUE_VOLUME
    : (param_id == PARAM_BRIGHTNESS) ? VALUE_BRIGHTNESS
    : VALUE_PLAYLIST;

  if ((action == ROTOR_CLOCKWISE)
      && (param_new[id] < param_max[id]))
    ++(param_new[id]);
  else if ((action == ROTOR_COUNTER_CLOCKWISE)
           && (param_new[id] > param_min[id]))
    --(param_new[id]);
}

static void select_apply (uint8_t action)
{
  /* clockwise => apply, counter-clockwise cancel */
  if (action == ROTOR_CLOCKWISE)
    param_id = PARAM_APPLY;
  else
    param_id = PARAM_CANCEL;

  /* debug_2 (DEBUG_MENU, 100, action, param_id); */
}

static void render_label (struct buf_t *buf, uint8_t param)
{
  switch (param) {
  case PARAM_APPLY:
    {
      uint8_t tag[] = {FONT_A, FONT_p, FONT_p, FONT_l, FONT_y};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
     }
    break;
  case PARAM_BRIGHTNESS:
    {
      uint8_t tag[] = {FONT_B, FONT_r};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_CANCEL:
    {
      uint8_t tag[] = {FONT_C, FONT_a, FONT_n, FONT_c, FONT_e, FONT_l};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_NEXT:
    {
      uint8_t tag[] = {FONT_N, FONT_e, FONT_x, FONT_t};
      render_word (buf, tag, sizeof (tag) / sizeof (uint8_t));
    }
    break;
  case PARAM_PLAY:
    {
      uint8_t tag[] = {FONT_P, FONT_l, FONT_a, FONT_y};
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
      uint8_t tag[] = {FONT_P, FONT_r, FONT_e, FONT_v};
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
  case PARAM_STOP:
    {
      uint8_t tag[] = {FONT_S, FONT_t, FONT_o, FONT_p};
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

  render_label (&buf, param_id);

  if ((state == STATE_PARAM)
      || (state == STATE_VALUE)) {
    render_source (&buf, param_id);
    render_destination (&buf, param_id);
  }

  render_tail (&buf);
  flush_stable_display (&buf);
}

static uint8_t rotor_to_state (uint8_t id)
{
  if (id == ROTOR_APPLY)
    return STATE_APPLY;
  else if (id == ROTOR_PARAM)
    return STATE_PARAM;

  return STATE_VALUE;
}

static uint8_t rotor_to_param (uint8_t id)
{
  if (id == ROTOR_APPLY)
    return PARAM_APPLY;

  if (mode_is_connnected () == 0)
    return (id == ROTOR_PARAM) ? PARAM_POWER_ON : PARAM_BRIGHTNESS;

  return (id == ROTOR_VOLUME) ? PARAM_VOLUME
    : (id == ROTOR_TRACK) ? PARAM_TRACK
    : (id == ROTOR_PLAYLIST) ? PARAM_PLAYLIST
    : PARAM_POWER_OFF;          /* ??? bad value should be here */
}

void menu_handle_rotor (uint8_t id, uint8_t action)
{
  uint8_t rotor_state = rotor_to_state (id);
  if ((rotor_state == STATE_APPLY)
      && (state == STATE_IDLE))
    return;

  /* debug_2 (DEBUG_MENU, 111, id, action); */

  if (state == rotor_state) {
    if (state == STATE_APPLY)
      select_apply (action);
    else if (state == STATE_PARAM)
      select_param (action);
    else if (state == STATE_VALUE) {
      if (param_id != rotor_to_param (id)) {
        param_id = rotor_to_param (id);
        query_source ();
      }
      select_value (action);
      schedule_tag ();
    }
    schedule ((state == STATE_APPLY) ? DELAY_APPLY : DELAY_SELECT);
    render ();
    return;
  }

  if (state == STATE_IDLE)
    start (id);

  set_state (rotor_state);

  if (rotor_state == STATE_APPLY)
    select_apply (action);
  else if (rotor_state == STATE_PARAM)
    select_param (action);
  else if (rotor_state == STATE_VALUE) {
    param_id = rotor_to_param (id);
    select_value (action);
    query_source ();
  }
  
  schedule ((rotor_state == STATE_APPLY) ? DELAY_APPLY : DELAY_SELECT);

  render ();
}

void menu_init ()
{
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

  schedule (DELAY_SELECT);

  id = (parameter == PARAMETER_VOLUME) ? PARAM_VOLUME
    : (parameter == PARAMETER_TRACK) ? PARAM_TRACK
    : PARAM_PLAYLIST;

  if (id == param_id)
    render ();

  return 1;
}

uint8_t menu_parameter_name (uint8_t parameter, uint8_t id)
{
  if ((parameter != PARAMETER_PLAYLIST)
      && (parameter != PARAMETER_TRACK))
    return 0;

  uint8_t param = (parameter == PARAMETER_PLAYLIST)
    ? PARAM_PLAYLIST : PARAM_TRACK;
  if ((param != param_id)
      || (param != tag_param)
      || (id != tag_value))
    /* too late */
    return 1;

  schedule (DELAY_READ);

  set_state (STATE_TAG);

  return 1;
}
