# 1 "ht1632c.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "ht1632c.c"






# 1 "frequency.h" 1
# 8 "ht1632c.c" 2



# 1 "/opt/lfs78/avr/avr/include/avr/io.h" 1 3
# 99 "/opt/lfs78/avr/avr/include/avr/io.h" 3
# 1 "/opt/lfs78/avr/avr/include/avr/sfr_defs.h" 1 3
# 126 "/opt/lfs78/avr/avr/include/avr/sfr_defs.h" 3
# 1 "/opt/lfs78/avr/avr/include/inttypes.h" 1 3
# 37 "/opt/lfs78/avr/avr/include/inttypes.h" 3
# 1 "/opt/lfs78/avr/lib/gcc/avr/5.4.0/include/stdint.h" 1 3 4
# 9 "/opt/lfs78/avr/lib/gcc/avr/5.4.0/include/stdint.h" 3 4
# 1 "/opt/lfs78/avr/avr/include/stdint.h" 1 3 4
# 125 "/opt/lfs78/avr/avr/include/stdint.h" 3 4

# 125 "/opt/lfs78/avr/avr/include/stdint.h" 3 4
typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef signed int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
# 146 "/opt/lfs78/avr/avr/include/stdint.h" 3 4
typedef int16_t intptr_t;




typedef uint16_t uintptr_t;
# 163 "/opt/lfs78/avr/avr/include/stdint.h" 3 4
typedef int8_t int_least8_t;




typedef uint8_t uint_least8_t;




typedef int16_t int_least16_t;




typedef uint16_t uint_least16_t;




typedef int32_t int_least32_t;




typedef uint32_t uint_least32_t;







typedef int64_t int_least64_t;






typedef uint64_t uint_least64_t;
# 217 "/opt/lfs78/avr/avr/include/stdint.h" 3 4
typedef int8_t int_fast8_t;




typedef uint8_t uint_fast8_t;




typedef int16_t int_fast16_t;




typedef uint16_t uint_fast16_t;




typedef int32_t int_fast32_t;




typedef uint32_t uint_fast32_t;







typedef int64_t int_fast64_t;






typedef uint64_t uint_fast64_t;
# 277 "/opt/lfs78/avr/avr/include/stdint.h" 3 4
typedef int64_t intmax_t;




typedef uint64_t uintmax_t;
# 10 "/opt/lfs78/avr/lib/gcc/avr/5.4.0/include/stdint.h" 2 3 4
# 38 "/opt/lfs78/avr/avr/include/inttypes.h" 2 3
# 77 "/opt/lfs78/avr/avr/include/inttypes.h" 3
typedef int32_t int_farptr_t;



typedef uint32_t uint_farptr_t;
# 127 "/opt/lfs78/avr/avr/include/avr/sfr_defs.h" 2 3
# 100 "/opt/lfs78/avr/avr/include/avr/io.h" 2 3
# 318 "/opt/lfs78/avr/avr/include/avr/io.h" 3
# 1 "/opt/lfs78/avr/avr/include/avr/iom8a.h" 1 3
# 319 "/opt/lfs78/avr/avr/include/avr/io.h" 2 3
# 585 "/opt/lfs78/avr/avr/include/avr/io.h" 3
# 1 "/opt/lfs78/avr/avr/include/avr/portpins.h" 1 3
# 586 "/opt/lfs78/avr/avr/include/avr/io.h" 2 3

# 1 "/opt/lfs78/avr/avr/include/avr/common.h" 1 3
# 588 "/opt/lfs78/avr/avr/include/avr/io.h" 2 3

# 1 "/opt/lfs78/avr/avr/include/avr/version.h" 1 3
# 590 "/opt/lfs78/avr/avr/include/avr/io.h" 2 3






# 1 "/opt/lfs78/avr/avr/include/avr/fuse.h" 1 3
# 248 "/opt/lfs78/avr/avr/include/avr/fuse.h" 3
typedef struct
{
    unsigned char low;
    unsigned char high;
} __fuse_t;
# 597 "/opt/lfs78/avr/avr/include/avr/io.h" 2 3


