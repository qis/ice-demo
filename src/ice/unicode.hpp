// ============================================================================
// Utf8Cpp License
// ============================================================================
// Copyright 2006 Nemanja Trifunovic
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// ============================================================================
// Utf8Proc License
// ============================================================================
// utf8proc is a software package originally developed
// by Jan Behrens and the rest of the Public Software Group, who
// deserve nearly all of the credit for this library, that is now maintained
// by the Julia-language developers. Like the original utf8proc,
// whose copyright and license statements are reproduced below, all new
// work on the utf8proc library is licensed under the MIT "expat" license:
//
// Copyright (c) 2014-2019 by Steven G. Johnson, Jiahao Chen, Tony Kelman,
// Jonas Fonseca, and other contributors listed in the git history.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// ============================================================================
// Original Utf8Proc License
// ============================================================================
// Copyright (c) 2009, 2013 Public Software Group e. V., Berlin, Germany
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// ============================================================================
// Unicode Data License
// ============================================================================
// This software contains data (src/unicode.cpp) derived from processing
// the Unicode data files. The following license applies to that data:
//
// COPYRIGHT AND PERMISSION NOTICE
//
// Copyright (c) 1991-2007 Unicode, Inc. All rights reserved. Distributed
// under the Terms of Use in http://www.unicode.org/copyright.html.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of the Unicode data files and any associated documentation (the "Data
// Files") or Unicode software and any associated documentation (the
// "Software") to deal in the Data Files or Software without restriction,
// including without limitation the rights to use, copy, modify, merge,
// publish, distribute, and/or sell copies of the Data Files or Software, and
// to permit persons to whom the Data Files or Software are furnished to do
// so, provided that (a) the above copyright notice(s) and this permission
// notice appear with all copies of the Data Files or Software, (b) both the
// above copyright notice(s) and this permission notice appear in associated
// documentation, and (c) there is clear notice in each modified Data File or
// in the Software as well as in the documentation associated with the Data
// File(s) or Software that the data or software has been modified.
//
// THE DATA FILES AND SOFTWARE ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
// KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
// THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
// INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR
// CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
// USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THE DATA FILES OR SOFTWARE.
//
// Except as contained in this notice, the name of a copyright holder shall
// not be used in advertising or otherwise to promote the sale, use or other
// dealings in these Data Files or Software without prior written
// authorization of the copyright holder.
//
// Unicode and the Unicode logo are trademarks of Unicode, Inc., and may be
// registered in some jurisdictions. All other trademarks and registered
// trademarks mentioned herein are the property of their respective owners.

#pragma once
#include <ice/result.hpp>
#include <iterator>
#include <string>
#include <string_view>
#include <cstddef>
#include <cstdint>

