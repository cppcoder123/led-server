//
//
//
#ifndef CORE_MATRIX_HPP
#define CORE_MATRIX_HPP

#include <algorithm>
#include <bitset>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "row-help.hpp"

namespace core
{

  class matrix_t
  {

  public:

    static const std::size_t column_size = 8;
    typedef std::bitset<column_size> column_t;
    
    matrix_t () {}
    virtual ~matrix_t () {}

    void dump () const;         // for debugging

    std::size_t size () const {return m_vector.size ();}
    bool empty () const {return m_vector.empty ();}

    const column_t& get_column (std::size_t i) const {return m_vector[i];}

    void add (const column_t &column);
    void add (const matrix_t &arg, std::size_t start, std::size_t size);
    void add (const matrix_t &arg);

    void clear ();
    
    template <typename row_t>
    void add_row (const row_t &row, std::size_t y, std::size_t start_x);

    void invert ();
    
  private:

    typedef std::vector<column_t> vector_t;
    vector_t m_vector;
    
  };

  //
  //
  //
  inline void matrix_t::dump () const
  {
    for (std::size_t y = 0; y < column_size; ++y) {
      for (std::size_t x = 0; x < m_vector.size (); ++x) {
        const column_t &column (m_vector[x]);
        std::cout << ((column.test (y) == true) ? 'O' : ' ');
      }
      std::cout << std::endl;
    }
  }

  inline void matrix_t::add (const column_t &column)
  {
    m_vector.push_back (column);
  }
  
  inline void matrix_t::add (const matrix_t &arg,
                             std::size_t start, std::size_t size)
  {
    for (std::size_t i = start; i < start + size; ++i)
      m_vector.push_back (arg.m_vector[i]);
  }

  inline void matrix_t::add (const matrix_t &arg)
  {
    for (std::size_t i = 0; i < arg.m_vector.size (); ++i)
      m_vector.push_back (arg.m_vector[i]);
  }

  inline void matrix_t::clear ()
  {
    m_vector.clear ();
  }
  
  // start_x should be equal size, or start_x + row-size == size()
  // otherwise throw
  template <typename row_t>
  void matrix_t::add_row (const row_t &row, std::size_t y, std::size_t start_x)
  {
    if (y >= column_size)
      throw std::invalid_argument ("incorrect y value");
    
    typedef row::help_t<row_t> help_t;
    const std::size_t row_size (help_t::size (row));

    if (start_x == size ()) {
      // allocate space for new info
      vector_t empty (row_size, column_t ());
      m_vector.insert (m_vector.end (), empty.begin (), empty.end ());
    } else if (start_x + row_size == size ())
      ;
    else
      throw std::invalid_argument ("incorrect start_x value");

    // fill info
    for (std::size_t x = 0; x < row_size; ++x) {
      column_t &column (m_vector[start_x + x]);
      column.set (y, help_t::test (row, x));
    }
  }

  inline void matrix_t::invert ()
  {
    std::for_each (m_vector.begin (), m_vector.end (),
                   [] (column_t &column)
                   {
                     column.flip ();
                   });
  }
  
} // namespace core

#endif
