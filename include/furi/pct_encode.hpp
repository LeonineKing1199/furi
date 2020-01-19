//
// Copyright (c) 2018-2019 Christian Mazakas (christian dot mazakas at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/LeonineKing1199/furi
//

#ifndef FURI_PCT_ENCODE_HPP_
#define FURI_PCT_ENCODE_HPP_

#include <furi/uri.hpp>
#include <furi/utf8.hpp>
#include <furi/code_point_view.hpp>

#include <string_view>
#include <boost/locale/utf.hpp>

#include <boost/spirit/include/karma_generate.hpp>
#include <boost/spirit/include/karma_string.hpp>
#include <boost/spirit/include/karma_sequence.hpp>
#include <boost/spirit/include/karma_numeric.hpp>
#include <boost/spirit/include/karma_right_alignment.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <array>
#include <iterator>
#include <type_traits>

namespace furi
{
namespace uri
{
template <class OutputIterator>
auto
encode_host(std::u32string_view const host, OutputIterator out) -> OutputIterator
{
  namespace x3    = boost::spirit::x3;
  namespace utf   = boost::locale::utf;
  namespace karma = boost::spirit::karma;

  auto pos = host.begin();

  auto match = x3::parse(pos, host.end(), unicode::ip_literal | unicode::ip_v4_address);
  if (match) {
    for (auto const code_point : host) { out = utf::utf_traits<char>::encode(code_point, out); }
    return out;
  }

  for (auto const code_point : host) {
    pos = host.begin();

    // no need to encode the normal ascii set
    //
    if ((code_point > 32) && (code_point < 127) &&
        std::u32string_view(U"\"#/<>?@[\\]^`{|}").find(code_point) == std::u32string_view::npos) {
      out = utf::utf_traits<char>::encode(code_point, out);
      continue;
    }

    auto buffer = std::array<char, 4>{0x7f, 0x7f, 0x7f, 0x7f};

    auto const end = ::furi::utf8_encode(code_point, buffer.begin());

    for (auto pos = buffer.begin(); pos < end; ++pos) {
      karma::generate(out, karma::lit("%") << karma::right_align(2, karma::lit("0"))[karma::hex],
                      *pos);
    }
  }

  return out;
}

template <class OutputIterator>
auto
encode_path(std::u32string_view const host, OutputIterator out) -> OutputIterator
{
  namespace x3    = boost::spirit::x3;
  namespace utf   = boost::locale::utf;
  namespace karma = boost::spirit::karma;

  for (auto const code_point : host) {
    // no need to encode the normal ascii set
    //
    if ((code_point > 32) && (code_point < 127) &&
        std::u32string_view(U"\"#<>?[\\]^`{|}").find(code_point) == std::u32string_view::npos) {
      out = utf::utf_traits<char>::encode(code_point, out);
      continue;
    }

    auto buffer = std::array<char, 4>{0x7f, 0x7f, 0x7f, 0x7f};

    auto const end = ::furi::utf8_encode(code_point, buffer.begin());

    for (auto pos = buffer.begin(); pos < end; ++pos) {
      karma::generate(out, karma::lit("%") << karma::right_align(2, karma::lit("0"))[karma::hex],
                      *pos);
    }
  }

  return out;
}

template <class OutputIterator>
auto
encode_query(std::u32string_view const host, OutputIterator out) -> OutputIterator
{
  namespace x3    = boost::spirit::x3;
  namespace utf   = boost::locale::utf;
  namespace karma = boost::spirit::karma;

  for (auto const code_point : host) {
    // no need to encode the normal ascii set
    //
    if ((code_point > 32) && (code_point < 127) &&
        std::u32string_view(U"\"#:<>@[\\]^`{|}").find(code_point) == std::u32string_view::npos) {
      out = utf::utf_traits<char>::encode(code_point, out);
      continue;
    }

    auto buffer = std::array<char, 4>{0x7f, 0x7f, 0x7f, 0x7f};

    auto const end = ::furi::utf8_encode(code_point, buffer.begin());

    for (auto pos = buffer.begin(); pos < end; ++pos) {
      karma::generate(out, karma::lit("%") << karma::right_align(2, karma::lit("0"))[karma::hex],
                      *pos);
    }
  }

  return out;
}

template <class OutputIterator>
auto
encode_fragment(std::u32string_view const host, OutputIterator out) -> OutputIterator
{
  return encode_query(host, out);
}

} // namespace uri
} // namespace furi

#endif // FURI_PCT_ENCODE_HPP_
