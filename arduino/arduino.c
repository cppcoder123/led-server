/*
 *
 *
 *
 *
 *
 */

#include <stdint.h>

#include "device-id.h"

enum {
  MATRIX_READ_STARTED = (1 << 0),
  MATRIX_READ_FINISHED = (1 << 1),
  MATRIX_READ_MASK = (MATRIX_READ_STARTED | MATRIX_READ_FINISHED),
  /**/
  MATRIX_COPY_STARTED = (1 << 2),
  MATRIX_COPY_FINISHED = (1 << 3),
  MATRIX_COPY_MASK = (MATRIX_COPY_STARTED | MATRIX_COPY_FINISHED),
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
  ((matrix_flag & MATRIX_READ_MASK == MATRIX_READ_MASK)         \
   && (matrix_flag & MATRIX_COPY_FINISHED == 0))

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

int main ()
{

  return 0;
}
