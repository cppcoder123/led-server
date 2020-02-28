/*
 *
 */

#include "boost.h"
#include "feedback.h"

/* 12 volt, fixme check */
#define TARGET 125

/* suitable delta ? 10% fixme */
#define DELTA 12

/* delay , 5 times? fixme */
#define DELAY 5

static struct feedback_t feedback;

static void control (uint8_t current)
{
  /* fixme */
}

void boost_init ()
{
  /* init sw part */
  feedback_init (&feedback, TARGET, DELTA, DELAY, &control);
  /* init hw part, fixme */
}

void boost_uninit ()
{
  /* uninit hw part, fixme */
}

void boost_try ()
{
  /* fixme */
  feedback_try (&feedback);
}
