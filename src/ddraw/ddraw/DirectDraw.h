// ----------------------------------------------------------------------------
// DirectDraw
// ----------------------------------------------------------------------------

#pragma once

#ifndef _DIRECTDRAW_H_
#define _DIRECTDRAW_H_

// ----------------------------------------------------------------------------

#include "ddraw.h"
//#include "main.h"

// ----------------------------------------------------------------------------

interface MDirectDraw : public IDirectDraw
{
  private:
    unsigned long _ref_count;

  public:
    MDirectDraw(void);
    ~MDirectDraw(void);

    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR * ppvObj);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);
    /*** IDirectDraw methods ***/
    STDMETHOD(Compact)(THIS);
    STDMETHOD(CreateClipper)(THIS_ DWORD, LPDIRECTDRAWCLIPPER FAR *, IUnknown FAR *);
    STDMETHOD(CreatePalette)(THIS_ DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR *, IUnknown FAR *);
    STDMETHOD(CreateSurface)(THIS_ LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR *, IUnknown FAR *);
    STDMETHOD(DuplicateSurface)(THIS_ LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR *);
    STDMETHOD(EnumDisplayModes)(THIS_ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK);
    STDMETHOD(EnumSurfaces)(THIS_ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMSURFACESCALLBACK);
    STDMETHOD(FlipToGDISurface)(THIS);
    STDMETHOD(GetCaps)(THIS_ LPDDCAPS, LPDDCAPS);
    STDMETHOD(GetDisplayMode)(THIS_ LPDDSURFACEDESC);
    STDMETHOD(GetFourCCCodes)(THIS_ LPDWORD, LPDWORD);
    STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE FAR *);
    STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD);
    STDMETHOD(GetScanLine)(THIS_ LPDWORD);
    STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL);
    STDMETHOD(Initialize)(THIS_ GUID FAR *);
    STDMETHOD(RestoreDisplayMode)(THIS);
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD);
    STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD, DWORD);
    STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE);
};

// ----------------------------------------------------------------------------

#endif /* _DIRECTDRAW_H_ */
