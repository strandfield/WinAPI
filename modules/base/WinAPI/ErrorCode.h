// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_ERRORCODE_H
#define WINAPI_ERRORCODE_H

#include <string>

namespace Win32
{

/**
 * \brief represents an error code
 * 
 * This class wraps an error code returned by Windows.h ::GetLastError().
 * 
 * The Value() 0 is used to indicate success (i.e. the absence of error).
 * 
 * Depending on the context, a non-zero error code may also indicate success
 * while providing additional information about how the function performed its task.
 */
class ErrorCode
{
private:
  long m_value = 0;

public:
  ErrorCode() = default;
  ErrorCode(const ErrorCode&) = default;
  ~ErrorCode() = default;

  explicit ErrorCode(long value);

  long Value() const;
  std::string Message() const;

  operator bool() const;

  ErrorCode& operator=(const ErrorCode&) = default;

  bool operator!() const;
};

inline bool operator==(const ErrorCode& lhs, const ErrorCode& rhs)
{
  return lhs.Value() == rhs.Value();
}

inline bool operator!=(const ErrorCode& lhs, const ErrorCode& rhs)
{
  return !(lhs == rhs);
}

ErrorCode GetLastError();

/**
 * \brief constructs an error code given its value
 */
inline ErrorCode::ErrorCode(long value)
  : m_value(value)
{

}

/**
 * \brief returns the value of the error code
 */
inline long ErrorCode::Value() const
{
  return m_value;
}

/**
 * \brief test if there is an error
 */
inline ErrorCode::operator bool() const
{
  return m_value != 0;
}

/**
 * \brief test if there are no error
 */
inline bool ErrorCode::operator!() const
{
  return m_value == 0;
}

} // namespace Win32

#endif // WINAPI_ERRORCODE_H