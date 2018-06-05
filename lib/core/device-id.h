/*
 * Message ids to communicate with mcu
 *
 */
#ifndef CORE_DEVICE_ID_H
#define CORE_DEVICE_ID_H

/*
 * Message starts with 'ID_MSG_START' next field is message length,
 * where length is calculated w/o eye-catcher (ID_MSG_START) and length itself, i.e.:
 *  e.g. : <ID_EYE_CATCH><msg-size><msg-id><serial-id><msg-body>
 *          ^
 *          |
 * Message header (4 bytes)
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
  ID_HEADER_SIZE = 4,
};

/*
 * Max matrix size
 */
enum {
  ID_MAX_MATRIX_SIZE = 800,     // just ~100 fat (8 bit wide) symbols
  ID_MAX_SUB_MATRIX_SIZE = 195
};

/*wrap ids*/
enum {
  ID_EYE_CATCH = 254,
  /*Note: we are at unsigned char limit*/
};

/*message ids*/
enum {                          /* msg body */
  ID_INVALID_MSG,               /* invalid msg id */
  /*
   * from arduino
   */
  ID_BUTTON,                    /* fixme */
  ID_HEADER_DECODE_FAILED,      /* <wrong symbol instead of eye catch> */
  ID_STATUS,                    /* <value>*/
  /*
   * to arduino
   */
  ID_BRIGHTNESS,                /* <0-15> */
  ID_HANDSHAKE,                 /* <empty> */
  ID_INIT,                      /* <empty> turn on matrix driver IC*/
  ID_SUB_MATRIX,                /* see below */
  ID_PIXEL_DELAY,               /* <pixel-delay> */
  ID_PHRASE_DELAY,              /* <phrase-delay> */
  ID_STABLE_DELAY,              /* <stable-delay> fixme: implement*/
  ID_UNINIT,                    /* <empty> turn off matrix driver IC*/
};

/*
 * Matrix message format:
 *   <sub-matrix-type><data-1><data-2>...<data-(msg-size minus 1)>
 *
 */


/*status value*/
enum {
  ID_STATUS_OK = 7,
  ID_STATUS_BUFFER_CORRUPTED_0,
  ID_STATUS_BUFFER_CORRUPTED_1,
  ID_STATUS_BUFFER_CORRUPTED_2,
  ID_STATUS_DRAIN_FAILURE,
  ID_STATUS_HELLO,
  ID_STATUS_MSG_UNKNOWN_H,
  ID_STATUS_MSG_UNKNOWN_0,
  ID_STATUS_MSG_UNKNOWN_1,
  ID_STATUS_SUB_MATRIX_TOO_SHORT,
  ID_STATUS_SUB_MATRIX_UPDATE_START_FAILURE,
  ID_STATUS_SUB_MATRIX_UPDATE_FAILURE,
  ID_STATUS_SUB_MATRIX_UPDATE_FINISH_FAILURE,
  ID_STATUS_THROTTLE
};

enum {
  ID_SUB_MATRIX_TYPE_FIRST = (1 << 0),
  ID_SUB_MATRIX_TYPE_MIDDLE = (1 << 1),
  ID_SUB_MATRIX_TYPE_LAST = (1 << 2),
  ID_SUB_MATRIX_TYPE_MASK = (ID_SUB_MATRIX_TYPE_FIRST | \
                             ID_SUB_MATRIX_TYPE_MIDDLE | ID_SUB_MATRIX_TYPE_LAST)
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

enum {
  ID_BUTTON_MIN = (1 << 0),
  ID_BUTTON_0 = ID_BUTTON_MIN,
  ID_BUTTON_1 = (1 << 1),
  ID_BUTTON_2 = (1 << 2),
  ID_BUTTON_MAX = ID_BUTTON_2,
  ID_BUTTON_MASK = (ID_BUTTON_0 | ID_BUTTON_1 | ID_BUTTON_2)
};

#endif
