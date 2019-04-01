/*
 *
 */
#ifndef CONSTANT_H
#define CONSTANT_H

#define PROTOCOL_VERSION 1

#define EYE_CATCH 250
#define SPI_READ_OVERFLOW 245   /* an error */
#define SPI_WRITE_UNDERFLOW 240 /* not an error */
#define SPI_SPEED_ERROR 235
#define RING_INIT_PATTERN 230

#define SERIAL_ID_TO_IGNORE 0

enum {
  /*should not be sent in either direction, initialization*/
  MSG_ID_EMPTY,

  /*avr=>pi: button(s) is(are) pressed*/
  MSG_ID_BUTTON,

  /*pi=>avr: one column as payload,*/
  /*avr=>pi: STATUS*/
  MSG_ID_MONO_LED,

  /*pi=>avr: No response from avr is expected, SERIAL_ID_TO_IGNORE should be used*/
  /*aux message to enable spi slave (avr) to master (pi) transfer*/
  MSG_ID_QUERY,

  /*avr=>pi: see status below*/
  MSG_ID_STATUS,

  /*pi=>avr: PROTOCOL_VERSION as payload, */
  /*avr=>pi: VERSION: STATUS_SUCCESS(or STATUS_FAIL) as payload*/
  MSG_ID_VERSION,

  /* avr=>pi: request data to display, 0 as payload */
  MSG_ID_POLL,

  /* avr=>pi: debug */
  MSG_ID_DEBUG_A,
  MSG_ID_DEBUG_B,
  MSG_ID_DEBUG_C,
};

enum {
  STATUS_SUCCESS,
  STATUS_HOLD_ON,
  STATUS_FAIL,
  STATUS_UNKNOWN_MSG
};

#endif
