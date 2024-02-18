// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Event.h"
#include "EventImpl.h"

#include "Exception.h"
#include "String.h"

namespace Win32
{

Event::Event() noexcept
  : d(nullptr)
{

}

Event::~Event()
{
  Close();
  d.reset();
}

/**
 * \brief create or open an event
 * \param eventName  the name of the event
 * \throw Exception on failure
 * 
 * Use the Created() function to check whether this constructor actually 
 * created the event or only opened it.
 */
Event::Event(const std::string& eventName)
  : d(std::make_unique<Impl::EventPriv>())
{
  d->name = eventName;
  std::wstring weventName = ToUtf16(eventName);

  constexpr bool manual_reset = true;
  constexpr bool initial_state = false;

  d->handle = ::CreateEventW(nullptr, manual_reset, initial_state, weventName.c_str());

  if (!d->handle) {
    throw Exception(GetLastError());
  }

  d->created = ::GetLastError() != ERROR_ALREADY_EXISTS;
}

Event::Event(std::unique_ptr<Impl::EventPriv> dPtr)
  : d(std::move(dPtr))
{

}

/**
 * \brief returns whether this object does not represent a valid event
 */
bool Event::IsNull() const
{
  return !d;
}

/**
 * \brief open an event
 * \param eventName  the name of the event
 * \return the opened event
 * \throw Exception on failure
 * 
 * This function will fail if the event does not exist.
 */
Event Event::Open(const std::string& eventName)
{
  auto dptr = std::make_unique<Impl::EventPriv>();
  dptr->name = eventName;
  std::wstring weventName = ToUtf16(eventName);

  constexpr DWORD desired_access = EVENT_MODIFY_STATE;
  constexpr bool inherit_handle = false;

  Impl::EventPriv data;

  dptr->handle = ::OpenEventW(desired_access, inherit_handle, weventName.c_str());

  if (!dptr->handle) {
    throw Exception(Win32::GetLastError());
  }

  return Event{ std::move(dptr) };
}

/**
 * \brief create an event
 * \param eventName  the name of the event
 * \return the created event
 * \throw Exception on failure
 * 
 * This function will fail if the event already exists.
 */
Event Event::Create(const std::string& eventName)
{
  auto dptr = std::make_unique<Impl::EventPriv>();
  dptr->name = eventName;
  std::wstring weventName = ToUtf16(eventName);

  constexpr bool manual_reset = true;
  constexpr bool initial_state = false;

  dptr->handle = ::CreateEventW(nullptr, manual_reset, initial_state, weventName.c_str());

  if (dptr->handle) {
    dptr->created = ::GetLastError() != ERROR_ALREADY_EXISTS;
  }

  if (!dptr->handle || !dptr->created) {
    if (dptr->handle) {
      ::CloseHandle(dptr->handle);
    }

    throw Exception(Win32::GetLastError());
  }

  return Event{ std::move(dptr) };
}

/**
 * \brief returns whether the event was created by this reference to the event
 */
bool Event::Created() const
{
  return d && d->created;
}

/**
 * \brief returns the name of the event
 */
const std::string& Event::GetName() const
{
  if (!d) {
    static const std::string emptyName = "";
    return emptyName;
  } else {
    return d->name;
  }
}

/**
 * \brief sets the event to its 'set' state
 */
bool Event::Set()
{
  return d && ::SetEvent(d->handle);
}

/**
 * \brief close the event
 */
void Event::Close()
{
  if (d) {
    ::CloseHandle(d->handle);
    d.reset();
  }
}

Event& Event::operator=(Event&& other)
{
  if (d && d != other.d) {
    Close();
  }

  d = std::move(other.d);

  return *this;
}

Impl::EventPriv* Event::GetImpl() const
{
  return d.get();
}

HANDLE GetHANDLE(const Event& e)
{
  return e.GetImpl() ? e.GetImpl()->handle : nullptr;
}

} // namespace Win32