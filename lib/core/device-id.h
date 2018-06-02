/*
 * Message ids to communicate with mcu
 *
 */
#ifndef CORE_DEVICE_ID_H
#define CORE_DEVICE_ID_H

/*
 * Message starts with 'ID_MSG_START' next field is message length,
 * where length is calculated w/o eye-catcher (ID_MSG_START) and length itself, i.e.:
 *  e.g. : <ID_EYE_CATCH><msg-size-lsb><msg-size-msb><msg-id><serial-lsb><serial-msb> <msg-body>
 *          ^
 *          |
 * Message header
 *
 * Note:
 *  Messages going _to_ arduino have 2 bytes for message size,
 *  we are expecting long message in that direction (more than 255 chars),
 *  but messages going _from_ arduino should be short, so one character (255)
 *  should be enough to code its size.
 */

/*
 * Message header size
 */
enum {
  ID_TO_ARDUINO_HEADER_SIZE = 6,
  ID_FROM_ARDUINO_HEADER_SIZE = 5
};

/*
 * Max matrix size
 */
enum {
  ID_MAX_MATRIX_SIZE = 800 // just ~100 fat (8 bit wide) symbols
};

/*wrap ids*/
enum {
  ID_EYE_CATCH = 254,
  /*Note: we are at unsigned char limit*/
};

/*message ids*/
enum {                          /* msg body */
  /*
   * from arduino
   */
  ID_UNKNOWN_MSG,               /* <invalid msg> */
  ID_MISSING_EYE_CATCH,         /* <wrong-value> != EYE_CATCH */
  ID_HEADER_DECODE_FAILED,      /* <empty> */
  ID_STATUS ,                   /* <value>*/
  ID_BUTTON,                    /* fixme */
  /*
   * to arduino
   */
  ID_INIT,                      /* <empty> */
  ID_UNINIT,                    /* <empty> */
  ID_HANDSHAKE,                 /* <empty> */
  ID_MATRIX,                    /* see below */
  ID_SHIFT_DELAY,               /* <pixel-delay><matrix-delay> fixme */
  ID_STABLE_DELAY,              /* <stable-delay> fixme: implement*/
  ID_BRIGHTNESS,                /* <0-15> */
};

/*
 * Matrix message format:
 *   <data-1><data-2>...<data-(msg-size minus 3)>
 *  3 = 2 bytes for serial id + 1 byte for message id
 *
 * Shift-delay format:
 *   <pixel-delay><matrix-delay>
 */


/*status value*/
enum {
  ID_STATUS_OK = 7,
  ID_STATUS_THROTTLE,
  /**/
  ID_STATUS_NO_FINISH,
  ID_STATUS_ID_UNKNOWN,
  ID_STATUS_LONG_ID_UNKNOWN,
  ID_STATUS_PARSE_ERROR,
  ID_STATUS_NO_ARRAY_START,
  ID_STATUS_NO_ARRAY_FINISH,
  ID_STATUS_TOO_LONG_MATRIX,
  ID_STATUS_UNKNOWN_DELAY_ID,
  ID_STATUS_BRIGHTNESS_OUT_OF_RANGE,
  /**/
  MSG_ID_STATUS_MAX
};

enum {
  ID_BRIGHTNESS_MIN,
  ID_BRIGHTNESS_0 = ID_BRIGHTNESS_MIN, /* first level of brightness */
  ID_BRIGHTNESS_1,
  ID_BRIGHTNESS_2,
  ID_BRIGHTNESS_3,
  ID_BRIGHTNESS_4,
  ID_BRIGHTNESS_5,
  ID_BRIGHTNESS_6,
  ID_BRIGHTNESS_7,
  ID_BRIGHTNESS_8,
  ID_BRIGHTNESS_9,
  ID_BRIGHTNESS_10,
  ID_BRIGHTNESS_11,
  ID_BRIGHTNESS_12,
  ID_BRIGHTNESS_13,
  ID_BRIGHTNESS_14,
  ID_BRIGHTNESS_15,            /* max level */
  ID_BRIGHTNESS_MAX = ID_BRIGHTNESS_15,
};

#endif
