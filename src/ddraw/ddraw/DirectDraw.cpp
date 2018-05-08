// ----------------------------------------------------------------------------
// DirectDraw.cpp
// ----------------------------------------------------------------------------

#include "DirectDraw.h"
#include "DirectDrawPalette.h"
#include "DirectDrawSurface.h"

#include "main.h"

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// MDirectDraw
// ----------------------------------------------------------------------------
MDirectDraw::MDirectDraw(void)
{
  _debug_print(("MDirectDraw::MDirectDraw [this = 0x%08X]\n", this));
  _ref_count = 1;
}
// ----------------------------------------------------------------------------
MDirectDraw::~MDirectDraw(void)
{
  _debug_print(("MDirectDraw::~MDirectDraw\n"));
  deinit_render();
}
// ----------------------------------------------------------------------------
/*** IUnknown methods ***/
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
  _debug_print(("MDirectDraw::QueryInterface\n"));
  return DD_OK; //m_lpDD->QueryInterface(riid, ppvObj);
}
// ----------------------------------------------------------------------------
ULONG WINAPI MDirectDraw::AddRef(void)
{
  _debug_print(("MDirectDraw::AddRef\n"));

  _ref_count++;

  return _ref_count; //m_lpDD->AddRef();
}
// ----------------------------------------------------------------------------
ULONG WINAPI MDirectDraw::Release(void)
{
  _debug_print(("MDirectDraw::Release\n"));

  int cnt = --_ref_count;

  if (!cnt)
    delete this;

  return cnt; //m_lpDD->Release();
}
// ----------------------------------------------------------------------------
/*** IDirectDraw methods ***/
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::Compact(void)
{
  _debug_print(("MDirectDraw::Compact\n"));
  return DD_OK; //m_lpDD->Compact();
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::CreateClipper(DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter)
{
  _debug_print(("MDirectDraw::CreateClipper\n"));
  return DD_OK; //m_lpDD->CreateClipper(dwFlags, lplpDDClipper, pUnkOuter);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::CreatePalette(DWORD dwFlags, LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE FAR *lplpDDPalette, IUnknown FAR *pUnkOuter)
{
  _debug_print(("MDirectDraw::CreatePalette\n"));
/*
  for (int i = 0; i < 256; i++)
    g_render.palette[i] = lpColorTable[i].peBlue | (lpColorTable[i].peGreen << 8) | (lpColorTable[i].peRed << 16) | 0xff000000;

  if (g_render.palette[255] == 0xff000000)
    g_render.palette[255] = 0xffffffff;

  *lplpDDPalette = new MDirectDrawPalette();
*/
  MDirectDrawPalette* palette = new MDirectDrawPalette();
  palette->SetEntries(dwFlags, 0, 256, lpColorTable);

  *lplpDDPalette = palette;

  return DD_OK; //m_lpDD->CreatePalette(dwFlags, lpColorTable, lplpDDPalette, pUnkOuter);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::CreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE FAR *lplpDDSurface, IUnknown FAR *pUnkOuter)
{
  _debug_print(("MDirectDraw::CreateSurface\n"));

  *lplpDDSurface = new MDirectDrawSurface(lpDDSurfaceDesc);

  // ѕри создании поверхности пропускаем один Flip
  //g_render.state = render_state_skip_flip;

  return DD_OK; //m_lpDD->CreateSurface(lpDDSurfaceDesc, lplpDDSurface, pUnkOuter);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::DuplicateSurface(LPDIRECTDRAWSURFACE lpDDSurface, LPDIRECTDRAWSURFACE FAR *lplpDupDDSurface)
{
  _debug_print(("MDirectDraw::DuplicateSurface\n"));
  return DD_OK; //m_lpDD->DuplicateSurface(lpDDSurface, lplpDupDDSurface);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::EnumDisplayModes(DWORD dwFlags, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext, LPDDENUMMODESCALLBACK lpEnumCallback)
{
  _debug_print(("MDirectDraw::EnumDisplayModes\n"));
  return DD_OK; //m_lpDD->EnumDisplayModes(dwFlags, lpDDSurfaceDesc, lpContext, lpEnumCallback);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::EnumSurfaces(DWORD dwFlags, LPDDSURFACEDESC lpddsd, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumCallback)
{
  _debug_print(("MDirectDraw::EnumSurfaces\n"));
  return DD_OK; //m_lpDD->EnumSurfaces(dwFlags, lpddsd, lpContext, lpEnumCallback);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::FlipToGDISurface(void)
{
  _debug_print(("MDirectDraw::FlipToGDISurface\n"));
  return DD_OK; //m_lpDD->FlipToGDISurface();
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::GetCaps(LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps)
{
  _debug_print(("MDirectDraw::GetCaps\n"));
  return DD_OK; //m_lpDD->GetCaps(lpDDDriverCaps, lpDDHELCaps);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::GetDisplayMode(LPDDSURFACEDESC lpDDSurfaceDesc)
{
  _debug_print(("MDirectDraw::GetDisplayMode\n"));
  return DD_OK; //m_lpDD->GetDisplayMode(lpDDSurfaceDesc);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::GetFourCCCodes(DWORD FAR *lpNumCodes, DWORD FAR *lpCodes)
{
  _debug_print(("MDirectDraw::GetFourCCCodes\n"));
  return DD_OK; //m_lpDD->GetFourCCCodes(lpNumCodes, lpCodes);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::GetGDISurface(LPDIRECTDRAWSURFACE FAR *lplpGDIDDSSurface)
{
  _debug_print(("MDirectDraw::GetGDISurface\n"));
  return DD_OK; //m_lpDD->GetGDISurface(lplpGDIDDSSurface);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::GetMonitorFrequency(LPDWORD lpdwFrequency)
{
  _debug_print(("MDirectDraw::GetMonitorFrequency\n"));
  return DD_OK; //m_lpDD->GetMonitorFrequency(lpdwFrequency);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::GetScanLine(LPDWORD lpdwScanLine)
{
  _debug_print(("MDirectDraw::GetScanLine\n"));
  return DD_OK; //m_lpDD->GetScanLine(lpdwScanLine);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::GetVerticalBlankStatus(LPBOOL lpbIsInVB)
{
  _debug_print(("MDirectDraw::GetVerticalBlankStatus\n"));
  return DD_OK; //m_lpDD->GetVerticalBlankStatus(lpbIsInVB);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::Initialize(GUID FAR *lpGUID)
{
  _debug_print(("MDirectDraw::Initialize\n"));
  return DD_OK; //m_lpDD->Initialize(lpGUID);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::RestoreDisplayMode(void)
{
  _debug_print(("MDirectDraw::RestoreDisplayMode\n"));
  return DD_OK; //m_lpDD->RestoreDisplayMode();
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
  _debug_print(("MDirectDraw::SetCooperativeLevel\n"));

  g_render.hwnd = hWnd;

  return DD_OK; //m_lpDD->SetCooperativeLevel(hWnd, dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwBpp)
{
  _debug_print(("MDirectDraw::SetDisplayMode(%u, %u, %u)\n", dwWidth, dwHeight, dwBpp));

  g_render.width  = dwWidth;
  g_render.height = dwHeight;

  reset_render();

  return DD_OK; //m_lpDD->SetDisplayMode(dwWidth, dwHeight, dwBpp);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDraw::WaitForVerticalBlank(DWORD dwFlags, HANDLE hEvent)
{
  //_debug_print(("MDirectDraw::WaitForVerticalBlank\n"));

  //if (g_render.state == render_state_before_wait_vsync)
  //  g_render.state = render_state_skip_flip;

  return DD_OK; //m_lpDD->WaitForVerticalBlank(dwFlags, hEvent);
}
// ----------------------------------------------------------------------------
