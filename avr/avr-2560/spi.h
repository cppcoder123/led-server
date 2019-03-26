/*
 *
 */
#ifndef SPI_INIT_H
#define SPI_INIT_H

#include <stdint.h>

#include "data-type.h"

void spi_init ();

uint8_t spi_read_symbol (data_t *symbol);
uint8_t spi_read_array (data_t *array, uint8_t array_size);

/* uint8_t spi_write_symbol (data_t symbol); */
uint8_t spi_write_array (data_t *array, uint8_t array_size);

uint8_t spi_read_space ();

uint8_t spi_read_size ();

#endif
