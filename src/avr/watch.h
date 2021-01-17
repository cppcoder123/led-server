
/*
 * Watch module, it communicates with RTC module.
 * It sets 1Hz interrupt and reads data from the module
 */
#ifndef WATCH_H
#define WATCH_H

#include <stdint.h>

#define WATCH_HOUR_MAX 23
#define WATCH_MINUTE_MAX 59

uint8_t watch_alarm_set (uint8_t hour, uint8_t minute);
void watch_alarm_get (uint8_t *hour, uint8_t *minute);

uint8_t watch_alarm_state ();
/* 1 - engage, 0 - disengage */
void watch_alarm_control (uint8_t arg);

void watch_init ();
void watch_try ();

void watch_enable ();
void watch_disable ();

/*  */
void watch_set (uint8_t hour, uint8_t minute, uint8_t second);
void watch_get (uint8_t *hour, uint8_t *minute);

#endif

