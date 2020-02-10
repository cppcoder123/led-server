/*
 *
 */

#include "buf.h"
#include "counter.h"
#include "invoke.h"

#define INVOKE_COUNTER COUNTER_2
#define INVOKE_PRESCALER COUNTER_PRESCALER_1024

#define HUNDRED_TIMES_PER_SECOND 39
#define TWENTY_FIVE_TIMES_PER_SECOND 160

static invoke_callback callback_array[INVOKE_ID_MAX];
static uint8_t factor_array[INVOKE_ID_MAX];
static uint8_t counter_array[INVOKE_ID_MAX];

static volatile struct buf_t queue;

void invoke_init ()
{
  for (uint8_t i = 0; i < INVOKE_ID_MAX; ++i) {
    callback_array[i] = 0;
    factor_array[i] = 0;
    counter_array[i] = 0;
  }

  buf_init (&queue);
}

void invoke_try ()
{
  uint8_t id = INVOKE_ID_MAX;
  if ((buf_byte_drain (&queue, &id) == 0)
      || (id >= INVOKE_ID_MAX)  /* ? */
      || (callback_array[id] == 0))
    return;

  callback_array[id] ();
}

static void interrupt_function ()
{
  for (uint8_t i = 0; i < INVOKE_ID_MAX; ++i)
    if (++(counter_array[i]) >= factor_array[i]) {
      counter_array[i] = 0;
      buf_byte_fill (&queue, i);
    }
}

static void engage_timer ()
{
  /* counter_prescaler (INVOKE_COUNTER, COUNTER_PRESCALER_1024); */
  counter_interrupt (INVOKE_COUNTER,
                     COUNTER_INTERRUPT_COMPARE_A, interrupt_function);
  /* counter_set_compare_a (INVOKE_COUNTER, HUNDRED_TIMES_PER_SECOND, 0); */
  counter_set_compare_a (INVOKE_COUNTER, TWENTY_FIVE_TIMES_PER_SECOND, 0);
  counter_enable (INVOKE_COUNTER, INVOKE_PRESCALER);
}

static void disengage_timer ()
{
  counter_disable (INVOKE_COUNTER);
  /* ? */
  /* counter_interrupt (INVOKE_COUNTER, COUNTER_INTERRUPT_COMPARE_A, 0); */
}

static uint8_t is_required ()
{
  uint8_t status = 0;
  for (uint8_t i = 0; i < INVOKE_ID_MAX; ++i)
    if (callback_array[i] != 0) {
      status = 1;
      break;
    }

  return status;
}

uint8_t invoke_enable (uint8_t id, uint8_t factor, invoke_callback callback)
{
  if (id >= INVOKE_ID_MAX)
    return 0;

  uint8_t required = is_required ();

  callback_array[id] = callback;
  factor_array[id] = factor;
  counter_array[id] = 0;

  if (required != 0)
    engage_timer ();

  return 1;
}

uint8_t invoke_disable (uint8_t id)
{
  if (id >= INVOKE_ID_MAX)
    return 0;

  callback_array[id] = 0;
  factor_array[id] = 0;
  counter_array[id] = 0;

  if (is_required () == 0)
    disengage_timer ();

  return 1;
}
