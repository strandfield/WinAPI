// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_ERRORMESSAGE_H
#define WINAPI_ERRORMESSAGE_H

#include <string>

namespace Win32
{

std::string GetErrorMessage(int errorCode);

} // namespace Win32

#endif // WINAPI_ERRORMESSAGE_H