//
//
//

#include <limits>
#include <stdexcept>
#include <vector>

#include "libled/log.hpp"
#include "libled/spi-message.h"

#include "ledhw/spi.hpp"

namespace ledhw
{
  typedef libled::log_t log_t;

  spi_t::spi_t ()
  {
    // fixme
  }

  bool spi_t::start ()
  {
    try {
      send ({SPI_MESSAGE_HANDSHAKE});
      send ({SPI_MESSAGE_DELAY, SPI_DELAY_SCROLL_SHIFT, 100}); // fixme ?
      send ({SPI_MESSAGE_BRIGHTNESS, SPI_BRIGHTNESS_MAX});
    }

    catch (std::exception &e) {
      log_t::buffer_t buf;
      buf << "Failed to start spi bus: \"" << e.what () << "\"";
      log_t::error (buf);
      return false;
    }
    
    return true;
  }

  void spi_t::stop ()
  {
    // fixme : put mcu (and leds) in low-consumption mode
  }

  bool spi_t::render (const libled::matrix_t &matrix)
  {
    //
    // <msg-id><data-id><size-msb><size-lsb><data-0><data-1>...
    //
    vector_t msg;
    msg.reserve (4 + matrix.size ());

    msg.push_back (SPI_MESSAGE_DATA);
    msg.push_back (SPI_DATA_MATRIX);
    msg.push_back (matrix.size () / std::numeric_limits<uchar>::max ());
    msg.push_back (matrix.size () % std::numeric_limits<uchar>::max ());

    for (std::size_t i = 0; i < matrix.size (); ++i)
      msg.push_back (get_char (matrix.get_column (i)));

    return status_send (msg);
  }

  bool spi_t::brightness (int level)
  {
    if ((level < SPI_BRIGHTNESS_MIN)
        || (level > SPI_BRIGHTNESS_MAX))
      return false;

    // <msg-id><brightness-level>
    return status_send ({SPI_MESSAGE_BRIGHTNESS, static_cast<uchar>(level)});
  }

  spi_t::uchar spi_t::get_char (const column_t &column)
  {
    static const std::size_t bits_per_column = 8;

    uchar res = 0, mask = 1;
    for (std::size_t bit = 0; bit < bits_per_column; ++bit) {
      if (column.test (bit) == true)
        res |= mask;
      mask <<= 1;
    }

    return res;
  }

  spi_t::name_list_t spi_t::get_name_list ()
  {
    name_list_t result (SPI_MESSAGE_MAX, "Unknown");

    result[SPI_MESSAGE_STATUS] = "Status";
    result[SPI_MESSAGE_HANDSHAKE] = "Handshake";
    result[SPI_MESSAGE_DATA] = "Data";
    result[SPI_MESSAGE_CAPACITY] = "Capacity";
    result[SPI_MESSAGE_DELAY] = "Delay";
    result[SPI_MESSAGE_BRIGHTNESS] = "Brightness";

    return result;
  }

  bool spi_t::status_send (const vector_t &msg)
  {
    try {
      send (msg);
    }
    catch (std::exception &e) {
      log_t::error (e.what ());
      return false;
    }

    return true;
  }
  
  void spi_t::send (const vector_t &msg)
  {
    static const name_list_t name_list (get_name_list ());
    
    if (write (msg) == false) {
      const std::string name (name_list[msg[0]]);
      log_t::buffer_t buf;
      buf << name << ": Failed to write message to spi bus";
      throw std::domain_error (buf.str ());
    }

    if (read () == false) {
      const std::string name (name_list[msg[0]]);
      log_t::buffer_t buf;
      buf << name << ": Failed to read message from spi bus";
      throw std::domain_error (buf.str ());
    }
  }

  bool spi_t::write (const vector_t &msg)
  {
    // fixme
    return true;
  }

  bool spi_t::read ()
  {
    // fixme
    return true;
  }
  
} // namespace ledhw
