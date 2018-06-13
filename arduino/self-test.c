/*
 *
 */

#include <stdint.h>

#include "device-id.h"

#include "matrix-buffer.h"
#include "self-test.h"
#include "spi-write.h"

static void test_pattern (uint8_t pattern)
{
  matrix_buffer_update_symbol (ID_SUB_MATRIX_TYPE_FIRST, pattern);
  for (uint8_t i = 0; i < SPI_WRITE_MATRIX_SIZE - 1; ++i)
    matrix_buffer_update_symbol (ID_SUB_MATRIX_TYPE_MIDDLE, pattern);
  matrix_buffer_update_symbol (ID_SUB_MATRIX_TYPE_LAST, pattern);
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
