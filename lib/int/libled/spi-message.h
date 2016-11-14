/*
 * Message ids to communicate with mcu
 *
 */
#ifndef LIBLED_SPI_MESSAGE_H
#define LIBLED_SPI_MESSAGE_H

/*#define SPI_SLAVE_EYE_CATCH 0xF1
  #define SPI_MASTER_EYE_CATCH 0xF7*/

enum {                          /* status is the only msg from mcu */
  SPI_MESSAGE_EMPTY,
  SPI_MESSAGE_STATUS,           /* <error-code> */
  SPI_MESSAGE_MIN,              /* for mcu incoming messages */
  SPI_MESSAGE_START = SPI_MESSAGE_MIN, /* <id> */
  SPI_MESSAGE_STOP,             /* <id> */
  SPI_MESSAGE_HANDSHAKE,        /* <id> */
  SPI_MESSAGE_MATRIX,   /* <id><size-lsb><size-msb><arr-start><data-1>..<arr-fin> */
  SPI_MESSAGE_DELAY,            /* <id><delay-id><delay-data> */
  SPI_MESSAGE_BRIGHTNESS,       /* <id><brightness-data> */
  /* SPI_MESSAGE_,*/
  SPI_MESSAGE_MAX,              /* msg id limit */
  SPI_MESSAGE_SLAVE_START = 247, /* start of message directed to slave */
  SPI_MESSAGE_SLAVE_FINISH,     /* finish of the message directed to slavle */
  SPI_MESSAGE_MASTER_START,     /* start of message directed to master */
  SPI_MESSAGE_MASTER_FINISH,    /* finish of the message directed to master */
};

enum {
  SPI_STATUS_RESERVED = 6,
  SPI_STATUS_OK,
  SPI_STATUS_MESSAGE_ID_UNKNOWN,
  SPI_STATUS_LONG_MESSAGE_ID_UNKNOWN,
  SPI_STATUS_PARSE_ERROR,
  SPI_STATUS_NO_ARRAY_START,
  SPI_STATUS_NO_ARRAY_FINISH,
  SPI_STATUS_TOO_LONG_MATRIX,
  SPI_STATUS_UNKNOWN_DELAY_ID,
  SPI_STATUS_BRIGHTNESS_OUT_OF_RANGE,
  //  SPI_STATUS_MASTER_PARSE_ERROR,
  //
  SPI_STATUS_MAX
};

enum {                          /* data to render */
  SPI_MATRIX_ARRAY_START,
  SPI_MATRIX_ARRAY_FINISH,
  SPI_MATRIX_CAPACITY = 600     /* max that mcu can handle fixme: increase ?*/
};

/*enum {
  SPI_CAPACITY_MATRIX_MAX,*/      /* max buffer capacity, 2*capacity < 1024 - <extra> 
                                     where <extra> is memory required by mcu, 
                                     lets say ~200 bytes */
/*};*/

enum {
  SPI_DELAY_SCROLL_SHIFT,       /* scroll step delay */
};

enum {
  SPI_BRIGHTNESS_EMPTY,
  SPI_BRIGHTNESS_MIN,
  SPI_BRIGHTNESS_1 = SPI_BRIGHTNESS_MIN,
  SPI_BRIGHTNESS_2,
  SPI_BRIGHTNESS_3,
  SPI_BRIGHTNESS_4,
  SPI_BRIGHTNESS_5,
  SPI_BRIGHTNESS_6,
  SPI_BRIGHTNESS_7,
  SPI_BRIGHTNESS_8,
  SPI_BRIGHTNESS_9,
  SPI_BRIGHTNESS_10,
  SPI_BRIGHTNESS_11,
  SPI_BRIGHTNESS_12,
  SPI_BRIGHTNESS_13,
  SPI_BRIGHTNESS_14,
  SPI_BRIGHTNESS_15,
  SPI_BRIGHTNESS_16,
  SPI_BRIGHTNESS_MAX = SPI_BRIGHTNESS_16,
};

#endif
