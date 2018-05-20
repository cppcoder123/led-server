/*
 * Message ids to communicate with mcu
 *
 */
#ifndef LIBLED_MSG_ID_H
#define LIBLED_MSG_ID_H

/*
 * Master message wrapped by 'MSG_ID_MASTER_START' and 'MSG_ID_MASTER_FINISH'
 *  e.g. : <MSG_ID_MASTER_START><MSG_ID_STATUS_x><MSG_ID_MASTER_FINISH>
 *
 * Slave message wrapped by 'MSG_ID_SLAVE_START' and 'MSG_ID_SLAVE_FINISH'
 *  e.g. : <MSG_ID_SLAVE_START><MSG_ID_SLAVE_MSG_ID_BRIGHTNESS><brightness-value><MSG_ID_SLAVE_FINISH>
 */

/* messages directed to master */
enum {
  MSG_ID_MASTER_STATUS = 15,
  MSG_ID_MASTER_BUTTON,
  MSG_ID_MASTER_START = 230, /* Note: It should not be equal to SLAVE_START */
  MSG_ID_MASTER_FINISH,
};

/* messages directed to slave */
enum {
  MSG_ID_SLAVE_EMPTY = 100,
  MSG_ID_SLAVE_MIN,
  MSG_ID_SLAVE_START = MSG_ID_SLAVE_MIN, /* <id> */
  MSG_ID_SLAVE_STOP,                         /* <id> */
  MSG_ID_SLAVE_HANDSHAKE,                    /* <id> */
  MSG_ID_SLAVE_MATRIX, /* <id><size-lsb><size-msb><arr-start><data-1>..<arr-fin> */
  MSG_ID_SLAVE_DELAY,  /* <id><delay-id><delay-data> */
  MSG_ID_SLAVE_BRIGHTNESS,      /* <id><brightness-data> */
  MSG_ID_SLAVE_SWITCH_RELAY,
  MSG_ID_SLAVE_MAX,             /* msg id limit */
  MSG_ID_SLAVE_MAX_LENGTH = 31, /* except matrix */
  MSG_ID_SLAVE_START = 240, /* Note:  It should not be equal to MASTER_START */
  MSG_ID_SLAVE_FINISH,
};

enum {
  MSG_ID_STATUS_RESERVED = 6,
  MSG_ID_STATUS_OK,
  MSG_ID_STATUS_NO_FINISH,
  MSG_ID_STATUS_ID_UNKNOWN,
  MSG_ID_STATUS_LONG_ID_UNKNOWN,
  MSG_ID_STATUS_PARSE_ERROR,
  MSG_ID_STATUS_NO_ARRAY_START,
  MSG_ID_STATUS_NO_ARRAY_FINISH,
  MSG_ID_STATUS_TOO_LONG_MATRIX,
  MSG_ID_STATUS_UNKNOWN_DELAY_ID,
  MSG_ID_STATUS_BRIGHTNESS_OUT_OF_RANGE,
  /**/
  MSG_ID_STATUS_MAX
};

enum {                          /* data to render */
  MSG_ID_MATRIX_ARRAY_START,
  MSG_ID_MATRIX_ARRAY_FINISH,
  MSG_ID_MATRIX_CAPACITY = 600 /* max that mcu can handle fixme: increase ?*/
};

enum {
  MSG_ID_DELAY_SCROLL_SHIFT,    /* scroll step delay */
};

enum {
  MSG_ID_BRIGHTNESS_MIN,
  MSG_ID_BRIGHTNESS_0 = MSG_ID_BRIGHTNESS_MIN, /* first level of brightness */
  MSG_ID_BRIGHTNESS_1,
  MSG_ID_BRIGHTNESS_2,
  MSG_ID_BRIGHTNESS_3,
  MSG_ID_BRIGHTNESS_4,
  MSG_ID_BRIGHTNESS_5,
  MSG_ID_BRIGHTNESS_6,
  MSG_ID_BRIGHTNESS_7,
  MSG_ID_BRIGHTNESS_8,
  MSG_ID_BRIGHTNESS_9,
  MSG_ID_BRIGHTNESS_10,
  MSG_ID_BRIGHTNESS_11,
  MSG_ID_BRIGHTNESS_12,
  MSG_ID_BRIGHTNESS_13,
  MSG_ID_BRIGHTNESS_14,
  MSG_ID_BRIGHTNESS_15,            /* max level */
  MSG_ID_BRIGHTNESS_MAX = MSG_BRIGHTNESS_15,
};

#endif