namespace ice::unicode {
namespace detail {

// Leading (high) surrogates: 0xD800 - 0xDBFF
constexpr uint16_t lead_surrogate_min = 0xD800;
constexpr uint16_t lead_surrogate_max = 0xDBFF;

// Trailing (low) surrogates: 0xDC00 - 0xDFFF
constexpr uint16_t trail_surrogate_min = 0xDC00;
constexpr uint16_t trail_surrogate_max = 0xDFFF;

// Lead offset: lead_surrogate_min - (0x010000 >> 10)
constexpr uint16_t lead_offset = 0xD7C0;

// Surrogate offset: 0x010000 - (lead_surrogate_min << 10) - trail_surrogate_min
constexpr uint32_t surrogate_offset = 0xFCA02400;

// Maximum valid value for a Unicode code point.
constexpr uint32_t code_point_max = 0x10FFFF;

template <typename OctetType>
constexpr uint8_t mask8(OctetType oc) noexcept
{
  return static_cast<uint8_t>(0xFF & oc);
}

template <typename U16Type>
constexpr uint16_t mask16(U16Type oc) noexcept
{
  return static_cast<uint16_t>(0xFFFF & oc);
}

template <typename OctetType>
constexpr bool is_trail(OctetType oc) noexcept
{
  return ((detail::mask8(oc) >> 6) == 0x02);
}

template <typename U16>
constexpr bool is_lead_surrogate(U16 uc) noexcept
{
  return (uc >= lead_surrogate_min && uc <= lead_surrogate_max);
}

template <typename U16>
constexpr bool is_trail_surrogate(U16 uc) noexcept
{
  return (uc >= trail_surrogate_min && uc <= trail_surrogate_max);
}

template <typename U16>
constexpr bool is_surrogate(U16 uc) noexcept
{
  return (uc >= lead_surrogate_min && uc <= trail_surrogate_max);
}

template <typename U32>
constexpr bool is_code_point_valid(U32 uc) noexcept
{
  return (uc <= code_point_max && !detail::is_surrogate(uc));
}

template <typename OctetIterator>
constexpr auto sequence_length(OctetIterator lead_it) noexcept
{
  using difference_type = typename std::iterator_traits<OctetIterator>::difference_type;
  uint8_t lead = detail::mask8(*lead_it);
  if (lead < 0x80) {
    return difference_type{ 1 };
  } else if ((lead >> 5) == 0x06) {
    return difference_type{ 2 };
  } else if ((lead >> 4) == 0x0E) {
    return difference_type{ 3 };
  } else if ((lead >> 3) == 0x1E) {
    return difference_type{ 4 };
  }
  return difference_type{ 0 };
}

template <typename OctetDifferenceType>
constexpr bool is_overlong_sequence(uint32_t uc, OctetDifferenceType length) noexcept
{
  if (uc < 0x80) {
    if (length != 1) {
      return true;
    }
  } else if (uc < 0x0800) {
    if (length != 2) {
      return true;
    }
  } else if (uc < 0x010000) {
    if (length != 3) {
      return true;
    }
  }
  return false;
}

template <typename OctetIterator>
constexpr errc increase_safely(OctetIterator& it, OctetIterator end) noexcept
{
  if (++it == end) {
    return ice::errc::unicode_buffer_too_small;
  }
  if (!detail::is_trail(*it)) {
    return ice::errc::unicode_incomplete_sequence;
  }
  return ice::errc::success;
}

template <typename OctetIterator>
constexpr errc get_sequence_1(OctetIterator& it, OctetIterator end, uint32_t& uc) noexcept
{
  if (it == end) {
    return ice::errc::unicode_buffer_too_small;
  }
  uc = detail::mask8(*it);
  return ice::errc::success;
}

template <typename OctetIterator>
constexpr auto get_sequence_2(OctetIterator& it, OctetIterator end, uint32_t& uc) noexcept
{
  if (it == end) {
    return ice::errc::unicode_buffer_too_small;
  }
  uc = detail::mask8(*it);
  if (const auto code = increase_safely(it, end); code != ice::errc::success) {
    return code;
  }
  uc = ((uc << 6) & 0x07FF) + ((*it) & 0x3F);
  return ice::errc::success;
}

template <typename OctetIterator>
constexpr errc get_sequence_3(OctetIterator& it, OctetIterator end, uint32_t& uc) noexcept
{
  if (it == end) {
    return ice::errc::unicode_buffer_too_small;
  }
  uc = detail::mask8(*it);
  if (const auto code = increase_safely(it, end); code != ice::errc::success) {
    return code;
  }
  uc = ((uc << 12) & 0xFFFF) + ((detail::mask8(*it) << 6) & 0x0FFF);
  if (const auto code = increase_safely(it, end); code != ice::errc::success) {
    return code;
  }
  uc += (*it) & 0x3F;
  return ice::errc::success;
}

template <typename OctetIterator>
constexpr auto get_sequence_4(OctetIterator& it, OctetIterator end, uint32_t& uc) noexcept
{
  if (it == end) {
    return ice::errc::unicode_buffer_too_small;
  }
  uc = detail::mask8(*it);
  if (const auto code = increase_safely(it, end); code != ice::errc::success) {
    return code;
  }
  uc = ((uc << 18) & 0x1FFFFF) + ((detail::mask8(*it) << 12) & 0x03FFFF);
  if (const auto code = increase_safely(it, end); code != ice::errc::success) {
    return code;
  }
  uc += (detail::mask8(*it) << 6) & 0x0FFF;
  if (const auto code = increase_safely(it, end); code != ice::errc::success) {
    return code;
  }
  uc += (*it) & 0x3F;
  return ice::errc::success;
}

template <typename OctetIterator>
constexpr errc validate_next(OctetIterator& it, OctetIterator end, uint32_t& uc) noexcept
{
  if (it == end) {
    return ice::errc::unicode_buffer_too_small;
  }

  uint32_t result = 0;

  // Save the original value of it so we can go back in case of failure
  // Of course, it does not make much sense with i.e. stream iterators
  OctetIterator original_it = it;

  // Determine the sequence length based on the lead octet
  using difference_type = typename std::iterator_traits<OctetIterator>::difference_type;
  const difference_type length = detail::sequence_length(it);

  // Get trail octets and calculate the code point
  auto err = ice::errc::success;
  switch (length) {
  case 0:
    return ice::errc::unicode_invalid_lead;
  case 1:
    err = detail::get_sequence_1(it, end, result);
    break;
  case 2:
    err = detail::get_sequence_2(it, end, result);
    break;
  case 3:
    err = detail::get_sequence_3(it, end, result);
    break;
  case 4:
    err = detail::get_sequence_4(it, end, result);
    break;
  }

  if (err == ice::errc::success) {
    // Decoding succeeded. Now, security checks...
    if (detail::is_code_point_valid(result)) {
      if (!detail::is_overlong_sequence(result, length)) {
        // Passed! Return here.
        uc = result;
        ++it;
        return {};
      } else {
        err = ice::errc::unicode_overlong_sequence;
      }
    } else {
      err = ice::errc::unicode_invalid_code_point;
    }
  }

  // Failure branch - restore the original value of the iterator
  it = original_it;
  return err;
}

template <typename OctetIterator>
constexpr errc validate_next(OctetIterator& it, OctetIterator end) noexcept
{
  uint32_t ignored = 0;
  return detail::validate_next(it, end, ignored);
}

}  // namespace detail

// ================================================================================================
// checks
// ================================================================================================

constexpr uint8_t bom[] = { 0xEF, 0xBB, 0xBF };

template <typename OctetIterator>
constexpr bool starts_with_bom(OctetIterator it, OctetIterator end) noexcept
{
  // clang-format off
  return (
    ((it != end) && (detail::mask8(*it++)) == bom[0]) &&
    ((it != end) && (detail::mask8(*it++)) == bom[1]) &&
    ((it != end) && (detail::mask8(*it))   == bom[2])
  );
  // clang-format on
}

constexpr bool starts_with_bom(std::string_view s) noexcept
{
  return starts_with_bom(s.begin(), s.end());
}

template <typename OctetIterator>
constexpr auto find_invalid(OctetIterator start, OctetIterator end) noexcept
{
  OctetIterator result = start;
  while (result != end) {
    if (detail::validate_next(result, end) != ice::errc::success) {
      return result;
    }
  }
  return result;
}

constexpr std::size_t find_invalid(std::string_view s) noexcept
{
  std::string_view::const_iterator invalid = find_invalid(s.begin(), s.end());
  return (invalid == s.end()) ? std::string_view::npos : (invalid - s.begin());
}

template <typename OctetIterator>
constexpr bool is_valid(OctetIterator start, OctetIterator end) noexcept
{
  return (find_invalid(start, end) == end);
}

constexpr bool is_valid(std::string_view s) noexcept
{
  return is_valid(s.begin(), s.end());
}

ICE_API bool is_lower(char32_t uc) noexcept;
ICE_API bool is_upper(char32_t uc) noexcept;

// ================================================================================================
// read operations
// ================================================================================================

template <typename OctetIterator>
constexpr uint32_t next(OctetIterator& it) noexcept
{
  uint32_t uc = detail::mask8(*it);
  typename std::iterator_traits<OctetIterator>::difference_type length = detail::sequence_length(it);
  switch (length) {
  case 1:
    break;
  case 2:
    it++;
    uc = ((uc << 6) & 0x07FF) + ((*it) & 0x3F);
    break;
  case 3:
    ++it;
    uc = ((uc << 12) & 0xFFFF) + ((detail::mask8(*it) << 6) & 0x0FFF);
    ++it;
    uc += (*it) & 0x3F;
    break;
  case 4:
    ++it;
    uc = ((uc << 18) & 0x1FFFFF) + ((detail::mask8(*it) << 12) & 0x03FFFF);
    ++it;
    uc += (detail::mask8(*it) << 6) & 0x0FFF;
    ++it;
    uc += (*it) & 0x3F;
    break;
  }
  ++it;
  return uc;
}

template <typename OctetIterator>
constexpr uint32_t peek_next(OctetIterator it) noexcept
{
  return ice::unicode::next(it);
}

template <typename OctetIterator>
constexpr uint32_t prior(OctetIterator& it) noexcept
{
  while (detail::is_trail(*(--it))) {
  }
  OctetIterator temp = it;
  return ice::unicode::next(temp);
}

template <typename OctetIterator, typename DistanceType>
constexpr void advance(OctetIterator& it, DistanceType n) noexcept
{
  const DistanceType zero(0);
  if (n < zero) {
    // Backward.
    for (DistanceType i = n; i < zero; ++i) {
      ice::unicode::prior(it);
    }
  } else {
    // Forward.
    for (DistanceType i = zero; i < n; ++i) {
      ice::unicode::next(it);
    }
  }
}

template <typename OctetIterator>
constexpr auto distance(OctetIterator first, OctetIterator last) noexcept
{
  using difference_type = typename std::iterator_traits<OctetIterator>::difference_type;
  difference_type dist = 0;
  for (; first < last; ++dist) {
    ice::unicode::next(first);
  }
  return dist;
}

// ================================================================================================
// write operations
// ================================================================================================

template <typename OctetIterator>
constexpr auto append(uint32_t uc, OctetIterator result) noexcept
{
  if (uc < 0x80) {  // one octet
    *(result++) = static_cast<uint8_t>(uc);
  } else if (uc < 0x0800) {  // two octets
    *(result++) = static_cast<uint8_t>((uc >> 6) | 0xC0);
    *(result++) = static_cast<uint8_t>((uc & 0x3F) | 0x80);
  } else if (uc < 0x010000) {  // three octets
    *(result++) = static_cast<uint8_t>((uc >> 12) | 0xE0);
    *(result++) = static_cast<uint8_t>(((uc >> 6) & 0x3F) | 0x80);
    *(result++) = static_cast<uint8_t>((uc & 0x3F) | 0x80);
  } else {  // four octets
    *(result++) = static_cast<uint8_t>((uc >> 18) | 0xF0);
    *(result++) = static_cast<uint8_t>(((uc >> 12) & 0x3F) | 0x80);
    *(result++) = static_cast<uint8_t>(((uc >> 6) & 0x3F) | 0x80);
    *(result++) = static_cast<uint8_t>((uc & 0x3F) | 0x80);
  }
  return result;
}

// ================================================================================================
// normalization
// ================================================================================================

// clang-format off
enum options {
  // The given UTF-8 input is NULL terminated.
  nullterm  = (1 << 0),

