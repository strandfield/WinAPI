// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_REGISTRY_H
#define WINAPI_REGISTRY_H

#ifdef WINAPI_REGISTRYPRIV_H
#error "Registry.h must be included before registry_priv.h"
#endif

#include <memory>
#include <string>

namespace Win32
{

class ErrorCode;

namespace Impl
{
struct RegistryKeyPriv;
} // namespace Impl

class RegistryKey;

/**
 * \brief provides access to registry keys
 * 
 * This class provides static member functions for creating, opening and deleting registry keys.
 */
class Registry
{
public:
  static const RegistryKey HKEY_CLASSES_ROOT;
  static const RegistryKey HKEY_CURRENT_CONFIG;
  static const RegistryKey HKEY_CURRENT_USER;
  static const RegistryKey HKEY_LOCAL_MACHINE;
  static const RegistryKey HKEY_USERS;

  enum AccessRights
  {
    Read = 0x20019,
    Write = 0x20006,
  };

  static RegistryKey OpenKey(const RegistryKey& key, const std::string& subKey, AccessRights accessRights);
  static RegistryKey CreateKey(const RegistryKey& key, const std::string& subKey, AccessRights accessRights, bool* created = nullptr);
  static void DeleteKey(const RegistryKey& key, const std::string& subKey);
};

/**
 * \brief represents a registry key
 * 
 * This class is move-only and its default constructor builds a null key (see IsNull()).
 * You can get a valid key by calling Registry::OpenKey() or Registry::CreateKey().
 * The destructor automatically closes the key (see Close()).
 * 
 * This class provides functions for setting and reading values associated with the key.
 */
class RegistryKey
{
public:
  RegistryKey();
  RegistryKey(const RegistryKey&) = delete;
  RegistryKey(RegistryKey&&) = default;
  ~RegistryKey();

  explicit RegistryKey(std::unique_ptr<Impl::RegistryKeyPriv> dPtr);

  bool IsNull() const;

  void Open(const RegistryKey& key, const std::string& subKey, Registry::AccessRights accessRights);
  ErrorCode TryOpen(const RegistryKey& key, const std::string& subKey, Registry::AccessRights accessRights);
  void Close();

  void SetValue(const std::string& name, int value);
  void SetValue(const std::string& name, const std::string& value);
  int GetIntValue(const std::string& name) const;
  std::string GetStringValue(const std::string& name) const;

  RegistryKey& operator=(const RegistryKey&) = delete;
  RegistryKey& operator=(RegistryKey&&) = default;

  Impl::RegistryKeyPriv* GetImpl() const;

private:
  std::unique_ptr<Impl::RegistryKeyPriv> d;
};

} // namespace Win32

#endif // WINAPI_REGISTRY_H