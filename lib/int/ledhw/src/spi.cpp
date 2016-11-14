//
//
//

#include <chrono>
#include <limits>
#include <stdexcept>
#include <thread>
#include <vector>

#include "libled/log.hpp"
#include "libled/spi-message.h"

#include "ledhw/spi.hpp"
#include "spi-io.hpp"

namespace ledhw
{
  typedef libled::log_t log_t;

  static spi_io m_io ("/dev/spi-dev-name-fixme");
  
  spi_t::spi_t ()
  {
    // fixme
  }

  bool spi_t::start ()
  {
    try {
      m_io.start ();
      //
      send ({SPI_MESSAGE_HANDSHAKE});
      send ({SPI_MESSAGE_DELAY, SPI_DELAY_SCROLL_SHIFT, 100}); // fixme ?
      send ({SPI_MESSAGE_BRIGHTNESS, SPI_BRIGHTNESS_MAX});
      send ({SPI_MESSAGE_START});
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
    try {
      send ({SPI_MESSAGE_STOP});
      //
      m_io.stop ();
    }
    catch (std::exception &e) {
      log_t::buffer_t buf;
      buf << "Failed to stop spi bus: \"" << e.what () << "\"";
      log_t::error (buf);
    }
  }

  bool spi_t::render (const libled::matrix_t &matrix)
  {
    if (matrix.size () > SPI_MATRIX_CAPACITY)
      return false;
    
    //
    // <msg-id><size-lsb><size-msb><array-start><data-0><data-1>...<array-finish>
    //
    vector_t msg;

    msg.push_back (SPI_MESSAGE_MATRIX);
    msg.push_back (matrix.size () % std::numeric_limits<uchar_t>::max ());
    msg.push_back (matrix.size () / std::numeric_limits<uchar_t>::max ());
    msg.push_back (SPI_MATRIX_ARRAY_START);
    for (std::size_t i = 0; i < matrix.size (); ++i)
      msg.push_back (get_char (matrix.get_column (i)));
    msg.push_back (SPI_MATRIX_ARRAY_FINISH);
    
    return status_send (msg);
  }

  bool spi_t::brightness (int level)
  {
    if ((level < SPI_BRIGHTNESS_MIN)
        || (level > SPI_BRIGHTNESS_MAX))
      return false;

    // <msg-id><brightness-level>
    return status_send ({SPI_MESSAGE_BRIGHTNESS, static_cast<uchar_t>(level)});
  }

  spi_t::uchar_t spi_t::get_char (const column_t &column)
  {
    static const std::size_t bits_per_column = 8;

    uchar_t res = 0, mask = 1;
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
    result[SPI_MESSAGE_MATRIX] = "Matrix";
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

    try {
      write (msg);
      // delay reading to let mcu handle the message
      std::this_thread::sleep_for (std::chrono::milliseconds (1)); // fixme ? 
      read ();
    }
    catch (std::exception &e) {
      // add message name to exception
      const std::string name (name_list[msg[0]]);
      log_t::buffer_t buf;
      buf << name << ": " << e.what ();
      throw std::domain_error (buf.str ());
    }
  }

  void spi_t::write (const vector_t &msg)
  {
    m_io.write (msg);
  }

  void spi_t::read ()
  {
    uchar_t info = m_io.read ();
    if (info != SPI_STATUS_OK)
      throw std::logic_error (get_error (info));
  }

  std::string spi_t::get_error (uchar_t err_id)
  {
    static const name_list_t error_list (get_error_list ());

    if (err_id >= error_list.size ())
      return "Unknown error during spi read";

    return error_list[err_id];
  }

  spi_t::name_list_t spi_t::get_error_list ()
  {
    name_list_t error_list (SPI_STATUS_MAX, "Unknown error");

    error_list[SPI_STATUS_OK] = "No-error";
    error_list[SPI_STATUS_MESSAGE_ID_UNKNOWN] = "Message id is unknown";
    error_list[SPI_STATUS_LONG_MESSAGE_ID_UNKNOWN] = "Long message id is unknown";
    error_list[SPI_STATUS_PARSE_ERROR] = "Parse error";
    error_list[SPI_STATUS_NO_ARRAY_START] = "There is no array start";
    error_list[SPI_STATUS_NO_ARRAY_FINISH] = "There is no array finish";
    error_list[SPI_STATUS_TOO_LONG_MATRIX] = "Matrix is too long";
    error_list[SPI_STATUS_UNKNOWN_DELAY_ID] = "Delay id is unknown";
    error_list[SPI_STATUS_BRIGHTNESS_OUT_OF_RANGE] = "Brightness is out of range";

    return error_list;
  }
  
} // namespace ledhw
