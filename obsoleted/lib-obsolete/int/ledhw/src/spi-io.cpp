//
//
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include "libled/log.hpp"
#include "libled/spi-message.h"

#include "spi-io.hpp"

namespace ledhw
{
  typedef libled::log_t::buffer_t buffer_t;
  typedef std::vector<unsigned char> vector_t;
  
  spi_io_t::spi_io_t (const std::string &devname)
    : m_devname (devname),
      m_fd (-1)
  {
  }

  void spi_io_t::start ()
  {
    m_fd = open (m_devname.c_str (), O_RDWR);
    if (m_fd < 0) {
      buffer_t buf;
      buf << "failed to open spi device \"" << m_devname << "\"";
      throw std::domain_error(buf.str ());
    }
  }

  void spi_io_t::stop ()
  {
    if (m_fd < 0)
      return;
    
    if (close (m_fd) != 0) {
      buffer_t buf;
      buf << "failed to close spi device \"" << m_devname << "\"";
      throw std::domain_error(buf.str ());
    }

    m_fd = -1;
  }

  void spi_io_t::message_start ()
  {
    m_message.clear ();
    unsigned char tmp = SPI_SLAVE_START;
    m_message.push_back (~tmp);
  }

  void spi_io_t::message_add (const unsigned char msg_body)
  {
    // we need to negate message
    m_message.push_back (~msg_body);
  }

  void spi_io_t::message_add (const vector_t &msg_body)
  {
    //m_message.insert (m_message.end (), msg_body.begin (), msg_body.end ());
    std::for_each (msg_body.begin (), msg_body.end (),
                   [this] (unsigned char msg_byte)
                   {
                     m_message.push_back (~msg_byte);
                   });
  }

  const spi_io_t::vector_t& spi_io_t::message_finish ()
  {
    unsigned char tmp = SPI_SLAVE_FINISH;
    m_message.push_back (~tmp);

    return m_message;
  }

  void spi_io_t::write (const vector_t &msg)
  {
    if (m_fd < 0)
      throw std::domain_error ("Can't write, device is not opened");

    if (::write (m_fd, msg.data (), msg.size ())
        != static_cast<ssize_t>(msg.size ())) {
      buffer_t buf;
      buf << "Failed to write \"" << msg.size () << "\" bytes to spi";
      throw std::domain_error (buf.str ());
    }
  }

  unsigned char spi_io_t::read ()
  {
    if (m_fd < 0) 
      throw std::domain_error ("Can't read, device is not opened");

    // fixme : 15 bytes : is it enough?
    static const std::size_t read_buffer_size = 15;
    vector_t raw_msg (read_buffer_size, 0);

    // fixme: do we need a loop here ?
    
    if (::read (m_fd, raw_msg.data (), raw_msg.size ())
        != static_cast<ssize_t>(raw_msg.size ())) {
      buffer_t buf;
      buf << "Failed to read \"" << raw_msg.size () << "\" bytes from spi";
      throw std::domain_error (buf.str ());
    }

    for (std::size_t i = 0; i < raw_msg.size () - 2; i++)
      if ((raw_msg[i] == SPI_MASTER_START)
          && (raw_msg[i + 2] == SPI_MASTER_FINISH))
        return raw_msg[i + 1];

    throw std::runtime_error ("Failed to find reply during spi read");
  }
  
}// namespace ledhw
