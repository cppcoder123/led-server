//
//
//
#ifndef LIBLED_ROW_HELP_HPP
#define LIBLED_ROW_HELP_HPP

namespace libled
{
  namespace row
  {

    template <typename info_t>
    struct help_t
    {
      static std::size_t size (const info_t &info) {return info.size ();}
      //
      static bool test (const info_t &info, std::size_t index)
      {return info.test (index);}
    };

    // template <>
    // struct help_t<bool>
    // {
    //   static std::size_t size () {return 1;}
    //   static bool test (bool info, std::size_t) {return info;}
    // };

  } // namespace row
} // namespace libled

#endif
