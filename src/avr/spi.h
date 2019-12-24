/*
 *
 */
#ifndef SPI_INIT_H
#define SPI_INIT_H

#include <stdint.h>

void spi_init ();

uint8_t spi_read_symbol (uint8_t *symbol);
uint8_t spi_read_array (uint8_t *array, uint8_t array_size);

/* uint8_t spi_write_symbol (uint8_t symbol); */
uint8_t spi_write_array (uint8_t *array, uint8_t array_size);

uint8_t spi_read_space ();

uint8_t spi_write_space ();

uint8_t spi_read_size ();

#endif
