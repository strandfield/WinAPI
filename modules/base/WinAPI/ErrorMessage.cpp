// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "ErrorMessage.h"

#include "String.h"

#include <Windows.h>

namespace Win32
{

std::string GetErrorMessage(int errorCode)
{
  constexpr LPCVOID source = nullptr;
  constexpr DWORD langid = 0;
  constexpr DWORD size = 0;
  constexpr va_list* vaargs = nullptr;

  LPWSTR buffer = nullptr;

  ::FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    source,
    errorCode,
    langid,
    (LPWSTR) &buffer,
    size, vaargs);

  auto str = std::wstring(buffer);
  ::LocalFree(buffer);

  return ToUtf8(str);
}

} // namespace Win32