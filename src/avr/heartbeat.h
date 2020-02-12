/*
 *
 */
#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <stdint.h>

void heartbeat_init ();

void heartbeat_confirm ();

typedef void (*heartbeat_callback) ();
/*
 * 'delay' - heartbeat delay step is '0.02' sec
 * 'max' - number of times we allowed to miss action 
 * 'report_cb' - report no action after provoke
 * 'provoke_cb' - initiate some action
 */
void heartbeat_start (uint8_t delay, uint8_t max,
                      heartbeat_callback report_cb,
                      heartbeat_callback provoke_cb);

void heartbeat_cancel ();

#endif
