//
//
//

#include <limits>
#include <stdexcept>
#include <vector>

#include "libled/log.hpp"
#include "libled/spi-message.h"

#include "ledhw/spi.hpp"
#include "spi-io.hpp"

namespace ledhw
{
  typedef libled::log_t log_t;

  spi_t::spi_t (const std::string &spi_device)
    : m_io (spi_device)
  {
  }

  bool spi_t::start ()
  {
    try {
      m_io.start ();
      //
      m_io.message_start ();
      m_io.message_add ({SPI_SLAVE_MSG_HANDSHAKE});
      send (m_io.message_finish ());
      //
      m_io.message_start ();
      m_io.message_add ({SPI_SLAVE_MSG_DELAY, SPI_DELAY_SCROLL_SHIFT, 30}); // fixme ?
      send (m_io.message_finish ());

      m_io.message_start ();
      m_io.message_add ({SPI_SLAVE_MSG_BRIGHTNESS, SPI_BRIGHTNESS_MAX});
      send (m_io.message_finish ());

      m_io.message_start ();
      m_io.message_add ({SPI_SLAVE_MSG_START});
      send (m_io.message_finish ());
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
      m_io.message_start ();
      m_io.message_add ({SPI_SLAVE_MSG_STOP});
      send (m_io.message_finish ());
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
    m_io.message_start ();
    m_io.message_add ({SPI_SLAVE_MSG_MATRIX,
          static_cast<uchar_t>
          (matrix.size () % std::numeric_limits<uchar_t>::max ()), //lsb
          static_cast<uchar_t>
          (matrix.size () / std::numeric_limits<uchar_t>::max ()), // msb
          SPI_MATRIX_ARRAY_START});
    for (std::size_t i = 0; i < matrix.size (); ++i)
      m_io.message_add (get_char (matrix.get_column (i)));
    m_io.message_add (SPI_MATRIX_ARRAY_FINISH);
    return status_send (m_io.message_finish ());
  }

  bool spi_t::brightness (int level)
  {
    if ((level < SPI_BRIGHTNESS_MIN)
        || (level > SPI_BRIGHTNESS_MAX))
      return false;

    // <msg-id><brightness-level>
    m_io.message_start ();
    m_io.message_add ({SPI_SLAVE_MSG_BRIGHTNESS, static_cast<uchar_t>(level)});

    return status_send (m_io.message_finish ());
  }

  bool spi_t::switch_relay (bool on)
  {
    uchar_t state = static_cast<uchar_t>((on == true) ? 1 : 0);

    m_io.message_start ();
    m_io.message_add ({SPI_SLAVE_MSG_SWITCH_RELAY, state});
    
    return status_send (m_io.message_finish ());
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
  
  spi_t::name_list_t spi_t::get_name_list ()
  {
    name_list_t result (SPI_SLAVE_MSG_MAX, "Unknown");

    result[SPI_SLAVE_MSG_START] = "Start";
    result[SPI_SLAVE_MSG_STOP] = "Stop";
    result[SPI_SLAVE_MSG_HANDSHAKE] = "Handshake";
    result[SPI_SLAVE_MSG_MATRIX] = "Matrix";
    result[SPI_SLAVE_MSG_DELAY] = "Delay";
    result[SPI_SLAVE_MSG_BRIGHTNESS] = "Brightness";

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

    //try {
    m_io.write (msg);
    uchar_t status = m_io.read ();
    if (status != SPI_STATUS_OK)
      throw std::logic_error (get_error (status));
    //}
    //catch (std::exception &e) {
    // add message name to exception
    //  const std::string name (name_list[msg[0]]);
    //  log_t::buffer_t buf;
    //  buf << name << ": " << e.what ();
    // throw std::domain_error (buf.str ());
    //}
  }

} // namespace ledhw
