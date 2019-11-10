/*
 *
 */
#ifndef CONSTANT_H
#define CONSTANT_H

#define PROTOCOL_VERSION 1

#define SERIAL_ID_TO_IGNORE 0

#define EYE_CATCH 250

#define SPI_READ_OVERFLOW 245   /* an error */
#define SPI_WRITE_UNDERFLOW 240 /* not an error */
#define SPI_SPEED_ERROR 235

#define QUEUE_INIT_PATTERN 230

#define MSG_OVERHEAD 6 /* it is less actually */
#define LED_ARRAY_SIZE 20

enum {
  /*should not be sent in either direction, initialization*/
  MSG_ID_EMPTY,

  /*avr=>pi: button(s) is(are) pressed*/
  /*payload-1: button state, payload-2: prev button state*/
  MSG_ID_BUTTON,

  /*pi=>avr: one column as payload,*/
  /*avr=>pi: STATUS*/
  MSG_ID_MONO_LED,
  MSG_ID_MONO_LED_ARRAY,

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

  /* avr => pi: board is ready to operate, 0 as payload*/
  MSG_ID_BOARD_ENABLED,

  /* notify twi read error */
  /* avr => pi: error value as payload */
  MSG_ID_BOARD_READ_ERROR,

  /* notify twi write error */
  /* avr => pi: error value as payload */
  MSG_ID_BOARD_WRITE_ERROR,

  /* avr => pi: error during finger detection, error as payload */
  MSG_ID_BOARD_DETECT_ERROR,

  /* avr => pi: error during finger detection handling, mode as payload */
  MSG_ID_BOARD_HANDLE_ERROR,

  /* avr=>pi: debug */
  MSG_ID_DEBUG,
  MSG_ID_DEBUG_A,               /* 14 */
  MSG_ID_DEBUG_B,
  MSG_ID_DEBUG_C,
  MSG_ID_DEBUG_D,               /* 17 */
  MSG_ID_DEBUG_E,
  MSG_ID_DEBUG_F,
  MSG_ID_DEBUG_G,               /* 20 */
  MSG_ID_DEBUG_H,               /* 21 */
  MSG_ID_DEBUG_I,               /* 22 */
  MSG_ID_DEBUG_J,
  MSG_ID_DEBUG_K,
  MSG_ID_DEBUG_L,               /* 25 */
  MSG_ID_DEBUG_M,
  MSG_ID_DEBUG_N,               /* 27 */
  MSG_ID_DEBUG_O,
  MSG_ID_DEBUG_P,
  MSG_ID_DEBUG_Q,
  MSG_ID_DEBUG_R,               /* 31 */
  MSG_ID_DEBUG_S,
  MSG_ID_DEBUG_T,
  MSG_ID_DEBUG_U,
  MSG_ID_DEBUG_V,
  MSG_ID_DEBUG_W,
  MSG_ID_DEBUG_X,               /* 37 */
  MSG_ID_DEBUG_Y,
  MSG_ID_DEBUG_Z,
};

enum {
  STATUS_SUCCESS,
  STATUS_HOLD_ON,
  STATUS_FAIL,
  STATUS_UNKNOWN_MSG
};

#endif
