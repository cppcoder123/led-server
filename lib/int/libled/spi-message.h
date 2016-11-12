/*
 * Message ids to communicate with mcu
 *
 */
#ifndef LIBLED_SPI_MESSAGE_H
#define LIBLED_SPI_MESSAGE_H

enum {
  SPI_MESSAGE_RESERVED,
  SPI_MESSAGE_STATUS,           /* mcu =>, <id><src-id><error-code> */
  SPI_MESSAGE_HANDSHAKE,        /* mcu <=, <id> */
  SPI_MESSAGE_DATA,             /* mcu <=, 
                                   <id><data-id><size-msb><size-lsb><data-1>.. */
  SPI_MESSAGE_DELAY,            /* mcu <=, <id><delay-id><delay-data> */
  SPI_MESSAGE_BRIGHTNESS,       /* mcu <=, <id><brightness-data> */
  /* SPI_MESSAGE_,              /\* mcu *\/ */
  SPI_MESSAGE_MAX               /* msg id limit */
};

enum {
  SPI_STATUS_OK,                /* zero is OK*/
  SPI_STATUS_FAIL,              /*  */
  SPI_STATUS_BAD_MESSAGE,       /*  */
};

enum {
  SPI_DATA_MATRIX,              /* data to render */
};

enum {
  SPI_CAPACITY_MATRIX_MAX,      /* max buffer capacity, 2*capacity < 1024 - <extra> 
                                   where <extra> is memory required by mcu, 
                                   lets say ~200 bytes */
};

enum {
  SPI_DELAY_SCROLL_SHIFT,       /* scroll step delay */
};

enum {
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