# 1 "/opt/lfs78/avr/avr/include/avr/lock.h" 1 3
# 600 "/opt/lfs78/avr/avr/include/avr/io.h" 2 3
# 12 "ht1632c.c" 2
# 1 "/opt/lfs78/avr/avr/include/util/delay.h" 1 3
# 45 "/opt/lfs78/avr/avr/include/util/delay.h" 3
# 1 "/opt/lfs78/avr/avr/include/util/delay_basic.h" 1 3
# 40 "/opt/lfs78/avr/avr/include/util/delay_basic.h" 3
static __inline__ void _delay_loop_1(uint8_t __count) __attribute__((__always_inline__));
static __inline__ void _delay_loop_2(uint16_t __count) __attribute__((__always_inline__));
# 80 "/opt/lfs78/avr/avr/include/util/delay_basic.h" 3
void
_delay_loop_1(uint8_t __count)
{
 __asm__ volatile (
  "1: dec %0" "\n\t"
  "brne 1b"
  : "=r" (__count)
  : "0" (__count)
 );
}
# 102 "/opt/lfs78/avr/avr/include/util/delay_basic.h" 3
void
_delay_loop_2(uint16_t __count)
{
 __asm__ volatile (
  "1: sbiw %0,1" "\n\t"
  "brne 1b"
  : "=w" (__count)
  : "0" (__count)
 );
}
# 46 "/opt/lfs78/avr/avr/include/util/delay.h" 2 3
# 1 "/opt/lfs78/avr/avr/include/math.h" 1 3
# 127 "/opt/lfs78/avr/avr/include/math.h" 3
extern double cos(double __x) __attribute__((__const__));





extern double sin(double __x) __attribute__((__const__));





extern double tan(double __x) __attribute__((__const__));






extern double fabs(double __x) __attribute__((__const__));






extern double fmod(double __x, double __y) __attribute__((__const__));
# 168 "/opt/lfs78/avr/avr/include/math.h" 3
extern double modf(double __x, double *__iptr);


extern float modff (float __x, float *__iptr);




extern double sqrt(double __x) __attribute__((__const__));


extern float sqrtf (float) __attribute__((__const__));




extern double cbrt(double __x) __attribute__((__const__));
# 195 "/opt/lfs78/avr/avr/include/math.h" 3
extern double hypot (double __x, double __y) __attribute__((__const__));







extern double square(double __x) __attribute__((__const__));






extern double floor(double __x) __attribute__((__const__));






extern double ceil(double __x) __attribute__((__const__));
# 235 "/opt/lfs78/avr/avr/include/math.h" 3
extern double frexp(double __x, int *__pexp);







extern double ldexp(double __x, int __exp) __attribute__((__const__));





extern double exp(double __x) __attribute__((__const__));





extern double cosh(double __x) __attribute__((__const__));





extern double sinh(double __x) __attribute__((__const__));





extern double tanh(double __x) __attribute__((__const__));







extern double acos(double __x) __attribute__((__const__));







extern double asin(double __x) __attribute__((__const__));






extern double atan(double __x) __attribute__((__const__));
# 299 "/opt/lfs78/avr/avr/include/math.h" 3
extern double atan2(double __y, double __x) __attribute__((__const__));





extern double log(double __x) __attribute__((__const__));





extern double log10(double __x) __attribute__((__const__));





extern double pow(double __x, double __y) __attribute__((__const__));






extern int isnan(double __x) __attribute__((__const__));
# 334 "/opt/lfs78/avr/avr/include/math.h" 3
extern int isinf(double __x) __attribute__((__const__));






__attribute__((__const__)) static inline int isfinite (double __x)
{
    unsigned char __exp;
    __asm__ (
 "mov	%0, %C1		\n\t"
 "lsl	%0		\n\t"
 "mov	%0, %D1		\n\t"
 "rol	%0		"
 : "=r" (__exp)
 : "r" (__x) );
    return __exp != 0xff;
}






__attribute__((__const__)) static inline double copysign (double __x, double __y)
{
    __asm__ (
 "bst	%D2, 7	\n\t"
 "bld	%D0, 7	"
 : "=r" (__x)
 : "0" (__x), "r" (__y) );
    return __x;
}
# 377 "/opt/lfs78/avr/avr/include/math.h" 3
extern int signbit (double __x) __attribute__((__const__));






