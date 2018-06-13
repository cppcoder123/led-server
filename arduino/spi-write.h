/*
 *
 */
#ifndef SPI_WRITE_H
#define SPI_WRITE_H

#include <stdint.h>

/*how many led columns we have*/
#define SPI_WRITE_MATRIX_SIZE 32

/* init module internals */
void spi_write_init ();

/* init display device: led matrix */
void spi_write_initialize ();

void spi_write_uninitialize ();

void spi_write_brightness (uint8_t brightness);

enum {
  SPI_WRITE_FIRST,
  SPI_WRITE_MIDDLE,
  SPI_WRITE_LAST
};
/* 
 * Note:
 *  'type' is a first/middle/last,
 *   exactly 'SPI_WRITE_MATRIX_SIZE' symbols should be supplied
 */
void spi_write_matrix_symbol (uint8_t type, uint8_t symbol);

#endif
