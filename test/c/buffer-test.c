//
//
//
#include <iostream>

#include "buffer.h"

int main (int argc, char **argv)
{
  volatile buffer_t buff;
  buffer_init (&buff);

  std::cout << "empty buf size: " << (int) buffer_size (&buff) << "\n";

  for (int i = 0; i < 260; ++i)
    std::cout << "fill status: " << (int) buffer_byte_fill (&buff, i) << "\n";

  for (int i = 0; i < 257; ++i) {
    uint8_t byte = 0;
    std::cout << "drain status: "
              << (int) buffer_byte_drain (&buff, &byte) << "\n";
    std::cout << "drain value: " << (int) byte << "\n";
  }

  std::cout << "empty buf size: " << (int) buffer_size (&buff) << "\n";

  constexpr auto arr_size = 10;
  uint8_t src[arr_size];
  for (int i = 0; i < arr_size; ++i)
    src[i] = i;

  for (int i = 0; i < arr_size; ++i)
    std::cout << "src array value: "
              << (int) i << " - " << (int) src[i] << "\n";

  std::cout << "array fill status: "
            << (int) buffer_array_fill (&buff, src, arr_size) << "\n";

  uint8_t dst[arr_size];
  std::cout << "array drain status: "
            << (int) buffer_array_drain (&buff, dst, arr_size) << "\n";

  for (int i = 0; i < arr_size; ++i)
    std::cout << "dst array value: "
              << (int) i << " - " << (int) dst[i] << "\n";

  for (int i = 0; i < 5; ++i)
    std::cout << "fill status: " << (int) buffer_byte_fill (&buff, i) << "\n";

  for (int i = 0; i < 3; ++i) {
    uint8_t byte = 0;
    std::cout << "drain status: "
              << (int) buffer_byte_drain (&buff, &byte) << "\n";
    std::cout << "drain value: " << (int) byte << "\n";
  }

  for (int i = 0; i < 3; ++i)
    std::cout << "fill status: "
              << (int) buffer_byte_fill (&buff, i + 10) << "\n";

  for (int i = 0; i < 10; ++i) {
    uint8_t byte = 0;
    std::cout << "drain status: "
              << (int) buffer_byte_drain (&buff, &byte) << "\n";
    std::cout << "drain value: " << (int) byte << "\n";
  }

  std::cout << "aaa\n";
  return 0;
}
