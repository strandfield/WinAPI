// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "ErrorCode.h"

#include "ErrorMessage.h"

#include <Windows.h>

namespace Win32
{

/**
 * \brief returns an error message for the error code
 */
std::string ErrorCode::Message() const
{
  return GetErrorMessage(Value());
}

/**
 * \brief returns the last error
 * 
 * This function wraps the ::GetLastError() function from Windows.h.
 */
ErrorCode GetLastError()
{
  return ErrorCode(::GetLastError());
}

} // namespace Win32