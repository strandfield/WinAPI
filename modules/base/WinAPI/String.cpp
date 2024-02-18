// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "String.h"

#include <Windows.h>
#include <stringapiset.h>

namespace Win32
{

/**
 * \brief performs utf8 to utf16 conversion
 */
std::wstring ToUtf16(const std::string& utf8)
{
  constexpr DWORD flags = 0;

  auto result = std::wstring(utf8.size() + 1, '\0');

  int charsWritten = ::MultiByteToWideChar(
    CP_UTF8,
    flags,
    utf8.c_str(),
    static_cast<int>(utf8.size()),
    result.data(),
    static_cast<int>(result.size())
  );

  result.resize(charsWritten);

  return result;
}

/**
* \brief performs utf16 to utf8 conversion
*/
std::string ToUtf8(const std::wstring& utf16)
{
  constexpr DWORD flags = 0;
  constexpr LPCCH defaultChar = nullptr;
  constexpr LPBOOL usedDefaultChar = nullptr;

  auto result = std::string(utf16.size() * 4, '\0');
  
  int bytesWritten = ::WideCharToMultiByte(
    CP_UTF8,
    flags,
    utf16.data(),
    static_cast<int>(utf16.size()),
    result.data(),
    static_cast<int>(result.size()),
    defaultChar,
    usedDefaultChar);

  result.resize(bytesWritten);

  return result;
}

} // namespace Win32