// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_PROCESSENVIRONMENT_H
#define WINAPI_PROCESSENVIRONMENT_H

#include <map>
#include <string>
#include <vector>

namespace Win32
{

/**
 * \brief represents the environment variables for a process
 */
class ProcessEnvironment
{
public:
  ProcessEnvironment() = default;
  ProcessEnvironment(const ProcessEnvironment&) = default;
  ProcessEnvironment(ProcessEnvironment&&) = default;
  ~ProcessEnvironment() = default;

  bool Contains(const std::string& name) const;
  void Insert(const std::string& name, const std::string& value);
  void Remove(const std::string& name);
  void Clear();
  bool IsEmpty() const;
  std::vector<std::string> ToStringList() const;

  static ProcessEnvironment GetSystemEnvironment();

  ProcessEnvironment& operator=(const ProcessEnvironment&) = default;
  ProcessEnvironment& operator=(ProcessEnvironment&&) = default;

private:
  std::map<std::string, std::string> m_vars;
};

/**
 * \brief returns whether the environment contains a particular variable
 * \param name  name of the variable
 */
inline bool ProcessEnvironment::Contains(const std::string& name) const
{
  return m_vars.find(name) != m_vars.end();
}

/**
 * \brief sets the value of a variable
 * \param name   the name of the variable
 * \param value  the value
 * 
 * This function overwrites any existing variable with the same \a name.
 */
inline void ProcessEnvironment::Insert(const std::string& name, const std::string& value)
{
  m_vars[name] = value;
}

/**
 * \brief removes a variable
 * 
 * If no such variable exists, this does nothing.
 */
inline void ProcessEnvironment::Remove(const std::string& name)
{
  m_vars.erase(name);
}

/**
 * \brief removes all variables
 * 
 * \sa IsEmpty()
 */
inline void ProcessEnvironment::Clear()
{
  m_vars.clear();
}

/**
 * \brief returns whether there are no variables in this environment
 */
inline bool ProcessEnvironment::IsEmpty() const
{
  return m_vars.empty();
}

/**
 * \brief returns the environment variables as a list of strings
 * 
 * The syntax used for each variable is "name=value", even when the value 
 * is an empty string.
 */
inline std::vector<std::string> ProcessEnvironment::ToStringList() const
{
  std::vector<std::string> r;

  for (const auto& p : m_vars)
  {
    r.push_back(p.first + '=' + p.second);
  }

  return r;
}

} // namespace Win32

#endif // WINAPI_PROCESSENVIRONMENT_H