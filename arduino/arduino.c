/*
 *
 *
 *
 *
 *
 */

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
