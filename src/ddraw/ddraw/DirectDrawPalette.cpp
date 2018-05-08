// ----------------------------------------------------------------------------
// DirectDrawPalette
// ----------------------------------------------------------------------------

#include "DirectDrawPalette.h"

#include "main.h"

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// MDirectDrawPalette
// ----------------------------------------------------------------------------
MDirectDrawPalette::MDirectDrawPalette(void)
{
  _debug_print(("MDirectDrawPalette::MDirectDrawPalette [this = 0x%08X]\n", this));

  _ref_count = 1;
}
// ----------------------------------------------------------------------------
MDirectDrawPalette::~MDirectDrawPalette(void)
{
  _debug_print(("MDirectDrawPalette::~MDirectDrawPalette\n"));
}
// ----------------------------------------------------------------------------
/*** IUnknown methods ***/
HRESULT WINAPI MDirectDrawPalette::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
  _debug_print(("MDirectDrawPalette::QueryInterface\n"));
  return DD_OK; //m_lpDDPalette->QueryInterface(riid, ppvObj);
}
// ----------------------------------------------------------------------------
ULONG WINAPI MDirectDrawPalette::AddRef(void)
{
  _debug_print(("MDirectDrawPalette::AddRef\n"));

  _ref_count++;

  return _ref_count; //m_lpDDPalette->AddRef();
}
// ----------------------------------------------------------------------------
ULONG WINAPI MDirectDrawPalette::Release(void)
{
  _debug_print(("MDirectDrawPalette::Release\n"));

  int cnt = --_ref_count;

  if (!cnt)
    delete this;

  return cnt; //m_lpDDPalette->Release();
}
// ----------------------------------------------------------------------------
/*** IDirectDrawPalette methods ***/
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawPalette::GetCaps(LPDWORD lpdwCaps)
{
  _debug_print(("MDirectDrawPalette::GetCaps\n"));
  return DD_OK; //m_lpDDPalette->GetCaps(lpdwCaps);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawPalette::GetEntries(DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries)
{
  _debug_print(("MDirectDrawPalette::GetEntries\n"));
  return DD_OK; //m_lpDDPalette->GetEntries(dwFlags, dwBase, dwNumEntries, lpEntries);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawPalette::Initialize(LPDIRECTDRAW lpDD, DWORD dwFlags, LPPALETTEENTRY lpDDColorTable)
{
  _debug_print(("MDirectDrawPalette::Initialize\n"));
  return DD_OK; //m_lpDDPalette->Initialize(lpDD, dwFlags, lpDDColorTable);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawPalette::SetEntries(DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries)
{
  //_debug_print(("MDirectDrawPalette::SetEntries\n"));

  for (DWORD i = dwStartingEntry; i < dwCount; i++)
    g_render.palette[i] = lpEntries[i].peBlue | (lpEntries[i].peGreen << 8) | (lpEntries[i].peRed << 16) | 0xff000000;

  if (g_render.palette[255] == 0xff000000)
    g_render.palette[255] = 0xffffffff;

  return DD_OK; //m_lpDDPalette->SetEntries(dwFlags, dwStartingEntry, dwCount, lpEntries);
}
// ----------------------------------------------------------------------------
