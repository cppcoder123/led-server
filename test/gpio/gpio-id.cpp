/**
 *
 */

#include <unistd.h>

#include <iostream>

#include <gpiod.h>

int main (int argc, char **argv)
{
  auto gpio_14 = 14;
  auto chip_name = "gpiochip0";
  auto consumer = "gpio-test";

  auto chip = gpiod_chip_open_by_name (chip_name);
  if (chip == NULL) {
    std::cout << "Failed to open the chip\n";
    return 1;
  }

  auto gpio_14_handle = gpiod_chip_get_line (chip, gpio_14);
  if (gpio_14_handle == NULL) {
    std::cout << "Failed to get line 14\n";
    return 2;
  }

  // if (gpiod_line_direction (gpio_14_handle, GPIOD_LINE_DIRECTION_OUTPUT) != 0) {
  //   std::cout << "Failed to set line 14 direction\n";
  //   return 3;
  // }

  if (gpiod_line_request_output (gpio_14_handle, consumer, 0) != 0) {
    std::cout << "Failed to request output line 14 to 0\n";
    return 3;
  }
  sleep (5);
  
  while (true) {
    if (gpiod_line_set_value (gpio_14_handle, 1) != 0) {
      std::cout << "Failed to set output line 14 to 1\n";
      return 3;
    }
    sleep (5);
    if (gpiod_line_set_value (gpio_14_handle, 0) != 0) {
      std::cout << "Failed to set output line 14 to 0\n";
      return 4;
    }
    sleep (5);
  }
  
  return 0;
}