extern double fdim (double __x, double __y) __attribute__((__const__));
# 393 "/opt/lfs78/avr/avr/include/math.h" 3
extern double fma (double __x, double __y, double __z) __attribute__((__const__));







extern double fmax (double __x, double __y) __attribute__((__const__));







extern double fmin (double __x, double __y) __attribute__((__const__));






extern double trunc (double __x) __attribute__((__const__));
# 427 "/opt/lfs78/avr/avr/include/math.h" 3
extern double round (double __x) __attribute__((__const__));
# 440 "/opt/lfs78/avr/avr/include/math.h" 3
extern long lround (double __x) __attribute__((__const__));
# 454 "/opt/lfs78/avr/avr/include/math.h" 3
extern long lrint (double __x) __attribute__((__const__));
# 47 "/opt/lfs78/avr/avr/include/util/delay.h" 2 3
# 86 "/opt/lfs78/avr/avr/include/util/delay.h" 3
static __inline__ void _delay_us(double __us) __attribute__((__always_inline__));
static __inline__ void _delay_ms(double __ms) __attribute__((__always_inline__));
# 165 "/opt/lfs78/avr/avr/include/util/delay.h" 3
void
_delay_ms(double __ms)
{
 double __tmp ;



 uint32_t __ticks_dc;
 extern void __builtin_avr_delay_cycles(unsigned long);
 __tmp = ((
# 174 "/opt/lfs78/avr/avr/include/util/delay.h"
          16000000UL
# 174 "/opt/lfs78/avr/avr/include/util/delay.h" 3
               ) / 1e3) * __ms;
# 184 "/opt/lfs78/avr/avr/include/util/delay.h" 3
  __ticks_dc = (uint32_t)(ceil(fabs(__tmp)));


 __builtin_avr_delay_cycles(__ticks_dc);
# 210 "/opt/lfs78/avr/avr/include/util/delay.h" 3
}
# 254 "/opt/lfs78/avr/avr/include/util/delay.h" 3
void
_delay_us(double __us)
{
 double __tmp ;



 uint32_t __ticks_dc;
 extern void __builtin_avr_delay_cycles(unsigned long);
 __tmp = ((
# 263 "/opt/lfs78/avr/avr/include/util/delay.h"
          16000000UL
# 263 "/opt/lfs78/avr/avr/include/util/delay.h" 3
               ) / 1e6) * __us;
# 273 "/opt/lfs78/avr/avr/include/util/delay.h" 3
  __ticks_dc = (uint32_t)(ceil(fabs(__tmp)));


 __builtin_avr_delay_cycles(__ticks_dc);
# 299 "/opt/lfs78/avr/avr/include/util/delay.h" 3
}
# 13 "ht1632c.c" 2

# 1 "ht1632c.h" 1
# 22 "ht1632c.h"

