// ----------------------------------------------------------------------------
// DirectDrawPalette
// ----------------------------------------------------------------------------

#pragma once

#ifndef _DIRECTDRAWPALETTE_H_
#define _DIRECTDRAWPALETTE_H_

// ----------------------------------------------------------------------------

#include "DirectDraw.h"

// ----------------------------------------------------------------------------

interface MDirectDrawPalette : public IDirectDrawPalette
{
  private:
    unsigned long _ref_count;

  public:
    MDirectDrawPalette(void);
    ~MDirectDrawPalette(void);

    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR * ppvObj);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);
    /*** IDirectDrawPalette methods ***/
    STDMETHOD(GetCaps)(THIS_ LPDWORD);
    STDMETHOD(GetEntries)(THIS_ DWORD, DWORD, DWORD, LPPALETTEENTRY);
    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
    STDMETHOD(SetEntries)(THIS_ DWORD, DWORD,DWORD, LPPALETTEENTRY);
};

// ----------------------------------------------------------------------------

#endif /* _DIRECTDRAWPALETTE_H_ */
