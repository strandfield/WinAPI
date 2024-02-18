// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_WINDOWSREGISTRY_H
#define WINAPI_WINDOWSREGISTRY_H

#include "Registry.h"

#include <Windows.h>
#include <winreg.h>

namespace Win32
{

HKEY GetHKEY(const RegistryKey& rk);

} // namespace Win32

#endif // WINAPI_WINDOWSREGISTRY_H