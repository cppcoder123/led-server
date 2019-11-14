/*
 *
 */

#include <stdio.h>

#include "ring.h"

// can hold 3 symbols
#define TEST_BUF_SIZE 17

volatile data_t buf[TEST_BUF_SIZE];

int main (int argc, char **argv)
{
  ring_init (buf, TEST_BUF_SIZE);

  printf ("fill-status: %i\n", ring_symbol_fill (buf, 77));
  printf ("fill-status: %i\n", ring_symbol_fill (buf, 66));
  printf ("fill-status: %i\n", ring_symbol_fill (buf, 55));
  printf ("fill-status: %i\n", ring_symbol_fill (buf, 44));
  printf ("fill-status: %i\n", ring_symbol_fill (buf, 33));

  
  uint8_t sym = 0;
  printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);
  printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym));
  printf ("drain-symbol: %i\n", sym);

  uint8_t data[10];
  printf ("array-drain %i\n", ring_array_drain (buf, data, 1));
  printf ("array-symbol %i\n", data[0]);
  
  /* printf ("fill-status: %i\n", ring_symbol_fill (buf, 88)); */

  /* printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym)); */
  /* printf ("drain-symbol: %i\n", sym); */

  /* printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym)); */
  /* printf ("drain-symbol: %i\n", sym); */
  
  return 0;
}

/*   printf ("%i\n", ring_is_fillable (buf, 100)); */
/*   printf ("%i\n", ring_is_fillable (buf, TEST_BUF_SIZE)); */

/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, 77)); */
/*   uint8_t sym = 0; */
/*   printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym)); */
/*   printf ("drain-symbol: %i\n", sym); */

/*   sym = 0; */
/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, ++sym)); */
/*   printf ("fill-symbol: %i\n", sym); */
/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, ++sym)); */
/*   printf ("fill-symbol: %i\n", sym); */
/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, ++sym)); */
/*   printf ("fill-symbol: %i\n", sym); */
/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, ++sym)); */
/*   printf ("fill-symbol: %i\n", sym); */

/*   printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym)); */
/*   printf ("drain-symbol: %i\n", sym); */
/*   printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym)); */
/*   printf ("drain-symbol: %i\n", sym); */
/*   printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym)); */
/*   printf ("drain-symbol: %i\n", sym); */
/*   printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym)); */
/*   printf ("drain-symbol: %i\n", sym); */


/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, ++sym)); */
/*   printf ("fill-symbol: %i\n", sym); */
/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, ++sym)); */
/*   printf ("fill-symbol: %i\n", sym); */
/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, ++sym)); */
/*   printf ("fill-symbol: %i\n", sym); */
/*   printf ("fill-status: %i\n", ring_symbol_fill (buf, ++sym)); */
/*   printf ("fill-symbol: %i\n", sym); */

/* #define ARR_SIZE 3 */
/*   data_t a[ARR_SIZE]; */
/*   printf ("drain-array-status: %i\n", ring_array_drain (buf, a, ARR_SIZE)); */
/*   for (int i = 0; i < ARR_SIZE; ++i) */
/*     printf ("drain-symbol-from-array: %i\n", a[i]); */
    
/*   data_t b[] = {0, 200, 255, 17, 47}; */
/*   data_t c[7]; */
/*   printf ("fill-array-status: %i\n", ring_array_fill (buf, b, 5)); */
/*   printf ("drain-array-status: %i\n", ring_array_drain (buf, c, 6)); */
/*   for (int i = 0; i < 6; ++i) */
/*     printf ("drain-symbol-from-array: %i\n", c[i]); */

/*   printf ("drain-status: %i\n", ring_symbol_drain (buf, &sym)); */
/*   printf ("drain-symbol: %i\n", sym); */

/*   ring_clear (buf); */

/*   data_t d[] = {1, 5, 7, 9, 11, 24, 56, 77, 89, 137}; */
/*   printf ("fill-array-status: %i\n", */
/*           ring_array_fill (buf, d, sizeof (d) / sizeof (data_t))); */
/*   printf ("---------\n"); */
/*   for (uint8_t i = 0; i < 11; ++i) { */
/*     printf ("get-status: %i\n", ring_symbol_get (buf, i, &sym)); */
/*     printf ("get-symbol: %i\n", sym); */
/*   } */
/*   printf ("---------\n"); */
  
/*   return 0; */
/* } */