# 22 "ht1632c.h"
void ht1632c_start ();
# 15 "ht1632c.c" 2
# 102 "ht1632c.c"
void ht1632c_start ()
{
  
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 |= (1 << 
# 104 "ht1632c.c" 3
 0
# 104 "ht1632c.c"
 ); _delay_us (0.1); 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 &= ~(1 << 
# 104 "ht1632c.c" 3
 0
# 104 "ht1632c.c"
 ); _delay_us (0.1); { uint8_t bit_i = 3 - 1; do { uint8_t mask_i = 1 << bit_i; 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 &= ~(1 << 
# 104 "ht1632c.c" 3
 1
# 104 "ht1632c.c"
 ); _delay_us (0.1); if (0b100 & mask_i) 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 |= (1 << 
# 104 "ht1632c.c" 3
 2
# 104 "ht1632c.c"
 ); else 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 &= ~(1 << 
# 104 "ht1632c.c" 3
 2
# 104 "ht1632c.c"
 ); _delay_us (0.1); 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 |= (1 << 
# 104 "ht1632c.c" 3
 1
# 104 "ht1632c.c"
 ); _delay_us (0.1); } while (bit_i-- > 0); { uint8_t bit_i = 8 - 1; do { uint8_t mask_i = 1 << bit_i; 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 &= ~(1 << 
# 104 "ht1632c.c" 3
 1
# 104 "ht1632c.c"
 ); _delay_us (0.1); if (0x0 & mask_i) 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 |= (1 << 
# 104 "ht1632c.c" 3
 2
# 104 "ht1632c.c"
 ); else 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 &= ~(1 << 
# 104 "ht1632c.c" 3
 2
# 104 "ht1632c.c"
 ); _delay_us (0.1); 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 |= (1 << 
# 104 "ht1632c.c" 3
 1
# 104 "ht1632c.c"
 ); _delay_us (0.1); } while (bit_i-- > 0); { uint8_t bit_i = 1 - 1; do { uint8_t mask_i = 1 << bit_i; 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 &= ~(1 << 
# 104 "ht1632c.c" 3
 1
# 104 "ht1632c.c"
 ); _delay_us (0.1); if (0 & mask_i) 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 |= (1 << 
# 104 "ht1632c.c" 3
 2
# 104 "ht1632c.c"
 ); else 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 &= ~(1 << 
# 104 "ht1632c.c" 3
 2
# 104 "ht1632c.c"
 ); _delay_us (0.1); 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 |= (1 << 
# 104 "ht1632c.c" 3
 1
# 104 "ht1632c.c"
 ); _delay_us (0.1); } while (bit_i-- > 0); 
# 104 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 104 "ht1632c.c"
 |= (1 << 
# 104 "ht1632c.c" 3
 0
# 104 "ht1632c.c"
 ); _delay_us (0.1);
  
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 |= (1 << 
# 105 "ht1632c.c" 3
 3
# 105 "ht1632c.c"
 ); _delay_us (0.1); 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 &= ~(1 << 
# 105 "ht1632c.c" 3
 3
# 105 "ht1632c.c"
 ); _delay_us (0.1); { uint8_t bit_i = 3 - 1; do { uint8_t mask_i = 1 << bit_i; 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 &= ~(1 << 
# 105 "ht1632c.c" 3
 4
# 105 "ht1632c.c"
 ); _delay_us (0.1); if (0b100 & mask_i) 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 |= (1 << 
# 105 "ht1632c.c" 3
 5
# 105 "ht1632c.c"
 ); else 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 &= ~(1 << 
# 105 "ht1632c.c" 3
 5
# 105 "ht1632c.c"
 ); _delay_us (0.1); 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 |= (1 << 
# 105 "ht1632c.c" 3
 4
# 105 "ht1632c.c"
 ); _delay_us (0.1); } while (bit_i-- > 0); { uint8_t bit_i = 8 - 1; do { uint8_t mask_i = 1 << bit_i; 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 &= ~(1 << 
# 105 "ht1632c.c" 3
 4
# 105 "ht1632c.c"
 ); _delay_us (0.1); if (0x0 & mask_i) 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 |= (1 << 
# 105 "ht1632c.c" 3
 5
# 105 "ht1632c.c"
 ); else 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 &= ~(1 << 
# 105 "ht1632c.c" 3
 5
# 105 "ht1632c.c"
 ); _delay_us (0.1); 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 |= (1 << 
# 105 "ht1632c.c" 3
 4
# 105 "ht1632c.c"
 ); _delay_us (0.1); } while (bit_i-- > 0); { uint8_t bit_i = 1 - 1; do { uint8_t mask_i = 1 << bit_i; 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 &= ~(1 << 
# 105 "ht1632c.c" 3
 4
# 105 "ht1632c.c"
 ); _delay_us (0.1); if (0 & mask_i) 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 |= (1 << 
# 105 "ht1632c.c" 3
 5
# 105 "ht1632c.c"
 ); else 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 &= ~(1 << 
# 105 "ht1632c.c" 3
 5
# 105 "ht1632c.c"
 ); _delay_us (0.1); 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 |= (1 << 
# 105 "ht1632c.c" 3
 4
# 105 "ht1632c.c"
 ); _delay_us (0.1); } while (bit_i-- > 0); 
# 105 "ht1632c.c" 3
 (*(volatile uint8_t *)((0x15) + 0x20)) 
# 105 "ht1632c.c"
 |= (1 << 
# 105 "ht1632c.c" 3
 3
# 105 "ht1632c.c"
 ); _delay_us (0.1);
}
