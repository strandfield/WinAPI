// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Exception.h"

namespace Win32
{

Exception::Exception(const ErrorCode& err) 
  : std::runtime_error(err.Message())
{

}

/**
 * \brief returns the error code associated with the exception
 */
const ErrorCode& Exception::GetErrorCode() const
{
  return m_err;
}

} // namespace Win32