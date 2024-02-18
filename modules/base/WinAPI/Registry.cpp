// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Registry.h"

#include "String.h"

namespace Win32
{
static RegistryKey CreatePredefinedRegKey(const std::string& name);

const RegistryKey Registry::HKEY_CLASSES_ROOT = CreatePredefinedRegKey("HKEY_CLASSES_ROOT");
const RegistryKey Registry::HKEY_CURRENT_CONFIG = CreatePredefinedRegKey("HKEY_CURRENT_CONFIG");
const RegistryKey Registry::HKEY_CURRENT_USER = CreatePredefinedRegKey("HKEY_CURRENT_USER");
const RegistryKey Registry::HKEY_LOCAL_MACHINE = CreatePredefinedRegKey("HKEY_LOCAL_MACHINE");
const RegistryKey Registry::HKEY_USERS = CreatePredefinedRegKey("HKEY_USERS");
}

#include "WindowsRegistry.h"
#include "registry_priv.h"

#include "Exception.h"

#include <map>

namespace Win32
{

/**
 * \brief opens a registry key
 * \param key           parent key
 * \param subKey        name of the subkey
 * \param accessRights  specifies whether the key should be writable or read-only
 * \throw Exception on failure
 * 
 * Opening fails if the key does not exist or if the application does not have the 
 * sufficient rights for opening the key (e.g., with write access).
 */
RegistryKey Registry::OpenKey(const RegistryKey& key, const std::string& subKey, Registry::AccessRights accessRights)
{
  RegistryKey rk;
  rk.Open(key, subKey, accessRights);
  return rk;
}

/**
 * \brief creates a registry key
 * \param      key           parent key
 * \param      subKey        name of the subkey to create
 * \param      accessRights  access rights on the returned key
 * \param[out] created       receives whether the key was actually created (can be nullptr)
 * \throw Exception on failure
 */
RegistryKey Registry::CreateKey(const RegistryKey& key, const std::string& subKey, AccessRights accessRights, bool* created)
{
  constexpr DWORD reserved = 0;
  std::wstring wsubKey = ToUtf16(subKey);
  constexpr LPWSTR kclass = nullptr;
  DWORD options = 0;
  SECURITY_ATTRIBUTES* secattrs = nullptr;
  Impl::RegistryKeyPriv rkp;
  DWORD disposition = 0;

  LSTATUS status = ::RegCreateKeyExW(
    GetHKEY(key), 
    wsubKey.c_str(), 
    reserved, 
    kclass, 
    options, 
    static_cast<REGSAM>(accessRights), 
    secattrs, 
    &rkp.hkey, 
    &disposition);

  if (status != ERROR_SUCCESS) {
    throw Exception(ErrorCode(status));
  }

  if (created) {
    *created = (disposition == REG_CREATED_NEW_KEY);
  }

  return RegistryKey(std::make_unique<Impl::RegistryKeyPriv>(rkp));
}

/**
 * \brief delete a registry key
 * \param key     parent key
 * \param subKey  name of the subkey to delete
 * \throw Exception on failure
 */
void Registry::DeleteKey(const RegistryKey& key, const std::string& subKey)
{
  std::wstring wsubKey = ToUtf16(subKey);
  LSTATUS status = ::RegDeleteKeyW(GetHKEY(key), wsubKey.c_str());

  if (status != ERROR_SUCCESS) {
    throw Exception(ErrorCode(status));
  }
}

/** 
 * \brief constructs a null key
 * 
 * Use Registry::OpenKey() or Registry::CreateKey() to get a valid key.
 * 
 * \sa IsNull().
 */
RegistryKey::RegistryKey()
{

}

RegistryKey::RegistryKey(std::unique_ptr<Impl::RegistryKeyPriv> dPtr)
  : d(std::move(dPtr))
{


}

/**
 * \brief destroys the key
 * 
 * This closes the key if it wasn't already closed.
 * 
 * \sa Close().
 */
RegistryKey::~RegistryKey()
{
  Close();
}

/**
 * \brief returns whether the key is null
 */
bool RegistryKey::IsNull() const
{
  return !d;
}

/**
 * \brief open another registry key
 * \param key           parent key
 * \param subKey        name of the subkey to open
 * \param accessRights  specifies whether the key should be writable or read-only
 * 
 * If this object is a valid key, it is closed first before attempting to open
 * the new key.
 * 
 * This function returns a non-zero error code on failure.
 */
ErrorCode RegistryKey::TryOpen(const RegistryKey& key, const std::string& subKey, Registry::AccessRights accessRights)
{
  if (!IsNull()) {
    Close();
  }

  constexpr DWORD options = 0;
  std::wstring wsubKey = ToUtf16(subKey);
  Impl::RegistryKeyPriv rk;
  rk.predefined = false;
  LSTATUS status = ::RegOpenKeyExW(
    GetHKEY(key),
    wsubKey.c_str(),
    options,
    static_cast<REGSAM>(accessRights),
    &rk.hkey);

  if (status == ERROR_SUCCESS)
  {
    d = std::make_unique<Impl::RegistryKeyPriv>(rk);
  }

  return ErrorCode{ status };
}

/**
* \brief open another registry key
* \param key           parent key
* \param subKey        name of the subkey to open
* \param accessRights  specifies whether the key should be writable or read-only
* \throw Exception on failure
* 
* This function uses TryOpen() and throws an exception if it returned a non-zero 
* error code.
*/
void RegistryKey::Open(const RegistryKey& key, const std::string& subKey, Registry::AccessRights accessRights)
{
  ErrorCode err = TryOpen(key, subKey, accessRights);

  if (err)
  {
    throw Exception(err);
  }
}

/**
 * \brief closes the key
 * 
 * The key becomes null after this.
 * 
 * \sa IsNull().
 */
void RegistryKey::Close()
{
  if (d && !d->predefined) {
    RegCloseKey(d->hkey);
    d.reset();
  }
}

/**
 * \brief sets an integer value to the registry key
 * \param name  the name of the value
 * \param value the value
 * 
 * The value is stored in the registry as a REG_DWORD.
 * 
 * For this function to succeed, the key must have been opened
 * with write access.
 */
void RegistryKey::SetValue(const std::string& name, int value)
{
  std::wstring wname = ToUtf16(name);

  LSTATUS status = ::RegSetValueExW(
    GetHKEY(*this),
    wname.c_str(),
    0,
    REG_DWORD,
    reinterpret_cast<const BYTE*>(&value),
    sizeof(DWORD));
}

/**
* \brief sets a string value to the registry key
* \param name  the name of the value
* \param value the value
* 
* The value is stored in the registry as a REG_SZ.
* 
* For this function to succeed, the key must have been opened
* with write access.
*/
void RegistryKey::SetValue(const std::string& name, const std::string& value)
{
  std::wstring wname = ToUtf16(name);
  std::wstring wvalue = ToUtf16(value);

  LSTATUS status = ::RegSetValueExW(
    GetHKEY(*this),
    wname.c_str(),
    0,
    REG_SZ,
    reinterpret_cast<const BYTE*>(wvalue.data()),
    static_cast<DWORD>(sizeof(std::wstring::value_type) * (wvalue.size() + 1)));
}

/**
 * \brief reads an integer value from the registry key
 * \param name  the name of the value
 */
int RegistryKey::GetIntValue(const std::string& name) const
{
  std::wstring wname = ToUtf16(name);
  DWORD type;
  DWORD result;
  DWORD size = sizeof(DWORD);

  LSTATUS status = ::RegGetValueW(
    GetHKEY(*this), 
    nullptr,
    wname.c_str(), 
    RRF_RT_REG_DWORD,
    &type,
    reinterpret_cast<void*>(&result),
    &size);

  return result;
}

/**
* \brief reads a string value from the registry key
* \param name  the name of the value
*/
std::string RegistryKey::GetStringValue(const std::string& name) const
{
  std::wstring wname = ToUtf16(name);
  DWORD type;
  DWORD size = 0;

  // a first call is performed to retrieve the size of the string
  LSTATUS status = ::RegGetValueW(
    GetHKEY(*this),
    nullptr,
    wname.c_str(),
    RRF_RT_REG_SZ,
    &type,
    nullptr,
    &size);

  if (status != ERROR_SUCCESS) {
    throw Exception(ErrorCode(status));
  }

  std::wstring result;
  result.resize(size);

  // a second call actually retrieves the string
  status = ::RegGetValueW(
    GetHKEY(*this),
    nullptr,
    wname.c_str(),
    RRF_RT_REG_SZ,
    &type,
    reinterpret_cast<void*>(result.data()),
    &size);

  if (size > 0) {
    result.resize(size - 1);
  } else {
    result.clear();
  }
  
  return ToUtf8(result);
}

Impl::RegistryKeyPriv* RegistryKey::GetImpl() const
{
  return d.get();
}

RegistryKey CreatePredefinedRegKey(const std::string& name)
{
  static const std::map<std::string, HKEY> dict = {
    {"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT},
    {"HKEY_CURRENT_CONFIG", HKEY_CURRENT_CONFIG},
    {"HKEY_CURRENT_USER", HKEY_CURRENT_USER},
    {"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE},
    {"HKEY_USERS", HKEY_USERS}
  };

  Impl::RegistryKeyPriv rk;
  rk.hkey = dict.at(name);
  rk.predefined = true;

  return RegistryKey(std::make_unique<Impl::RegistryKeyPriv>(rk));
}

HKEY GetHKEY(const RegistryKey& rk)
{
  return rk.IsNull() ? nullptr : rk.GetImpl()->hkey;
}

} // namespace Win32