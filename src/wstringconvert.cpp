/*!
 * @file       wstringconvert.cpp
 * @brief      Replacement for std::wstring_convert
 * @date       June 6, 2026
 */

#include "fastcgi++/wstringconvert.hpp"

#include <cerrno>
#include <iconv.h>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

//! Topmost namespace for the fastcgi++ library
namespace Fastcgipp
{
    template <typename ToChar, typename FromChar>
    std::optional<std::basic_string<ToChar>>
    iconv_convert(const std::basic_string<FromChar> & input,
                  const char *                        to_encoding,
                  const char *                        from_encoding)
    {
        if (input.empty())
            return std::basic_string<ToChar>();
        //
        if (input.size() >= std::numeric_limits<size_t>::max() / (4 * sizeof(ToChar))) // out_bytes_max below would overflow
            return std::nullopt;
        //
        size_t in_bytes_left  = input.size() * sizeof(FromChar);
        size_t out_bytes_max  = input.size() * (4 * sizeof(ToChar)); // worst case
        size_t out_bytes_left = out_bytes_max;
        //
        std::vector<char> output(out_bytes_max); // allocate outpout buffer
        //
        iconv_t cd = iconv_open(to_encoding, from_encoding);
        if (cd == (iconv_t)-1)
            return std::nullopt;
        //
        char * input_reader  = const_cast<char*>(reinterpret_cast<const char*>(input.data()));
        char * output_writer = output.data();
        //
        size_t res = iconv(cd, &input_reader, &in_bytes_left, &output_writer, &out_bytes_left);
        iconv_close(cd);
        //
        if (res == (size_t)-1)
            return std::nullopt;
        //
        size_t bytes_written = out_bytes_max - out_bytes_left;
        size_t chars_written = bytes_written / sizeof(ToChar);
        //
        return std::basic_string<ToChar>(reinterpret_cast<const ToChar*>(output.data()), chars_written); // resized to actual output size
    }

    std::wstring WstringConvert::from_bytes(const char *first, const char *last)
    {
        if (last < first)
            throw std::range_error("from_bytes invalid range");
        //
        auto ws = iconv_convert<wchar_t, char>(std::string(first, last), "WCHAR_T", "UTF-8"); // std::string uses [first, last) too
        if (ws.has_value())
            return ws.value();
        else
            throw std::range_error("from_bytes conversion failed");
    }

    std::wstring WstringConvert::from_bytes(const std::string &str)
    {
        auto ws = iconv_convert<wchar_t, char>(str, "WCHAR_T", "UTF-8");
        if (ws.has_value())
            return ws.value();
        else
            throw std::range_error("from_bytes conversion failed");
    }

    std::string WstringConvert::to_bytes(const std::wstring &wstr)
    {
        auto s = iconv_convert<char, wchar_t>(wstr, "UTF-8", "WCHAR_T");
        if (s.has_value())
            return s.value();
        else
            throw std::range_error("to_bytes conversion failed");
    }
}
