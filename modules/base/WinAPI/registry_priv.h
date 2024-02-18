// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_REGISTRYPRIV_H
#define WINAPI_REGISTRYPRIV_H

#include <Windows.h>
#include <winreg.h>

namespace Win32
{

namespace Impl
{

struct RegistryKeyPriv
{
  HKEY hkey = nullptr;
  bool predefined = false;
};

} // namespace Impl

} // namespace Win32

#endif // WINAPI_REGISTRYPRIV_H