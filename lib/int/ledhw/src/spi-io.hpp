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

    typedef std::vector<unsigned char> vector_t;
    
    void message_start ();
    void message_add (const unsigned char msg_body);
    void message_add (const vector_t &msg_body);
    const vector_t& message_finish ();
    
    // both functions throw
    void write (const vector_t &msg);
    unsigned char read ();

  private:

    const std::string m_devname;
    int m_fd;

    vector_t m_message;
  };
} // namespace ledhw

#endif
