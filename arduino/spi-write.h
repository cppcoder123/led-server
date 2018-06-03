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

/* init display device: led matrix, ... */
void spi_write_initialize ();

void spi_write_uninitialize ();

void spi_write_brightness (uint8_t brightness);

void spi_write_matrix (volatile uint8_t *data);

#endif
