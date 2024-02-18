// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_EVENT_H
#define WINAPI_EVENT_H

#include <memory>
#include <string>

namespace Win32
{

namespace Impl
{
struct EventPriv;
} // namespace Impl

/**
 * \brief represents an event
 * 
 * Events provide a mean of communication between processes.
 */
class Event
{
public:
  Event() noexcept;
  Event(const Event&) = delete;
  Event(Event&&) noexcept = default;
  ~Event();

  explicit Event(const std::string& eventName);

  explicit Event(std::unique_ptr<Impl::EventPriv> dPtr);

  bool IsNull() const;

  static Event Open(const std::string& eventName);
  static Event Create(const std::string& eventName);

  bool Created() const;
  const std::string& GetName() const;
  
  bool Set();
  void Close();

  Event& operator=(const Event&) = delete;
  Event& operator=(Event&&);

  Impl::EventPriv* GetImpl() const;

private:
  std::unique_ptr<Impl::EventPriv> d;
};

} // namespace Win32

#endif // WINAPI_EVENT_H