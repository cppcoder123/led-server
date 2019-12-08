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
#define BUFFER_INIT_PATTERN QUEUE_INIT_PATTERN

#define MSG_OVERHEAD 6 /* it is less actually */
#define LED_ARRAY_SIZE 20

enum {
  /* avr=>pi: debug */
  MSG_ID_DEBUG,

  /*should not be sent in either direction, initialization*/
  MSG_ID_EMPTY,

  /*pi=>avr: one column as payload,*/
  /*avr=>pi: STATUS*/
  MSG_ID_LED,
  MSG_ID_LED_ARRAY,

  /* avr=>pi: request data to display, 0 as payload */
  MSG_ID_POLL,

  /*pi=>avr: No response from avr is expected, SERIAL_ID_TO_IGNORE should be used*/
  /*aux message to enable spi slave (avr) to master (pi) transfer*/
  MSG_ID_QUERY,

  /* avr=>pi: rotary encoder related messages */
  MSG_ID_ROTOR,

  /*avr=>pi: see status below*/
  MSG_ID_STATUS,

  /*pi=>avr: PROTOCOL_VERSION as payload, */
  /*avr=>pi: VERSION: STATUS_SUCCESS(or STATUS_FAIL) as payload*/
  MSG_ID_VERSION,
};

enum {
  STATUS_SUCCESS,
  STATUS_HOLD_ON,
  STATUS_FAIL,
  STATUS_UNKNOWN_MSG
};

/* debug domain */
enum {
  DEBUG_DECODE,
  DEBUG_FLUSH,
  DEBUG_ROTOR,
  DEBUG_SPI,
};

/* debug key */
enum {
  DEBUG_0,
  DEBUG_1,
  DEBUG_2,
  DEBUG_3,
  DEBUG_4,
  DEBUG_5,
  DEBUG_6,
  DEBUG_7,
  DEBUG_8,
  DEBUG_9,
  DEBUG_10,
  DEBUG_11,
  DEBUG_12,
  DEBUG_13,
  DEBUG_14,
  DEBUG_15,
  DEBUG_16,
  DEBUG_17,
  DEBUG_18,
  DEBUG_19,
  DEBUG_20,
};

enum {
  ROTOR_A,
  ROTOR_B,
};

enum {
  ROTOR_PLUS,
  ROTOR_MINUS,
  ROTOR_PUSH,
};

#endif
