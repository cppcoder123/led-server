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

/*
  Message structure:

  a) EYE_CATCH <size> <serial> <msg-id>
  b) EYE_CATCH <size> <serial> <msg-id> <payload-1>
  c) EYE_CATCH <size> <serial> <msg-id> <payload-1> <payload-2>
  d) ....
*/


enum {
  /* avr=>pi: debug */
  MSG_ID_DEBUG,

  /*should not be sent in either direction, initialization*/
  MSG_ID_EMPTY,

  /* pi=>avr, confirm the version was ok */
  MSG_ID_HANDSHAKE,

  /*pi=>avr: No response from avr is expected, SERIAL_ID_TO_IGNORE should be used*/
  /*aux message to enable spi slave (avr) to master (pi) transfer*/
  MSG_ID_IDLE,

  /*pi=>avr: one column as payload,*/
  /*avr=>pi: STATUS*/
  /* MSG_ID_LED, */
  MSG_ID_LED_ARRAY,

  /*avr=>pi, set param value*/
  MSG_ID_PARAM_SET,

  /* avr=>pi, shutdown pi, param is delay in minutes */
  MSG_ID_POWEROFF,

  /* avr<=>pi, avr asks pi about track/playlist name */
  MSG_ID_QUERY_NAME,

  /*avr=>pi, query param value*/
  /*pi=>avr, param value*/
  MSG_ID_QUERY_NUMBER,

  /*avr=>pi: Reboot pi*/
  MSG_ID_REBOOT,

  /* avr => pi resume info flow */
  MSG_ID_RESUME,

  /*avr=>pi: see status below*/
  /*payload: original MSG_ID_* */
  MSG_ID_STATUS,

  /* avr => pi, suspend info flow */
  MSG_ID_SUSPEND,

  /*pi=>avr send time from OS to avr*/
  /*payload: <hours> <minutes>*/
  /**/
  /*avr=>pi Send current avr time to pi, used for debug*/
  /*payload: <hours> <minutes> <seconds> <second-fraction>*/
  MSG_ID_SYNC_CLOCK,

  /*pi=>avr: PROTOCOL_VERSION as payload, */
  /*avr=>pi: VERSION: STATUS_SUCCESS(or STATUS_FAIL) as payload*/
  MSG_ID_VERSION,
};

enum {
  STATUS_SUCCESS,
  STATUS_FAIL,
  STATUS_UNKNOWN_MSG
};

/* debug domain */
enum {
  DEBUG_BOOST,
  DEBUG_BUZZ,
  DEBUG_CLOCK,
  DEBUG_DECODE,
  DEBUG_FAN,
  DEBUG_FLUSH,
  DEBUG_MENU,
  DEBUG_ROTOR,
  DEBUG_SPI,
  DEBUG_TWI,
  DEBUG_WATCH,
};

/* try to tune params */
enum {
  PARAMETER_GO_AHEAD,
  PARAMETER_NEGATIVE,
  PARAMETER_POSITIVE,
  PARAMETER_PLAYLIST,
  PARAMETER_TRACK,
  PARAMETER_VOLUME,
};

#endif
