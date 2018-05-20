//
//
//
#ifndef LIBLED_FORMAT_ENTRY_HPP
#define LIBLED_FORMAT_ENTRY_HPP

namespace libled
{
  namespace format
  {

    template <typename info_t>
    struct entry_t
    {

      entry_t () = delete;
      entry_t (char info_key, const info_t &def)
        : key (info_key), info (def), default_info (def) {}
      ~entry_t () {}

      void reset () {info = default_info;}
      
      const char key;
      info_t info;
      const info_t default_info;
      
    };
    
  } // namespace format
} // namespace libled

#endif
