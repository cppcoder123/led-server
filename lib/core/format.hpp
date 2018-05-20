//
//
//
#ifndef CORE_FORMAT_HPP
#define CORE_FORMAT_HPP

#include <list>
#include <memory>

#include "format-entry.hpp"
#include "refsymbol.hpp"
#include "token.hpp"

namespace core
{

  class format_t
  {

  public:

    typedef std::unique_ptr<format_t> format_ptr_t;
    typedef std::list<format_ptr_t> list_t;

    static bool split (list_t &list, const std::string &format_string);
    static void join (std::string &format_string, const list_t &list);
    
    static std::string encode_empty () {return "-";}

    format_t ();
    ~format_t () {}

    std::size_t get_start () const {return m_start.info;}
    std::size_t get_size () const {return m_size.info;}
    std::size_t is_unlimited () const {return m_size.info == std::string::npos;}
    const std::string& get_font () const {return m_font.info;}
    bool get_inversion () const {return m_inversion.info;}

    void reset ();
    
  private:

    typedef token_t::pair_vector_t pair_vector_t;
    static bool split (format_t &format, std::size_t &count,
                       pair_vector_t &pair_vector, const std::string &src);

    typedef format::entry_t<char> end_t;
    static const end_t& get_end ();

    bool decode (bool &last, const std::string &src);

    
    format::entry_t<std::size_t> m_start, m_size;
    format::entry_t<std::string> m_font;
    format::entry_t<bool> m_inversion;
  };

  //
  //
  //
  inline bool format_t::split (list_t &dst, const std::string &src)
  {
    token_t::pair_vector_t pair_vector;
    if (token_t::tokenize (pair_vector, src, refsymbol_t::replace) == false)
      return false;

    format_t format;
    std::size_t count (0);      // pair counter
    while (split (format, count, pair_vector, src) == true) {
      dst.push_back (format_ptr_t (new format_t (format)));
      format.reset ();
    }

    return dst.empty () == false;
  }

  inline format_t::format_t ()
  // Attention: key should be unique, note 'get_end'
    : m_start ('s', 0),
      m_size ('i', std::string::npos),
      m_font ('f', ""),
      m_inversion ('n', false)
  {
  }

  inline bool format_t::split (format_t &format,
                               std::size_t &count,
                               pair_vector_t &pair_vector,
                               const std::string &src)
  {
    while (count < pair_vector.size ()) {
      token_t::position_pair_t pair (pair_vector[count++]);
      bool last (false);
      if (format.decode (last,
                         src.substr (pair.first, pair.second)) == false)
        // how to complain?
        continue;
      if (last == true)
        return true;
    }

    return false;
  }

  inline const format_t::end_t& format_t::get_end ()
  {
    // Attention: key should be unique, note 'format_t::format_t'
    static const end_t end ('e', 'e');

    return end;
  }

  inline bool format_t::decode (bool &last, const std::string &src)
  {
    if (src.size () < 1)
      return false;

    char key (src[0]);
    const std::string tail (src.substr (1));

    if (key == m_start.key)
      return token_t::decode (m_start.info, tail);
    else if (key == m_size.key)
      return token_t::decode (m_size.info, tail);
    else if (key == m_font.key)
      return token_t::decode (m_font.info, tail);
    else if (key == m_inversion.key)
      return token_t::decode (m_inversion.info, tail);
    else if (key == (get_end ()).key) 
      return last = true;

    return false;
  }

  inline void format_t::reset ()
  {
    m_start.reset ();
    m_size.reset ();
    m_font.reset ();
    m_inversion.reset ();
  }
  
} // namespace core

#endif
