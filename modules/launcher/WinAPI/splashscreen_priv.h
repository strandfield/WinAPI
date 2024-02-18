// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WL_SPLASHSCREEN_PRIV_H
#define WL_SPLASHSCREEN_PRIV_H

#include "SplashScreen.h"

#include <WinAPI/Event.h>

#include <Windows.h>

#include <string>

namespace Win32
{

namespace Impl
{

struct SplashScreenPriv
{
  resource_id splash_image_res_id;
  HWND window_handle = {};
  HBITMAP bitmap;
  Event close_event;
};

} // namespace Impl

} // namespace Win32

#endif // WL_SPLASHSCREEN_PRIV_H