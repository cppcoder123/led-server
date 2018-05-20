//
// LED matrix communication code (fixme)
//
#ifndef HT1632C_H
#define HT1632C_H

void ht1632c_start ();
void ht1632c_stop ();

void ht1632c_brightness (uint8_t brightness);

void ht1632c_data (uint8_t column, uint8_t left_data, uint8_t right_data);

#endif
