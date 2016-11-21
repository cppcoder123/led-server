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

    bool switch_relay (bool on);
    
  private:

    typedef unsigned char uchar_t;
    typedef std::vector<uchar_t> vector_t;

    typedef libled::matrix_t::column_t column_t;

    typedef std::vector<std::string> name_list_t;

    
    static uchar_t get_char (const column_t &column);

    static std::string get_error (uchar_t err_id);
    static name_list_t get_error_list ();

    static name_list_t get_name_list ();
    
    static bool status_send (const vector_t &msg); // doesn't throw
    static void send (const vector_t &msg);        // throws
  };
  
} // namespace ledhw

#endif
