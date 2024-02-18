// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_EXCEPTION_H
#define WINAPI_EXCEPTION_H

#include "ErrorCode.h"

#include <stdexcept>

namespace Win32
{

/**
 * \brief base class for the exception thrown by this library
 */
class Exception : public std::runtime_error
{
private:
  ErrorCode m_err;

public:
  explicit Exception(const ErrorCode& err);

  const ErrorCode& GetErrorCode() const;
};

} // namespace Win32

#endif // WINAPI_EXCEPTION_H