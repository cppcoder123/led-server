/*
 *
 */

#include <stdint.h>

#include "device-id.h"

#include "matrix.h"
#include "matrix-timer.h"
#include "self-test.h"
#include "spi-write.h"

#define DELAY 5                /* ~ .5 sec */

static void test_pattern (uint8_t pattern)
{
  uint8_t buf[SPI_WRITE_MATRIX_SIZE];
  for (uint8_t i = 0; i < SPI_WRITE_MATRIX_SIZE; ++i)
    buf[i] = pattern;

  spi_write_matrix (buf);

  matrix_timer_wait (DELAY);
}

void self_test ()
{
  spi_write_initialize ();
  /* */
  spi_write_brightness (ID_BRIGHTNESS_MAX);
  /* */
  test_pattern (0xFF);
  test_pattern (0x0F);
  test_pattern (0xF0);
  test_pattern (0x00);
  /* */
  spi_write_uninitialize ();
}
