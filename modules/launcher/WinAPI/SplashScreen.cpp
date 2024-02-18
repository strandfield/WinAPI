// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "SplashScreen.h"
#include "splashscreen_priv.h"

#include "WinAPI/comptr.h"
#include "WinAPI/ErrorCode.h"
#include "WinAPI/Event.h"

#include <windows.h>
#include <wincodec.h>

#include <cstdlib>
#include <iostream>

namespace Win32
{

namespace Impl
{

using IStreamPtr = com_ptr<IStream>;

IStreamPtr create_stream_on_resource(LPCTSTR lpName, LPCTSTR lpType)
{
  HRSRC hrsrc = FindResource(nullptr, lpName, lpType);
  if (hrsrc == nullptr)
    return {};

  // load the resource
  DWORD dwResourceSize = SizeofResource(nullptr, hrsrc);
  HGLOBAL hglbImage = LoadResource(nullptr, hrsrc);
  if (hglbImage == nullptr)
    return {};

  // lock the resource, getting a pointer to its data
  LPVOID pvSourceResourceData = LockResource(hglbImage);
  if (pvSourceResourceData == nullptr)
    return {};

  // allocate memory to hold the resource data
  HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
  if (hgblResourceData == nullptr)
    return {};

  // get a pointer to the allocated memory
  LPVOID pvResourceData = GlobalLock(hgblResourceData);
  if (pvResourceData == nullptr)
  {
    GlobalFree(hgblResourceData);
    return {};
  }

  // copy the data from the resource to the new memory block
  CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
  GlobalUnlock(hgblResourceData);

  // create a stream on the HGLOBAL containing the data
  IStream* stream = nullptr;
  constexpr bool delete_on_release = true;
  if (FAILED(CreateStreamOnHGlobal(hgblResourceData, delete_on_release, &stream)))
  {
    GlobalFree(hgblResourceData);
  }
  
  return IStreamPtr(stream);
}

using IWICBitmapSourcePtr = com_ptr<IWICBitmapSource>;

// Loads a PNG image from the specified stream (using Windows Imaging Component).
IWICBitmapSourcePtr load_bitmap_from_stream(IStream* ipImageStream)
{
  // load WIC's PNG decoder
  IWICBitmapDecoder* ipDecoder = nullptr;
  if (FAILED(CoCreateInstance(CLSID_WICPngDecoder, nullptr, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder))))
    return {};

  // load the PNG
  if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad)))
  {
    ipDecoder->Release();
    return {};
  }

  // check for the presence of the first frame in the bitmap
  UINT nFrameCount = 0;
  if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount != 1)
  {
    ipDecoder->Release();
    return {};
  }

  // load the first frame (i.e., the image)
  IWICBitmapFrameDecode* ipFrame = nullptr;
  if (FAILED(ipDecoder->GetFrame(0, &ipFrame)))
  {
    ipDecoder->Release();
    return {};
  }

  // convert the image to 32bpp BGRA format with pre-multiplied alpha
  //   (it may not be stored in that format natively in the PNG resource,
  //   but we need this format to create the DIB to use on-screen)
  IWICBitmapSource* ipBitmap = nullptr;
  WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);

  ipFrame->Release();
  ipDecoder->Release();

  return IWICBitmapSourcePtr(ipBitmap);
}

//32 - bit DIB from the specified WIC bitmap.
HBITMAP create_HBITMAP(IWICBitmapSource* ipBitmap)
{
  // initialize return value

  HBITMAP hbmp = nullptr;

  // get image attributes and check for valid image

  UINT width = 0;
  UINT height = 0;
  if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)
    return {};

  // prepare structure giving bitmap information (negative height indicates a top-down DIB)

  BITMAPINFO bminfo;
  ZeroMemory(&bminfo, sizeof(bminfo));
  bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bminfo.bmiHeader.biWidth = width;
  bminfo.bmiHeader.biHeight = -((LONG)height);
  bminfo.bmiHeader.biPlanes = 1;
  bminfo.bmiHeader.biBitCount = 32;
  bminfo.bmiHeader.biCompression = BI_RGB;

  // create a DIB section that can hold the image

  void* pvImageBits = nullptr;
  HDC hdcScreen = GetDC(nullptr);
  hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, nullptr, 0);
  ReleaseDC(nullptr, hdcScreen);
  if (hbmp == nullptr)
    return {};

  // extract the image into the HBITMAP

  const UINT cbStride = width * 4;
  const UINT cbImage = cbStride * height;
  if (FAILED(ipBitmap->CopyPixels(nullptr, cbStride, cbImage, static_cast<BYTE*>(pvImageBits))))
  {
    // couldn't extract image; delete HBITMAP

    DeleteObject(hbmp);
    hbmp = nullptr;
  }

  return hbmp;
}