  // Unicode Versioning Stability has to be respected.
  stable    = (1 << 1),

  // Compatibility decomposition (i.e. formatting information is lost).
  compat    = (1 << 2),

  // Return a result with decomposed characters.
  compose   = (1 << 3),

  // Return a result with decomposed characters.
  decompose = (1 << 4),

  // Strip "default ignorable characters" such as SOFT-HYPHEN or ZERO-WIDTH-SPACE.
  ignore    = (1 << 5),

  // Return an error, if the input contains unassigned codepoints.
  rejectna  = (1 << 6),

  // Indicating that NLF-sequences (LF, CRLF, CR, NEL) are representing a line break, and should be converted
  // to the codepoint for line separation (LS).
  nlf2ls    = (1 << 7),

  // Indicating that NLF-sequences are representing a paragraph break, and should be converted
  // to the codepoint for paragraph separation (PS).
  nlf2ps    = (1 << 8),

  // Indicating that the meaning of NLF-sequences is unknown.
  nlf2lf    = (nlf2ls | nlf2ps),

  // Strips and/or convers control characters.
  //
  // NLF-sequences are transformed into space, except if one of the NLF2LS/PS/LF options is given.
  // HorizontalTab (HT) and FormFeed (FF) are treated as a NLF-sequence in this case.
  // All other control characters are simply removed.
  stripcc   = (1 << 9),

