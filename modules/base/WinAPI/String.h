// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_STRING_H
#define WINAPI_STRING_H

#include <string>

namespace Win32
{

std::wstring ToUtf16(const std::string& utf8);
std::string ToUtf8(const std::wstring& utf16);

} // namespace Win32

#endif // WINAPI_STRING_H