HBITMAP load_splash_image(const resource_id& res_id)
{
  LPCSTR res_name = res_id.index() == 0 ? MAKEINTRESOURCE(std::get<int>(res_id)) : std::get<std::string>(res_id).c_str();

  // load the PNG image data into a stream
  IStreamPtr image_stream = create_stream_on_resource(res_name, "PNG");
  if (!image_stream)
    return {};

  // load the bitmap with WIC
  IWICBitmapSourcePtr bitmap = load_bitmap_from_stream(image_stream.get());
  if (!bitmap)
    return {};

  // create a HBITMAP containing the image
  return  create_HBITMAP(bitmap.get());
}

constexpr LPCSTR WindowClassName = "SplashWindow";

void register_window_class()
{
  WNDCLASS wc = { 0 };
  wc.lpfnWndProc = DefWindowProc;
  wc.hInstance = GetModuleHandle(nullptr);
  //wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_SPLASHICON));
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = WindowClassName;
  RegisterClass(&wc);
}

HWND create_splash_window()
{
  //HWND hwndOwner = CreateWindow(c_szSplashClass, NULL, WS_POPUP,
  //  0, 0, 0, 0, NULL, NULL, g_hInstance, NULL);
  HWND hwndOwner = 0;
  return CreateWindowEx(WS_EX_LAYERED, WindowClassName, NULL, WS_POPUP | WS_VISIBLE,
    0, 0, 0, 0, hwndOwner, NULL, GetModuleHandle(nullptr), NULL);
}

void set_splash_image(HWND hwndSplash, HBITMAP hbmpSplash)
{
  // get the size of the bitmap

  BITMAP bm;
  GetObject(hbmpSplash, sizeof(bm), &bm);
  SIZE sizeSplash = { bm.bmWidth, bm.bmHeight };

  // get the primary monitor's info

  POINT ptZero = { 0 };
  HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
  MONITORINFO monitorinfo = { 0 };
  monitorinfo.cbSize = sizeof(monitorinfo);
  GetMonitorInfo(hmonPrimary, &monitorinfo);

  // center the splash screen in the middle of the primary work area

  const RECT& rcWork = monitorinfo.rcWork;
  POINT ptOrigin;
  ptOrigin.x = rcWork.left + (rcWork.right - rcWork.left - sizeSplash.cx) / 2;
  ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - sizeSplash.cy) / 2;

  // create a memory DC holding the splash bitmap

  HDC hdcScreen = GetDC(NULL);
  HDC hdcMem = CreateCompatibleDC(hdcScreen);
  HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmpSplash);

  // use the source image's alpha channel for blending

  BLENDFUNCTION blend = { 0 };
  blend.BlendOp = AC_SRC_OVER;
  blend.SourceConstantAlpha = 255;
  blend.AlphaFormat = AC_SRC_ALPHA;

  // paint the window (in the right location) with the alpha-blended bitmap

  UpdateLayeredWindow(hwndSplash, hdcScreen, &ptOrigin, &sizeSplash,
    hdcMem, &ptZero, RGB(0, 0, 0), &blend, ULW_ALPHA);

  // delete temporary objects

  SelectObject(hdcMem, hbmpOld);
  DeleteDC(hdcMem);
  ReleaseDC(NULL, hdcScreen);
}

} // namespace Impl

/**
 * \brief construct a splashscreen from the given resource
 * \param res_id  the id of the resource used for the splashscreen
 * 
 * The resource can either be a PNG or a bitmap image.
 */
SplashScreen::SplashScreen(const resource_id& res_id)
  : d(std::make_unique<Impl::SplashScreenPriv>())
{
  d->splash_image_res_id = res_id;

  Impl::register_window_class();
}

SplashScreen::~SplashScreen()
{

}

bool SplashScreen::CreateCloseEvent(const std::string& name)
{
  try {
    d->close_event = Event::Create(name);
  }
  catch (...) {

  }

  return !d->close_event.IsNull();
}

Event& SplashScreen::GetCloseEvent() const
{
  return d->close_event;
}

/**
 * \brief displays the splash screen
 */
void SplashScreen::Show()
{
  if (!d->window_handle)
  {
    d->window_handle = Impl::create_splash_window();
    d->bitmap = Impl::load_splash_image(d->splash_image_res_id);
    Impl::set_splash_image(d->window_handle, d->bitmap);
  }
}

/**
 * \brief closes the splashscreen
 */
void SplashScreen::Close()
{
  if (d->window_handle)
  {
    CloseWindow(d->window_handle);
  }
}

Impl::SplashScreenPriv* SplashScreen::GetImpl() const
{
  return d.get();
}

/**
 * \brief request the splashscreen of an application to be closed
 * \param appname name of the application
 */
void CloseSplashScreen(const std::string& appname)
{
  const std::string computed_event_name = appname + "CloseSplashScreenEvent";
  const char* event_name = std::getenv("CLOSE_SPLASHSCREEN_EVENT_NAME");
  
  if (!event_name) {
    event_name = computed_event_name.c_str();
  }

  Event e;

  try
  {
    e = Event::Open(event_name);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Event::Open() failed: " << ex.what() << std::endl;
    return;
  }

  if (!e.Set()) {
    // oops
    std::cerr << "SetEvent() failed: " << ErrorCode(::GetLastError()).Message() << std::endl;
  }
}

} // namespace Win32
