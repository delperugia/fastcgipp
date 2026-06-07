/*!
 * @file       wstringconvert.hpp
 * @brief      Replacement for std::wstring_convert
 * @date       June 6, 2026
 */

#pragma once

#include <string>

//! Topmost namespace for the fastcgi++ library
namespace Fastcgipp
{
    /*!
     * This class is a simplified proxy to the now obsolete std::wstring_convert.
     * Throws std::range_error on conversion or range errors.
    */
    class WstringConvert
    {
    public:
        std::wstring from_bytes(const char *first, const char *last); // [first, last)
        std::wstring from_bytes(const std::string &str);
        std::string  to_bytes(const std::wstring &wstr);
    };
}
