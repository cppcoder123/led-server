//
//
//
#ifndef LEDHW_SPI_IO_HPP
#define LEDHW_SPI_IO_HPP

#include <string>

namespace ledhw
{
  class spi_io
  {
    
  public:

    spi_io (const std::string &devname);
    ~spi_io () {}

    // throws
    void start ();              // open device ...
    void stop ();               // close

    // both functions throw
    typedef std::vector<unsigned char> vector_t;
    void write (const vector_t &msg);
    unsigned char read ();

  private:

    const std::string m_devname;
    int m_fd;
  };
} // namespace ledhw

#endif
