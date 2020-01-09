/*
 *
 */

#include "clock.h"

#define FIFTY_NINE 59
#define TWENTY_THREE 23

#define MINUTE_CHANGED 1
#define MINUTE_NOT_CHANGED 0

static uint8_t second = 0;
static uint8_t minute = 0;
static uint8_t hour = 0;

void clock_init ()
{
  second = 0;
  minute = 0;
  hour = 0;
}

void clock_set(uint8_t new_hour, uint8_t new_minute)
{
    hour = new_hour;
    minute = new_minute;
    second = 0;
}

void clock_get(uint8_t *hour_value, uint8_t *minute_value)
{
    *hour_value = hour;
    *minute_value = minute;
}

uint8_t clock_advance_second()
{
    if (second < FIFTY_NINE) {
        ++second;
        return MINUTE_NOT_CHANGED;
    }

    second = 0;
    if (minute < FIFTY_NINE) {
        ++minute;
        return MINUTE_CHANGED;
    }

    minute = 0;
    if (hour < TWENTY_THREE) {
        ++hour;
        return MINUTE_CHANGED;
    }

    hour = 0;
    return MINUTE_CHANGED;
}
