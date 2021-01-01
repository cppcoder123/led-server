
/*
 * Watch module, it communicates with RTC module.
 * It sets 1Hz interrupt and reads data from the module
 */
#ifndef WATCH_H
#define WATCH_H

#include <stdint.h>

void watch_init ();
void watch_try ();

void watch_enable ();
void watch_disable ();

/*  */
void watch_write (uint8_t hour, uint8_t minute, uint8_t second);

#endif