  // Performs unicode case folding, to be able to do a case-insensitive string comparison.
  casefold  = (1 << 10),

  // Inserts 0xFF bytes at the beginning of each sequence which is representing a single grapheme cluster (see UAX#29).
  charbound = (1 << 11),

  // Lumps certain characters together.
  //
  // E.g. HYPHEN U+2010 and MINUS U+2212 to ASCII "-". See lump.md for details.
  //
  // If NLF2LF is set, this includes a transformation of paragraph and line separators to ASCII line-feed (LF).
  lump      = (1 << 12),

  // Strips all character markings.
  // This includes non-spacing, spacing and enclosing (i.e. accents).
  // This option works only with compose or decompose.
  stripmark = (1 << 13),

  //Strip unassigned codepoints.
  stripna   = (1 << 14),
};
// clang-format on

constexpr options operator|(options lhs, options rhs) noexcept
{
  return static_cast<options>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}

template <typename OctetIterator, typename OutputIterator>
constexpr auto replace_invalid(OctetIterator start, OctetIterator end, OutputIterator out, uint32_t replacement) noexcept
{
  while (start != end) {
    OctetIterator sequence_start = start;
    switch (const auto code = detail::validate_next(start, end)) {
    case ice::errc::success:
      for (OctetIterator it = sequence_start; it != start; ++it) {
        *out++ = *it;
      }
      break;
    case ice::errc::unicode_buffer_too_small:
      out = append(replacement, out);
      start = end;
      break;
    case ice::errc::unicode_invalid_lead:
      out = append(replacement, out);
      ++start;
      break;
    case ice::errc::unicode_incomplete_sequence:
    case ice::errc::unicode_overlong_sequence:
    case ice::errc::unicode_invalid_code_point:
      out = append(replacement, out);
      ++start;
      // Just one replacement mark for the sequence.
      while (start != end && detail::is_trail(*start)) {
        ++start;
      }
      break;
    default:
      break;
    }
  }
  return out;
}

template <typename OctetIterator, typename OutputIterator>
constexpr auto replace_invalid(OctetIterator start, OctetIterator end, OutputIterator out) noexcept
{
  constexpr uint32_t replacement_marker = detail::mask16(0xFFFD);
  return replace_invalid(start, end, out, replacement_marker);
}

inline std::string replace_invalid(std::string_view s, char32_t replacement) noexcept
{
  std::string result;
  replace_invalid(s.begin(), s.end(), std::back_inserter(result), replacement);
  return result;
}

inline std::string replace_invalid(std::string_view s) noexcept
{
  std::string result;
  replace_invalid(s.begin(), s.end(), std::back_inserter(result));
  return result;
}

// Decomposes, then reencodes the given string.
//
// [dec]ompose:
// - option_rejectna  : return an error if codepoint is unassigned
// - option_ignore    : strip "default ignorable" codepoints
// - option_casefold  : apply Unicode casefolding
// - option_compat    : replace certain codepoints with their compatibility decomposition
// - option_charbound : insert 0xFF bytes before each grapheme cluster
// - option_lump      : lump certain different codepoints together
// - option_stripmark : remove all character marks
// - option_stripna   : remove unassigned codepoints
//
// re[enc]ode:
// - option_nlf2ls    : convert LF, CRLF, CR and NEL into LS
// - option_nlf2ps    : convert LF, CRLF, CR and NEL into PS
// - option_nlf2lf    : convert LF, CRLF, CR and NEL into LF
// - option_stripcc   : strip or convert all non-affected control characters
// - option_compose   : try to combine decomposed codepoints into composite codepoints
// - option_stable    : prohibit combining characters that would violate the unicode versioning stability
// - option_charbound : insert 0xFF bytes before each grapheme cluster
//
ICE_API ice::result<std::string> normalize(std::string_view src, options dec, options enc = options::nlf2lf) noexcept;

// ================================================================================================
// transformation
// ================================================================================================

ICE_API char32_t to_lower(char32_t uc) noexcept;

template <typename U16bitIterator, typename OctetIterator>
constexpr auto to_lower(U16bitIterator start, U16bitIterator end, OctetIterator result) noexcept
{
  while (start != end) {
    ice::unicode::append(ice::unicode::to_lower(ice::unicode::next(start)), result);
  }
  return result;
}

inline std::string to_lower(std::string_view s) noexcept
{
  std::string result;
  ice::unicode::to_lower(s.begin(), s.end(), std::back_inserter(result));
  return result;
}

ICE_API char32_t to_upper(char32_t uc) noexcept;

template <typename U16bitIterator, typename OctetIterator>
constexpr auto to_upper(U16bitIterator start, U16bitIterator end, OctetIterator result) noexcept
{
  while (start != end) {
    ice::unicode::append(ice::unicode::to_upper(ice::unicode::next(start)), result);
  }
  return result;
}

inline std::string to_upper(std::string_view s) noexcept
{
  std::string result;
  ice::unicode::to_upper(s.begin(), s.end(), std::back_inserter(result));
  return result;
}

ICE_API char32_t to_title(char32_t uc) noexcept;

template <typename U16bitIterator, typename OctetIterator>
constexpr auto to_title(U16bitIterator start, U16bitIterator end, OctetIterator result) noexcept
{
  while (start != end) {
    ice::unicode::append(ice::unicode::to_title(ice::unicode::next(start)), result);
  }
  return result;
}

inline std::string to_title(std::string_view s) noexcept
{
  std::string result;
  ice::unicode::to_title(s.begin(), s.end(), std::back_inserter(result));
  return result;
}

// ================================================================================================
// conversion
// ================================================================================================

template <typename U16bitIterator, typename OctetIterator>
constexpr auto utf8to16(OctetIterator start, OctetIterator end, U16bitIterator result) noexcept
{
  while (start < end) {
    uint32_t uc = ice::unicode::next(start);
    if (uc > 0xFFFF) {
      // Make a surrogate pair.
      *result++ = static_cast<uint16_t>((uc >> 10) + detail::lead_offset);
      *result++ = static_cast<uint16_t>((uc & 0x03FF) + detail::trail_surrogate_min);
    } else {
      *result++ = static_cast<uint16_t>(uc);
    }
  }
  return result;
}

inline std::u16string utf8to16(std::string_view s) noexcept
{
  std::u16string result;
  utf8to16(s.begin(), s.end(), std::back_inserter(result));
  return result;
}

template <typename U16bitIterator, typename OctetIterator>
constexpr auto utf16to8(U16bitIterator start, U16bitIterator end, OctetIterator result) noexcept
{
  while (start != end) {
    uint32_t uc = detail::mask16(*start++);
    // Take care of surrogate pairs first.
    if (detail::is_lead_surrogate(uc)) {
      uint32_t trail_surrogate = detail::mask16(*start++);
      uc = (uc << 10) + trail_surrogate + detail::surrogate_offset;
    }
    result = append(uc, result);
  }
  return result;
}

inline std::string utf16to8(std::u16string_view s) noexcept
{
  std::string result;
  utf16to8(s.begin(), s.end(), std::back_inserter(result));
  return result;
}

template <typename OctetIterator, typename U32bitIterator>
constexpr auto utf8to32(OctetIterator start, OctetIterator end, U32bitIterator result) noexcept
{
  while (start < end) {
    (*result++) = ice::unicode::next(start);
  }
  return result;
}

inline std::u32string utf8to32(std::string_view s) noexcept
{
  std::u32string result;
  utf8to32(s.begin(), s.end(), std::back_inserter(result));
  return result;
}

template <typename OctetIterator, typename U32bitIterator>
constexpr auto utf32to8(U32bitIterator start, U32bitIterator end, OctetIterator result) noexcept
{
  while (start != end) {
    result = append(*(start++), result);
  }
  return result;
}

inline std::string utf32to8(std::u32string_view s) noexcept
{
  std::string result;
  utf32to8(s.begin(), s.end(), std::back_inserter(result));
  return result;
}

ICE_API ice::error_info make_error_info(errc) noexcept;

}  // namespace ice::unicode