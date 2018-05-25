/*
 * Message ids to communicate with mcu
 *
 */
#ifndef CORE_DEVICE_ID_H
#define CORE_DEVICE_ID_H

/*
 * Message wrapped by 'ID_MSG_START' and 'ID_MSG_FINISH'
 *  e.g. : <ID_MSG_START><MSG_ID_STATUS><status-data><ID_MSG_FINISH>
 *
 * Each message starts with <id><serial-lsb><serial-msb>
 *
 */

/*
 * Max matrix size (just ~100 fat (8 bit wide) symbols)
 */
enum {
  ID_MAX_MATRIX_SIZE = 800
};

/*wrap ids*/
enum {
  ID_MATRIX_START = 251,
  ID_MATRIX_FINISH,
  ID_MSG_START,
  ID_MSG_FINISH
  /*Note: we are at unsigned char limit*/
};

/*message ids*/
enum {
  ID_STATUS = 1,                /**/
  ID_BUTTON,                    /**/
  ID_INIT,                      /**/
  ID_UNINIT,                    /**/
  ID_HANDSHAKE,                 /**/
  ID_MATRIX,    /*<size-lsb><size-msb><arr-start><data-1>..<arr-fin>*/
  ID_SHIFT_DELAY, /*<delay-id-1><delay-data><delay-id-2><delay-data>*/
  ID_BRIGHTNESS                 /**/
};

/*status value*/
enum {
  ID_STATUS_RESERVED = 6,
  ID_STATUS_OK,
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

/*shift delay ids*/
enum {
  ID_SHIFT_PIXEL_DELAY,         /* 1-pixel shift delay */
  ID_SHIFT_MATRIX_DELAY         /* whole phrase delay */
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


#if 0

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
  MSG_ID_SLAVE_INIT = MSG_ID_SLAVE_MIN, /* <id> */
  MSG_ID_SLAVE_UNINIT,                  /* <id> */
  MSG_ID_SLAVE_HANDSHAKE,               /* <id> */
  MSG_ID_SLAVE_MATRIX, /* <id><serial-lsb><serial-msb><size-lsb><size-msb><arr-start><data-1>..<arr-fin> */
  MSG_ID_SLAVE_SHIFT_DELAY, /* <id><serial-lsb><serial-msb><delay-id-1><delay-data><delay-id-2><delay-data> */
  MSG_ID_SLAVE_BRIGHTNESS,      /* <id><serial-lsb><serial-msb><brightness-data> */
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

enum {
  MSG_ID_SHIFT_DELAY,           /* 1-pixel shift delay */
  MSG_ID_SHIFT_PAUSE            /* whole phrase delay */
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
  MSG_ID_BRIGHTNESS_MAX = MSG_ID_BRIGHTNESS_15,
};
#endif

#endif
