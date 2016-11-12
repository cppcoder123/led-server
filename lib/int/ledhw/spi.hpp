//
//
//
#ifndef LEDHW_SPI_HPP
#define LEDHW_SPI_HPP

#include <string>
#include <vector>

#include "ledhw/hw.hpp"

namespace ledhw
{

  class spi_t : public hw_t
  {

  public:

    spi_t ();
    ~spi_t () {}

    bool start ();
    void stop ();

    bool render (const libled::matrix_t &matrix);
    bool brightness (int level);

  private:

    typedef unsigned char uchar;
    typedef std::vector<uchar> vector_t;

    typedef libled::matrix_t::column_t column_t;

    typedef std::vector<std::string> name_list_t;

    
    static uchar get_char (const column_t &column);

    static name_list_t get_name_list ();
    
    bool status_send (const vector_t &msg); // doesn't throw
    void send (const vector_t &msg);        // throws

    bool write (const vector_t &msg);
    bool read ();
    
  };
  
} // namespace ledhw

#endif
