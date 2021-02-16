/*
 *
 */

#include "dac.h"
#include "twi.h"

/* fixme: define in twi.h */
#define TWI_ID_DAC 1
/* fixme: write right value*/
#define DAC_ADDRESS 0x61

static void write_callback (uint8_t event, uint8_t status)
{
  /**/
}

static void read_callback (uint8_t event, uint8_t status, uint8_t value)
{
  /**/
}

void dac_conduct ()
{
  /* fixme: fill conduct value */
  /* twi_write_byte (TWI_ID_DAC, EVENT_CONDUCT, REG, VALUE_CONDUCT); */
}

void dac_semiconduct ()
{
  /* fixme: fill semiconduct value */
}

void dac_misconduct ()
{
  /* fixme: fill misconduct value */
}

void dac_init ()
{
  twi_slave (TWI_ID_DAC, DAC_ADDRESS, &write_callback, &read_callback);

  dac_misconduct ();
}
