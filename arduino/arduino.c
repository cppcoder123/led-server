/*
 *
 *
 *
 *
 *
 */

#include <stdint.h>

#include "device-id.h"

#include "parse.h"

/* #define BUFFER_NAME uart_read_buffer */
/* #define BUFFER_MAX_SIZE 800 */
/* #define BUFFER_SIZE_TYPE uint16_t */
/* #include "buffer.h" */

#if 0
enum {
  MATRIX_READ_STARTED = (1 << 0),
  MATRIX_READ_FINISHED = (1 << 1),
  MATRIX_READ_MASK = (MATRIX_READ_STARTED | MATRIX_READ_FINISHED),
  /**/
  MATRIX_COPY_FINISHED = (1 << 2),
  /**/
  /*First matrix message raises the bit, and last drops*/
  MATRIX_THROTTLE = (1 << 4)
};

volatile uint8_t matrix_flag = 0;

/*
 * It is ok to read if copy is finished or read started and not finished
 * and we are not in throttling
 */
#define MATRIX_OK_TO_READ                                               \
  ((matrix_flag & MATRIX_THROTTLE == 0)                                 \
   && (((matrix_flag & MATRIX_READ_STARTED != 0)                        \
        && (matrix_flag & MATRIX_READ_FINISHED == 0))                   \
       || (matrix_flag & MATRIX_COPY_FINISHED != 0)))

/*
 * It is ok to copy if matrix read started & finished but copy is not finished
 *
 */
#define MATRIX_OK_TO_COPY                                       \
  (((matrix_flag & MATRIX_READ_MASK) == MATRIX_READ_MASK)       \
   && ((matrix_flag & MATRIX_COPY_FINISHED) == 0))

/*
 * Matrix data buffer
 */
struct matrix_data_t
{
  uint8_t data[ID_MAX_MATRIX_SIZE];
  uint16_t size;                /* actual data size */
};

volatile struct matrix_data_t matrix_read_buffer;
volatile struct matrix_data_t matrix_render_buffer;

#define MATRIX_PHYSICAL_SIZE 32

#define MATRIX_IS_SHORT (matrix_render_buffer.size == MATRIX_PHYSICAL_SIZE)
#define MATRIX_IS_LONG (matrix_render_buffer.size > MATRIX_PHYSICAL_SIZE)

#endif

void hardware_init ()
{
  /* fixme */
}

void software_init ()
{
  /*assume last cycle was OK*/
  /*matrix_flag = MATRIX_READ_MASK | MATRIX_COPY_FINISHED;*/
}

void self_test ()
{
  /* fixme*/
}

void say_hello ()
{
  /* fixme */
}

#if 0
void copy_matrix ()
{
  uint8_t left = 0;
  uint8_t right = 0;

  if (matrix_read_buffer.size < MATRIX_PHYSICAL_SIZE) {
    uint8_t delta = MATRIX_PHYSICAL_SIZE - matrix_read_buffer.size;
    left = delta / 2;
    right = delta - left;
  }

  uint8_t i = 0;
  for (i = 0; i < left; ++i) {
    matrix_render_buffer.data[i] = 0;
  }
  for (i = 0; i < matrix_read_buffer.size; ++i) {
    matrix_render_buffer.data[i + left] = matrix_read_buffer.data[i];
  }
  for (i = 0; i < right; ++i) {
    matrix_render_buffer.data[left + matrix_read_buffer.size + i] = 0;
  }

  matrix_flag |= MATRIX_COPY_FINISHED;
}
#endif

void check_button ()
{
  /* fixme */
}

int main ()
{
  hardware_init ();
  software_init ();
  self_test ();
  say_hello ();

  while (1) {
    /*
     * uart read/write & spi write uses interrupts,
     * so we only need to parse uart read queue 
     * and check buttons
     */
    parse ();
    check_button ();
  }

  return 0;
}
