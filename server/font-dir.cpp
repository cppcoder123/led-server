//
//
//

#include "unix/font-name.hpp"

#include "font-dir.hpp"
#include "log-wrapper.hpp"

namespace led_d
{
  font_dir_t::font_dir_t (const std::string &default_font)
    : m_font_map ({
        std::make_pair (font_name_t::id_ibm, get_info (font_name_t::id_ibm)),
        std::make_pair (font_name_t::id_slim, get_info (font_name_t::id_slim)),
        std::make_pair (font_name_t::id_greek, get_info (font_name_t::id_greek))
      }),
      m_default_font (m_font_map.at (font_name_t::get (default_font)))
  {
  }

  const font_t& font_dir_t::get_font (const std::string &name) const
  {
    return m_font_map.at (font_name_t::get (name));
  }

  font_t font_dir_t::get_info (font_name_t::id_t id)
  {
    font_t font;

    switch (id) {
    case font_name_t::id_ibm:
      font.add (' ', { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
      font.add ('!', { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00});
      font.add ('"', { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
      font.add ('#', { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00});
      font.add ('$', { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00});
      font.add ('%', { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00});
      font.add ('&', { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00});
      font.add ('\'', { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00});
      font.add ('(', { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00});
      font.add (')', { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00});
      font.add ('*', { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00});
      font.add ('+', { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00});
      font.add (',', { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06});
      font.add ('-', { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00});
      font.add ('.', { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00});
      font.add ('/', { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00});
      font.add ('0', { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00});
      font.add ('1', { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00});
      font.add ('2', { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00});
      font.add ('3', { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00});
      font.add ('4', { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00});
      font.add ('5', { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00});
      font.add ('6', { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00});
      font.add ('7', { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00});
      font.add ('8', { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00});
      font.add ('9', { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00});
      font.add (':', { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00});
      font.add ('/', { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06});
      font.add ('<', { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00});
      font.add ('=', { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00});
      font.add ('>', { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00});
      font.add ('?', { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00});
      font.add ('@', { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00});
      font.add ('A', { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00});
      font.add ('B', { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00});
      font.add ('C', { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00});
      font.add ('D', { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00});
      font.add ('E', { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00});
      font.add ('F', { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00});
      font.add ('G', { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00});
      font.add ('H', { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00});
      font.add ('I', { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00});
      font.add ('J', { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00});
      font.add ('K', { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00});
      font.add ('L', { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00});
      font.add ('M', { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00});
      font.add ('N', { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00});
      font.add ('O', { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00});
      font.add ('P', { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00});
      font.add ('Q', { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00});
      font.add ('R', { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00});
      font.add ('S', { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00});
      font.add ('T', { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00});
      font.add ('U', { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00});
      font.add ('V', { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00});
      font.add ('W', { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00});
      font.add ('X', { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00});
      font.add ('Y', { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00});
      font.add ('Z', { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00});
      font.add ('[', { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00});
      font.add ('\\', { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00});
      font.add (']', { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00});
      font.add ('^', { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00});
      font.add ('_', { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF});
      font.add ('`', { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00});
      font.add ('a', { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00});
      font.add ('b', { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00});
      font.add ('c', { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00});
      font.add ('d', { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00});
      font.add ('e', { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00});
      font.add ('f', { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00});
      font.add ('g', { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F});
      font.add ('h', { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00});
      font.add ('i', { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00});
      font.add ('j', { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E});
      font.add ('k', { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00});
      font.add ('l', { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00});
      font.add ('m', { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00});
      font.add ('n', { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00});
      font.add ('o', { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00});
      font.add ('p', { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F});
      font.add ('q', { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78});
      font.add ('r', { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00});
      font.add ('s', { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00});
      font.add ('t', { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00});
      font.add ('u', { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00});
      font.add ('v', { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00});
      font.add ('w', { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00});
      font.add ('x', { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00});
      font.add ('y', { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F});
      font.add ('z', { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00});
      font.add ('{', { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00});
      font.add ('|', { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00});
      font.add ('}', { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00});
      font.add ('~', { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
      break;
    case font_name_t::id_slim:
      font.add (' ', {0x00, 0x00, 0x00, 0x00, 0x00});
      font.add ('!', {0x00, 0x00, 0x5F, 0x00, 0x00});
      font.add ('"', {0x00, 0x07, 0x00, 0x07, 0x00});
      font.add ('#', {0x14, 0x7F, 0x14, 0x7F, 0x14});
      font.add ('$', {0x24, 0x2A, 0x7F, 0x2A, 0x12});
      font.add ('%', {0x23, 0x13, 0x08, 0x64, 0x62});
      font.add ('&', {0x36, 0x49, 0x55, 0x22, 0x50});
      font.add ('\'', {0x00, 0x05, 0x03, 0x00, 0x00});
      font.add ('(', {0x00, 0x1C, 0x22, 0x41, 0x00});
      font.add (')', {0x00, 0x41, 0x22, 0x1C, 0x00});
      font.add ('*', {0x08, 0x2A, 0x1C, 0x2A, 0x08});
      font.add ('+', {0x08, 0x08, 0x3E, 0x08, 0x08});
      font.add (',', {0x00, 0x50, 0x30, 0x00, 0x00});
      font.add ('-', {0x08, 0x08, 0x08, 0x08, 0x08});
      font.add ('.', {0x00, 0x60, 0x60, 0x00, 0x00});
      font.add ('/', {0x20, 0x10, 0x08, 0x04, 0x02});
      font.add ('0', {0x3E, 0x51, 0x49, 0x45, 0x3E});
      font.add ('1', {0x00, 0x42, 0x7F, 0x40, 0x00});
      font.add ('2', {0x42, 0x61, 0x51, 0x49, 0x46});
      font.add ('3', {0x21, 0x41, 0x45, 0x4B, 0x31});
      font.add ('4', {0x18, 0x14, 0x12, 0x7F, 0x10});
      font.add ('5', {0x27, 0x45, 0x45, 0x45, 0x39});
      font.add ('6', {0x3C, 0x4A, 0x49, 0x49, 0x30});
      font.add ('7', {0x01, 0x71, 0x09, 0x05, 0x03});
      font.add ('8', {0x36, 0x49, 0x49, 0x49, 0x36});
      font.add ('9', {0x06, 0x49, 0x49, 0x29, 0x1E});
      font.add (':', {0x00, 0x36, 0x36, 0x00, 0x00});
      font.add (';', {0x00, 0x56, 0x36, 0x00, 0x00});
      font.add ('<', {0x00, 0x08, 0x14, 0x22, 0x41});
      font.add ('=', {0x14, 0x14, 0x14, 0x14, 0x14});
      font.add ('>', {0x41, 0x22, 0x14, 0x08, 0x00});
      font.add ('?', {0x02, 0x01, 0x51, 0x09, 0x06});
      font.add ('@', {0x32, 0x49, 0x79, 0x41, 0x3E});
      font.add ('A', {0x7E, 0x11, 0x11, 0x11, 0x7E});
      font.add ('B', {0x7F, 0x49, 0x49, 0x49, 0x36});
      font.add ('C', {0x3E, 0x41, 0x41, 0x41, 0x22});
      font.add ('D', {0x7F, 0x41, 0x41, 0x22, 0x1C});
      font.add ('E', {0x7F, 0x49, 0x49, 0x49, 0x41});
      font.add ('F', {0x7F, 0x09, 0x09, 0x01, 0x01});
      font.add ('G', {0x3E, 0x41, 0x41, 0x51, 0x32});
      font.add ('H', {0x7F, 0x08, 0x08, 0x08, 0x7F});
      font.add ('I', {0x00, 0x41, 0x7F, 0x41, 0x00});
      font.add ('J', {0x20, 0x40, 0x41, 0x3F, 0x01});
      font.add ('K', {0x7F, 0x08, 0x14, 0x22, 0x41});
      font.add ('L', {0x7F, 0x40, 0x40, 0x40, 0x40});
      font.add ('M', {0x7F, 0x02, 0x04, 0x02, 0x7F});
      font.add ('N', {0x7F, 0x04, 0x08, 0x10, 0x7F});
      font.add ('O', {0x3E, 0x41, 0x41, 0x41, 0x3E});
      font.add ('P', {0x7F, 0x09, 0x09, 0x09, 0x06});
      font.add ('Q', {0x3E, 0x41, 0x51, 0x21, 0x5E});
      font.add ('R', {0x7F, 0x09, 0x19, 0x29, 0x46});
      font.add ('S', {0x46, 0x49, 0x49, 0x49, 0x31});
      font.add ('T', {0x01, 0x01, 0x7F, 0x01, 0x01});
      font.add ('U', {0x3F, 0x40, 0x40, 0x40, 0x3F});
      font.add ('V', {0x1F, 0x20, 0x40, 0x20, 0x1F});
      font.add ('W', {0x7F, 0x20, 0x18, 0x20, 0x7F});
      font.add ('X', {0x63, 0x14, 0x08, 0x14, 0x63});
      font.add ('Y', {0x03, 0x04, 0x78, 0x04, 0x03});
      font.add ('Z', {0x61, 0x51, 0x49, 0x45, 0x43});
      font.add ('[', {0x00, 0x00, 0x7F, 0x41, 0x41});
      font.add ('\\', {0x02, 0x04, 0x08, 0x10, 0x20});
      font.add (']', {0x41, 0x41, 0x7F, 0x00, 0x00});
      font.add ('^', {0x04, 0x02, 0x01, 0x02, 0x04});
      font.add ('_', {0x40, 0x40, 0x40, 0x40, 0x40});
      font.add ('`', {0x00, 0x01, 0x02, 0x04, 0x00});
      font.add ('a', {0x20, 0x54, 0x54, 0x54, 0x78});
      font.add ('b', {0x7F, 0x48, 0x44, 0x44, 0x38});
      font.add ('c', {0x38, 0x44, 0x44, 0x44, 0x20});
      font.add ('d', {0x38, 0x44, 0x44, 0x48, 0x7F});
      font.add ('e', {0x38, 0x54, 0x54, 0x54, 0x18});
      font.add ('f', {0x08, 0x7E, 0x09, 0x01, 0x02});
      font.add ('g', {0x08, 0x14, 0x54, 0x54, 0x3C});
      font.add ('h', {0x7F, 0x08, 0x04, 0x04, 0x78});
      font.add ('i', {0x00, 0x44, 0x7D, 0x40, 0x00});
      font.add ('j', {0x20, 0x40, 0x44, 0x3D, 0x00});
      font.add ('k', {0x00, 0x7F, 0x10, 0x28, 0x44});
      font.add ('l', {0x00, 0x41, 0x7F, 0x40, 0x00});
      font.add ('m', {0x7C, 0x04, 0x18, 0x04, 0x78});
      font.add ('n', {0x7C, 0x08, 0x04, 0x04, 0x78});
      font.add ('o', {0x38, 0x44, 0x44, 0x44, 0x38});
      font.add ('p', {0x7C, 0x14, 0x14, 0x14, 0x08});
      font.add ('q', {0x08, 0x14, 0x14, 0x18, 0x7C});
      font.add ('r', {0x7C, 0x08, 0x04, 0x04, 0x08});
      font.add ('s', {0x48, 0x54, 0x54, 0x54, 0x20});
      font.add ('t', {0x04, 0x3F, 0x44, 0x40, 0x20});
      font.add ('u', {0x3C, 0x40, 0x40, 0x20, 0x7C});
      font.add ('v', {0x1C, 0x20, 0x40, 0x20, 0x1C});
      font.add ('w', {0x3C, 0x40, 0x30, 0x40, 0x3C});
      font.add ('x', {0x44, 0x28, 0x10, 0x28, 0x44});
      font.add ('y', {0x0C, 0x50, 0x50, 0x50, 0x3C});
      font.add ('z', {0x44, 0x64, 0x54, 0x4C, 0x44});
      font.add ('{', {0x00, 0x08, 0x36, 0x41, 0x00});
      font.add ('|', {0x00, 0x00, 0x7F, 0x00, 0x00});
      font.add ('}', {0x00, 0x41, 0x36, 0x08, 0x00});
      //font.add ('->', {0x08, 0x08, 0x2A, 0x1C, 0x08});
      //font.add ('<-', {0x08, 0x1C, 0x2A, 0x08, 0x08});
      break;
    case font_name_t::id_greek:
      // fixme
      break;
    default:
      break;
    }

    return font;
  }

#if 0
  // Greek font, do we need it?
  const font_t::hex_font_t font_t::greek_font = {
    { 0x2D, 0x00, 0x0C, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+0390 (iota with tonos and diaeresis)
    { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},   // U+0391 (Alpha)
    { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},   // U+0392 (Beta)
    { 0x3F, 0x33, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00},   // U+0393 (Gamma)
    { 0x08, 0x1C, 0x1C, 0x36, 0x36, 0x63, 0x7F, 0x00},   // U+0394 (Delta)
    { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00},   // U+0395 (Epsilon)
    { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},   // U+0396 (Zeta)
    { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},   // U+0397 (Eta)
    { 0x1C, 0x36, 0x63, 0x7F, 0x63, 0x36, 0x1C, 0x00},   // U+0398 (Theta)
    { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0399 (Iota)
    { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},   // U+039A (Kappa)
    { 0x08, 0x1C, 0x1C, 0x36, 0x36, 0x63, 0x63, 0x00},   // U+039B (Lambda)
    { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},   // U+039C (Mu)
    { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},   // U+039D (Nu)
    { 0x7F, 0x63, 0x00, 0x3E, 0x00, 0x63, 0x7F, 0x00},   // U+039E (Xi)
    { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},   // U+039F (Omikron)
    { 0x7F, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x00},   // U+03A0 (Pi)
    { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00},   // U+03A1 (Rho)
    { 0x00, 0x01, 0x02, 0x04, 0x4F, 0x90, 0xA0, 0x40},   // U+03A2
    { 0x7F, 0x63, 0x06, 0x0C, 0x06, 0x63, 0x7F, 0x00},   // U+03A3 (Sigma 2)
    { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+03A4 (Tau)
    { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},   // U+03A5 (Upsilon)
    { 0x18, 0x7E, 0xDB, 0xDB, 0xDB, 0x7E, 0x18, 0x00},   // U+03A6 (Phi)
    { 0x63, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x63, 0x00},   // U+03A7 (Chi)
    { 0xDB, 0xDB, 0xDB, 0x7E, 0x18, 0x18, 0x3C, 0x00},   // U+03A8 (Psi)
    { 0x3E, 0x63, 0x63, 0x63, 0x36, 0x36, 0x77, 0x00},   // U+03A9 (Omega)
    { 0x33, 0x00, 0x1E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},   // U+0399 (Iota with diaeresis)
    { 0x33, 0x00, 0x33, 0x33, 0x1E, 0x0C, 0x1E, 0x00},   // U+03A5 (Upsilon with diaeresis)
    { 0x70, 0x00, 0x6E, 0x3B, 0x13, 0x3B, 0x6E, 0x00},   // U+03AC (alpha aigu)
    { 0x38, 0x00, 0x1E, 0x03, 0x0E, 0x03, 0x1E, 0x00},   // U+03AD (epsilon aigu)
    { 0x38, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x30},   // U+03AE (eta aigu)
    { 0x38, 0x00, 0x0C, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+03AF (iota aigu)
    { 0x2D, 0x00, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+03B0 (upsilon with tonos and diaeresis)
    { 0x00, 0x00, 0x6E, 0x3B, 0x13, 0x3B, 0x6E, 0x00},   // U+03B1 (alpha)
    { 0x00, 0x1E, 0x33, 0x1F, 0x33, 0x1F, 0x03, 0x03},   // U+03B2 (beta)
    { 0x00, 0x00, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x00},   // U+03B3 (gamma)
    { 0x38, 0x0C, 0x18, 0x3E, 0x33, 0x33, 0x1E, 0x00},   // U+03B4 (delta)
    { 0x00, 0x00, 0x1E, 0x03, 0x0E, 0x03, 0x1E, 0x00},   // U+03B5 (epsilon)
    { 0x00, 0x3F, 0x06, 0x03, 0x03, 0x1E, 0x30, 0x1C},   // U+03B6 (zeta)
    { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x30},   // U+03B7 (eta)
    { 0x00, 0x00, 0x1E, 0x33, 0x3F, 0x33, 0x1E, 0x00},   // U+03B8 (theta)
    { 0x00, 0x00, 0x0C, 0x0C, 0x0C, 0x2C, 0x18, 0x00},   // U+03B9 (iota)
    { 0x00, 0x00, 0x33, 0x1B, 0x0F, 0x1B, 0x33, 0x00},   // U+03BA (kappa)
    { 0x00, 0x03, 0x06, 0x0C, 0x1C, 0x36, 0x63, 0x00},   // U+03BB (lambda)
    { 0x00, 0x00, 0x66, 0x66, 0x66, 0x3E, 0x06, 0x03},   // U+03BC (mu)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},   // U+03BD (nu)
    { 0x1E, 0x03, 0x0E, 0x03, 0x03, 0x1E, 0x30, 0x1C},   // U+03BE (xi)
    { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+03BF (omikron)
    { 0x00, 0x00, 0x7F, 0x36, 0x36, 0x36, 0x36, 0x00},   // U+03C0 (pi)
    { 0x00, 0x00, 0x3C, 0x66, 0x66, 0x36, 0x06, 0x06},   // U+03C1 (rho)
    { 0x00, 0x00, 0x3E, 0x03, 0x03, 0x1E, 0x30, 0x1C},   // U+03C2 (sigma 1)
    { 0x00, 0x00, 0x7E, 0x1B, 0x1B, 0x1B, 0x0E, 0x00},   // U+03C3 (sigma 2)
    { 0x00, 0x00, 0x7E, 0x18, 0x18, 0x58, 0x30, 0x00},   // U+03C4 (tau)
    { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00},   // U+03C5 (upsilon)
    { 0x00, 0x00, 0x76, 0xDB, 0xDB, 0x7E, 0x18, 0x00},   // U+03C6 (phi)
    { 0x00, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},   // U+03C7 (chi)
    { 0x00, 0x00, 0xDB, 0xDB, 0xDB, 0x7E, 0x18, 0x00},   // U+03C8 (psi)
    { 0x00, 0x00, 0x36, 0x63, 0x6B, 0x7F, 0x36, 0x00}    // U+03C9 (omega)
  };
#endif
  
} // namespace led_d
