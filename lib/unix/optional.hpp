/*
 *
 */
#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#ifdef GCC6

#include <experimental/optional>

template <typename info>
using optional_t = std::experimental::optional<info>;

#else

#include <optional>

template <typename info>
using optional_t = std::optional<info>;
   
#endif

#endif
