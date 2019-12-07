/*
 *
 */

#include <stdio.h>

#include "buffer.h"

// can hold 3 symbols
/* #define TEST_BUF_SIZE 17 */

/* volatile data_t buf[TEST_BUF_SIZE]; */

int main (int argc, char **argv)
{
  volatile buffer_t buff;
  buffer_init (&buff);

  printf ("%i\n", buffer_is_fillable (&buff, 100));
  printf ("%i\n", buffer_is_fillable (&buff, 200));

  printf ("fill-status: %i\n", buffer_byte_fill (&buff, 77));
  uint8_t sym = 0;
  printf ("drain-status: %i\n", buffer_byte_drain (&buff, &sym));
  printf ("drain-symbol: %i\n", sym);

  sym = 0;
  printf ("fill-status: %i\n", buffer_byte_fill (&buff, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", buffer_byte_fill (&buff, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", buffer_byte_fill (&buff, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", buffer_byte_fill (&buff, ++sym));
  printf ("fill-symbol: %i\n", sym);

  printf ("drain-status: %i\n", buffer_byte_drain (&buff, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", buffer_byte_drain (&buff, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", buffer_byte_drain (&buff, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", buffer_byte_drain (&buff, &sym));
  printf ("drain-symbol: %i\n", sym);


  printf ("fill-status: %i\n", buffer_byte_fill (&buff, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", buffer_byte_fill (&buff, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", buffer_byte_fill (&buff, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", buffer_byte_fill (&buff, ++sym));
  printf ("fill-symbol: %i\n", sym);

#define ARR_SIZE 3
  uint8_t a[ARR_SIZE];
  printf ("drain-array-status: %i\n", buffer_array_drain (&buff, a, ARR_SIZE));
  for (int i = 0; i < ARR_SIZE; ++i)
    printf ("drain-symbol-from-array: %i\n", a[i]);
    
  uint8_t b[] = {0, 200, 255, 17, 47};
  uint8_t c[7];
  printf ("fill-array-status: %i\n", buffer_array_fill (&buff, b, 5));
  printf ("drain-array-status: %i\n", buffer_array_drain (&buff, c, 6));
  for (int i = 0; i < 6; ++i)
    printf ("drain-symbol-from-array: %i\n", c[i]);

  printf ("drain-status: %i\n", buffer_byte_drain (&buff, &sym));
  printf ("drain-symbol: %i\n", sym);

  buffer_clear (&buff);

  uint8_t d[] = {1, 5, 7, 9, 11, 24, 56, 77, 89, 137};
  printf ("fill-array-status: %i\n",
          buffer_array_fill (&buff, d, sizeof (d) / sizeof (uint8_t)));
  printf ("---------\n");
  for (uint8_t i = 0; i < 11; ++i) {
    printf ("get-status: %i\n", buffer_byte_get (&buff, i, &sym));
    printf ("get-symbol: %i\n", sym);
  }
  printf ("---------\n");
  
  return 0;
}
