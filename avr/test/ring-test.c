/*
 *
 */

#include <stdio.h>

#include "queue.h"

// can hold 3 symbols
#define TEST_BUF_SIZE 17

volatile uint8_t buf[TEST_BUF_SIZE];

int main (int argc, char **argv)
{
  queue_init (buf, TEST_BUF_SIZE);

  printf ("%i\n", queue_is_fillable (buf, 100));
  printf ("%i\n", queue_is_fillable (buf, TEST_BUF_SIZE));

  printf ("fill-status: %i\n", queue_symbol_fill (buf, 77));
  uint8_t sym = 0;
  printf ("drain-status: %i\n", queue_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);

  sym = 0;
  printf ("fill-status: %i\n", queue_symbol_fill (buf, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", queue_symbol_fill (buf, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", queue_symbol_fill (buf, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", queue_symbol_fill (buf, ++sym));
  printf ("fill-symbol: %i\n", sym);

  printf ("drain-status: %i\n", queue_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", queue_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", queue_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", queue_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);


  printf ("fill-status: %i\n", queue_symbol_fill (buf, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", queue_symbol_fill (buf, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", queue_symbol_fill (buf, ++sym));
  printf ("fill-symbol: %i\n", sym);
  printf ("fill-status: %i\n", queue_symbol_fill (buf, ++sym));
  printf ("fill-symbol: %i\n", sym);

#define ARR_SIZE 3
  uint8_t a[ARR_SIZE];
  printf ("drain-array-status: %i\n", queue_array_drain (buf, a, ARR_SIZE));
  for (int i = 0; i < ARR_SIZE; ++i)
    printf ("drain-symbol-from-array: %i\n", a[i]);
    
  uint8_t b[] = {0, 200, 255, 17, 47};
  uint8_t f[] = {22, 33, 44, 55, 66};
  uint8_t c[7];
  printf ("fill-array-status: %i\n", queue_array_fill (buf, b, 5));
  printf ("fill-array-status: %i\n", queue_array_fill (buf, f, 5));
  printf ("A drain-array-status: %i\n", queue_array_drain (buf, c, 5));
  for (int i = 0; i < 5; ++i)
    printf ("drain-symbol-from-array: %i\n", c[i]);
  printf ("B drain-array-status: %i\n", queue_array_drain (buf, c, 5));
  for (int i = 0; i < 5; ++i)
    printf ("drain-symbol-from-array: %i\n", c[i]);

  printf ("drain-status: %i\n", queue_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);

  queue_clear (buf);

  uint8_t d[] = {1, 5, 7, 9, 11, 24, 56, 77, 89, 137};
  printf ("fill-array-status: %i\n",
          queue_array_fill (buf, d, sizeof (d) / sizeof (uint8_t)));
  printf ("---------\n");
  for (uint8_t i = 0; i < 11; ++i) {
    printf ("get-status: %i\n", queue_symbol_get (buf, i, &sym));
    printf ("get-symbol: %i\n", sym);
  }
  printf ("---------\n");
  
  return 0;
}
