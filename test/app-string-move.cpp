//
//
//

#include <array>
#include <iostream>
#include <string>

#include "unix/string-move.hpp"

int main ()
{
  std::array<char, 10> a1 = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
  std::string s1 = "test1-xyz-abc";
  std::string s2;

  unix::string_move (s1, a1, s2);

  std::cout << "a1: \"";
  for (char s : a1)
    std::cout << s;
  std::cout << "\"\n";
  
  std::cout << "s2: \"" << s2 << "\"\n";

  s1 = "abcdefghijklmno";
  std::cout << "s1 before \"" << s1 << "\"\n";
  auto len = unix::string_move (s1, a1);
  std::cout << "len: \"" << len << "\"\n";
  std::cout << "s1 after \"" << s1 << "\"\n";
  std::cout << "a1: \"";
  for (char s : a1)
    std::cout << s;
  std::cout << "\"\n";

  s1 = "xyz";
  std::cout << "s1 before \"" << s1 << "\"\n";
  unix::string_move_append (a1, a1.size (), s1);
  std::cout << "s1 after \"" << s1 << "\"\n";
  std::cout << "a1: \"";
  for (char s : a1)
    std::cout << s;
  std::cout << "\"\n";
    
  

  return 0;
}
