/*
 *
 */

#include <stdint.h>

#include "unix/constant.h"

#include "at.h"
#include "encode.h"
#include "flush.h"
#include "menu.h"
#include "mode.h"
#include "rotor.h"

/* 5 seconds */
#define MENU_DELAY 5

#define PARAM_ID ROTOR_1

#define MIDDLE 0x7F

static uint8_t restore_mode = MODE_IDLE;

/* static uint8_t param = 0; */
/* static uint8_t value = MIDDLE; */

/* static uint8_t param = 0; */
/* static uint8_t value = 0; */

static void change_param (uint8_t action)
{
  /* fixme */
}

static void change_value (uint8_t action)
{
  /* fixme */
}

static void stop ()
{
  /* fixme send new value*/

  flush_shift_drain_stop ();
  mode_set (restore_mode);
  encode_msg_0 (MSG_ID_RESUME, SERIAL_ID_TO_IGNORE);
}

static void start (uint8_t id, uint8_t action)
{
  if (action == ROTOR_PUSH)
    return;

  if (at_empty (AT_MENU) != 0) {
    at_schedule (AT_MENU, MENU_DELAY, &stop);
    restore_mode = mode_get ();
    mode_set (MODE_IDLE);
    encode_msg_0 (MSG_ID_SUSPEND, SERIAL_ID_TO_IGNORE);
    flush_shift_drain_start ();
  } else {
    at_postpone (AT_MENU);
  }

  if (id == PARAM_ID)
    change_param (action);
  else
    change_value (action);
}

void menu_init ()
{
  rotor_register (&start);
  restore_mode = MODE_IDLE;
}
