# 1 "arduino.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "arduino.c"
# 9 "arduino.c"
# 1 "/opt/lfs82/avr/lib/gcc/avr/8.1.0/include/stdint.h" 1 3 4
# 9 "/opt/lfs82/avr/lib/gcc/avr/8.1.0/include/stdint.h" 3 4
# 1 "/opt/lfs82/avr/avr/include/stdint.h" 1 3 4
# 125 "/opt/lfs82/avr/avr/include/stdint.h" 3 4

# 125 "/opt/lfs82/avr/avr/include/stdint.h" 3 4
typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef signed int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
# 146 "/opt/lfs82/avr/avr/include/stdint.h" 3 4
typedef int16_t intptr_t;




typedef uint16_t uintptr_t;
# 163 "/opt/lfs82/avr/avr/include/stdint.h" 3 4
typedef int8_t int_least8_t;




typedef uint8_t uint_least8_t;




typedef int16_t int_least16_t;




typedef uint16_t uint_least16_t;




typedef int32_t int_least32_t;




typedef uint32_t uint_least32_t;







typedef int64_t int_least64_t;






typedef uint64_t uint_least64_t;
# 217 "/opt/lfs82/avr/avr/include/stdint.h" 3 4
typedef int8_t int_fast8_t;




typedef uint8_t uint_fast8_t;




typedef int16_t int_fast16_t;




typedef uint16_t uint_fast16_t;




typedef int32_t int_fast32_t;




typedef uint32_t uint_fast32_t;







typedef int64_t int_fast64_t;






typedef uint64_t uint_fast64_t;
# 277 "/opt/lfs82/avr/avr/include/stdint.h" 3 4
typedef int64_t intmax_t;




typedef uint64_t uintmax_t;
# 10 "/opt/lfs82/avr/lib/gcc/avr/8.1.0/include/stdint.h" 2 3 4
# 10 "arduino.c" 2

# 1 "../lib/core/device-id.h" 1
# 26 "../lib/core/device-id.h"

# 26 "../lib/core/device-id.h"
enum {
  ID_MAX_MATRIX_SIZE = 800
};


enum {
  ID_EYE_CATCH = 254,


};


enum {
  ID_STATUS = 1,
  ID_BUTTON,
  ID_INIT,
  ID_UNINIT,
  ID_HANDSHAKE,
  ID_MATRIX,
  ID_SHIFT_DELAY,
  ID_BRIGHTNESS
};
# 60 "../lib/core/device-id.h"
enum {
  ID_STATUS_RESERVED = 6,
  ID_STATUS_OK,
  ID_STATUS_NO_FINISH,
  ID_STATUS_ID_UNKNOWN,
  ID_STATUS_LONG_ID_UNKNOWN,
  ID_STATUS_PARSE_ERROR,
  ID_STATUS_NO_ARRAY_START,
  ID_STATUS_NO_ARRAY_FINISH,
  ID_STATUS_TOO_LONG_MATRIX,
  ID_STATUS_UNKNOWN_DELAY_ID,
  ID_STATUS_BRIGHTNESS_OUT_OF_RANGE,

  MSG_ID_STATUS_MAX
};

enum {
  ID_BRIGHTNESS_MIN,
  ID_BRIGHTNESS_0 = ID_BRIGHTNESS_MIN,
  ID_BRIGHTNESS_1,
  ID_BRIGHTNESS_2,
  ID_BRIGHTNESS_3,
  ID_BRIGHTNESS_4,
  ID_BRIGHTNESS_5,
  ID_BRIGHTNESS_6,
  ID_BRIGHTNESS_7,
  ID_BRIGHTNESS_8,
  ID_BRIGHTNESS_9,
  ID_BRIGHTNESS_10,
  ID_BRIGHTNESS_11,
  ID_BRIGHTNESS_12,
  ID_BRIGHTNESS_13,
  ID_BRIGHTNESS_14,
  ID_BRIGHTNESS_15,
  ID_BRIGHTNESS_MAX = ID_BRIGHTNESS_15,
};
# 12 "arduino.c" 2




# 1 "buffer.h" 1
# 17 "buffer.h"
struct uart_read_buffer
{
  uint8_t data[800];
  uint16_t size;
  uint16_t current;
};

inline void buffer_init (uart_read_buffer *buffer)
{
  buffer->size = 0;
  buffer->current = 0;
}
# 17 "arduino.c" 2


enum {
  MATRIX_READ_STARTED = (1 << 0),
  MATRIX_READ_FINISHED = (1 << 1),
  MATRIX_READ_MASK = (MATRIX_READ_STARTED | MATRIX_READ_FINISHED),

  MATRIX_COPY_FINISHED = (1 << 2),


  MATRIX_THROTTLE = (1 << 4)
};

volatile uint8_t matrix_flag = 0;
# 53 "arduino.c"
struct matrix_data_t
{
  uint8_t data[ID_MAX_MATRIX_SIZE];
  uint16_t size;
};

volatile struct matrix_data_t matrix_read_buffer;
volatile struct matrix_data_t matrix_render_buffer;






void hardware_init ()
{

}

void software_init ()
{

  matrix_flag = MATRIX_READ_MASK | MATRIX_COPY_FINISHED;
}

void hw_sw_test ()
{

}

void handle_io ()
{

}

void copy_matrix ()
{
  uint8_t left = 0;
  uint8_t right = 0;

  if (matrix_read_buffer.size < 32) {
    uint8_t delta = 32 - matrix_read_buffer.size;
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

void render_stable ()
{

}

void render_rolling ()
{

}

void handle_button ()
{

}

int main ()
{
  hardware_init ();
  software_init ();
  hw_sw_test ();

  while (1) {
    handle_button ();
    handle_io ();
    if ((((matrix_flag & MATRIX_READ_MASK) == MATRIX_READ_MASK) && ((matrix_flag & MATRIX_COPY_FINISHED) == 0)))
      copy_matrix ();
    if ((matrix_render_buffer.size == 32)) {
      render_stable ();
    } else if ((matrix_render_buffer.size > 32)) {
      render_rolling ();
    }
  }

  return 0;
}
