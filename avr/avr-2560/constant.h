/*
 *
 */
#ifndef CONSTANT_H
#define CONSTANT_H

#define ID_CATCH_EYE 250
#define SPI_READ_OVERFLOW 245   /* an error */
#define SPI_WRITE_UNDERFLOW 240 /* not an error */
#define RING_INIT_PATTERN 235

enum {
  MSG_ID_EMPTY,
  MSG_ID_BUTTON,
  MSG_ID_MONO_LED,
  MSG_ID_PING,
  MSG_ID_QUERY,                 /* serial should be in, no response from avr */
  MSG_ID_STATUS,
};

enum {
  STATUS_SUCCESS,
  STATUS_HOLD_ON,
  STATUS_FAIL,
  STATUS_UNKNOWN_MSG
};

#endif
