/*
 *
 */

#include <stdint.h>

#include "device-id.h"

/* #include "matrix-buffer.h" */
#include "self-test.h"
#include "spi-write.h"

void self_test ()
{
  spi_write_initialize ();

  spi_write_brightness (ID_BRIGHTNESS_MAX);

  spi_write_matrix_test (0xFF, 0, 32);

  /* spi_write_uninitialize (); */
}
