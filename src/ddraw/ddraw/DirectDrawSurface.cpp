// ----------------------------------------------------------------------------
// DirectDrawSurface
// ----------------------------------------------------------------------------

#include "DirectDrawSurface.h"
#include "DirectDrawPalette.h"

#include "main.h"

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// MDirectDrawSurface
// ----------------------------------------------------------------------------
MDirectDrawSurface::MDirectDrawSurface(LPDDSURFACEDESC lpDDSurfaceDesc)
{
  //_debug_print(("MDirectDrawSurface::MDirectDrawSurface [this = 0x%08X]\n", this));

  _ref_count = 1;

  _caps = lpDDSurfaceDesc->ddsCaps.dwCaps;

  _primary = !!(_caps & DDSCAPS_PRIMARYSURFACE);
  _flipped = false;

  //if (_caps == (DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE))
  if (_caps & DDSCAPS_PRIMARYSURFACE)
  {
    _width  = g_render.width;
    _height = g_render.height;
  }
  //else if ((_caps == DDSCAPS_OFFSCREENPLAIN) || (Caps == (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY)))
  else if (_caps & DDSCAPS_OFFSCREENPLAIN)
  {
    _width  = lpDDSurfaceDesc->dwWidth;
    _height = lpDDSurfaceDesc->dwHeight;
  }
  else
  {
    _width  = 0;
    _height = 0;
  }

  int size = _width * _height;

  _buf = size ? (new unsigned char [size]) : NULL;
}
// ----------------------------------------------------------------------------
MDirectDrawSurface::~MDirectDrawSurface(void)
{
  //_debug_print(("MDirectDrawSurface::~MDirectDrawSurface\n"));

  if (_buf)
  {
    delete [] _buf;
    _buf = NULL;
  }
}
// ----------------------------------------------------------------------------
/*** IUnknown methods ***/
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
  //_debug_print(("MDirectDrawSurface::QueryInterface\n"));

  if (riid == IID_IDirectDrawSurface2)
  {
    //_ref_count++;
    //*ppvObj = this;
    *ppvObj = new MDirectDrawSurface2();
    //_dd2->AddRef();
    //*ppvObj = _dd2;
  }

  return DD_OK; //m_lpDDSurface->QueryInterface(riid, ppvObj);
}
// ----------------------------------------------------------------------------
ULONG WINAPI MDirectDrawSurface::AddRef(void)
{
  //_debug_print(("MDirectDrawSurface::AddRef\n"));

  _ref_count++;

  return _ref_count; //return m_lpDDSurface->AddRef();
}
// ----------------------------------------------------------------------------
ULONG WINAPI MDirectDrawSurface::Release(void)
{
  //_debug_print(("MDirectDrawSurface::Release [this = 0x%08X]\n", this));

  int cnt = --_ref_count;

  if (!cnt)
    delete this;

  return cnt; //m_lpDDSurface->Release();
}
// ----------------------------------------------------------------------------
/*** IDirectDrawSurface methods ***/
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::AddAttachedSurface(LPDIRECTDRAWSURFACE lpDDSAttachedSurface)
{
  _debug_print(("MDirectDrawSurface::AddAttachedSurface\n"));
  return DD_OK; //m_lpDDSurface->AddAttachedSurface(lpDDSAttachedSurface);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::AddOverlayDirtyRect(LPRECT lpRect)
{
  _debug_print(("MDirectDrawSurface::AddOverlayDirtyRect\n"));
  return DD_OK; //m_lpDDSurface->AddOverlayDirtyRect(lpRect);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
  //_debug_print(("MDirectDrawSurface::Blt [this = 0x%08X, lpDDSrcSurface = 0x%08X]\n", this, lpDDSrcSurface));
  //_debug_print(("MDirectDrawSurface::Blt [this = 0x%08X, lpDDSrcSurface = 0x%08X, lpSrcRect = 0x%08X, dwFlags =  0x%08X]\n",
  //  this, lpDDSrcSurface, lpSrcRect, dwFlags));

  if (!lpDDSrcSurface && _buf)
    memset(_buf, 0, _width * _height);

  return DD_OK; //m_lpDDSurface->Blt(lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::BltBatch(LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags)
{
  _debug_print(("MDirectDrawSurface::BltBatch\n"));
  return DD_OK; //m_lpDDSurface->BltBatch(lpDDBltBatch, dwCount, dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags)
{
  //if (lpDDSrcSurface == this)
    //return DD_OK;

  //_debug_print(("MDirectDrawSurface::BltFast [this = 0x%08X, lpDDSrcSurface = 0x%08X, dwX = %u, dwY = %u, lpSrcRect = (%d,%d,%d,%d), dwFlags = 0x%08X]\n",
  //  this, lpDDSrcSurface, dwX, dwY, lpSrcRect->left, lpSrcRect->top, lpSrcRect->right, lpSrcRect->bottom, dwFlags));

  if (lpDDSrcSurface == this)
  {
    int row_size = lpSrcRect->right - lpSrcRect->left;

    for (int y = lpSrcRect->top; y < lpSrcRect->bottom; y++, dwY++)
    {
      unsigned char* dst = static_cast<unsigned char*>(_buf) + dwY * _width + dwX;
      unsigned char* src = static_cast<unsigned char*>(_buf) + y * _width + lpSrcRect->left;

      memcpy(dst, src, row_size);
    }
  }
  else
  {
    //_debug_print(("MDirectDrawSurface::BltFast [this = 0x%08X, lpDDSrcSurface = 0x%08X, dwX = %u, dwY = %u, lpSrcRect = (%d,%d,%d,%d), dwFlags = 0x%08X]\n",
    //  this, lpDDSrcSurface, dwX, dwY, lpSrcRect->left, lpSrcRect->top, lpSrcRect->right, lpSrcRect->bottom, dwFlags));

    // Отрисовка курсора
    //if ((lpSrcRect->left == 0) && (lpSrcRect->top == 0) && (lpSrcRect->right == 32) && (lpSrcRect->bottom == 32))
      //g_render.state = render_state_before_wait_vsync;

    MDirectDrawSurface* surface = (MDirectDrawSurface*)lpDDSrcSurface;

    if (surface)
    {
      bool use_colorkey = !!(dwFlags & DDBLTFAST_SRCCOLORKEY);

      //if (!use_colorkey && (lpSrcRect->left == 0) && (lpSrcRect->top == 0) && (lpSrcRect->right == _width) && (lpSrcRect->bottom == _height))
      //if (!use_colorkey &&
      //  (((lpSrcRect->left == 0) && (lpSrcRect->top == 0) && (lpSrcRect->right == _width) && (lpSrcRect->bottom == _height)) ||
      //  ((!_flipped && surface->_width == _width && surface->_height == _height))))
      if (!use_colorkey && _flipped && surface->_width == _width && surface->_height == _height)
      {
        memcpy(_buf, surface->_buf, _width * _height);
      }
      else
      {
        int y_dst = dwY;

        for (int y = lpSrcRect->top; y < lpSrcRect->bottom; y++)
        {
          if (y_dst >= _height)
            break;

          unsigned char* row_dst = static_cast<unsigned char*>(_buf) + y_dst * _width;
          unsigned char* row_src = static_cast<unsigned char*>(surface->_buf) + y * surface->_width;

          if (!use_colorkey)
          {
            memcpy(row_dst + dwX, row_src + lpSrcRect->left, lpSrcRect->right - lpSrcRect->left);
          }
          else
          {
            int x_dst = dwX;

            for (int x = lpSrcRect->left; x < lpSrcRect->right; x++)
            {
              if (x_dst >= _width)
                break;

              if (row_src[x])
                row_dst[x_dst] = row_src[x];

              x_dst++;
            }
          }

          y_dst++;
        }
      }
    }

    _flipped = false;
  }

  return DD_OK; //m_lpDDSurface->BltFast(dwX, dwY, lpDDSrcSurface, lpSrcRect, dwTrans);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSAttachedSurface)
{
  _debug_print(("MDirectDrawSurface::DeleteAttachedSurface\n"));
  return DD_OK; //m_lpDDSurface->DeleteAttachedSurface(dwFlags, lpDDSAttachedSurface);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::EnumAttachedSurfaces(LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback)
{
  _debug_print(("MDirectDrawSurface::EnumAttachedSurfaces\n"));
  return DD_OK; //m_lpDDSurface->EnumAttachedSurfaces(lpContext, lpEnumSurfacesCallback);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::EnumOverlayZOrders(DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback)
{
  _debug_print(("MDirectDrawSurface::EnumOverlayZOrders\n"));
  return DD_OK; //m_lpDDSurface->EnumOverlayZOrders(dwFlags, lpContext, lpfnCallback);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::Flip(LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface::Flip [this = 0x%08X, dwFlags = 0x%08X]\n", this, dwFlags));

  //if (g_render.state != render_state_skip_flip)
  {
    update_screen(_buf);

    _flipped = true;

    Sleep(1);
  }

  //g_render.state = render_state_default;

  return DD_OK;
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetAttachedSurface(LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE FAR *lplpDDAttachedSurface)
{
  _debug_print(("MDirectDrawSurface::GetAttachedSurface [this = 0x%08X]\n", this));

  *lplpDDAttachedSurface = this;

  return DD_OK;
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetBltStatus(DWORD dwFlags)
{
  _debug_print(("MDirectDrawSurface::GetBltStatus [this = 0x%08X]\n"));
  return DD_OK; //m_lpDDSurface->GetBltStatus(dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetCaps(LPDDSCAPS lpDDSCaps)
{
  _debug_print(("MDirectDrawSurface::GetCaps [this = 0x%08X]\n", this));

  lpDDSCaps->dwCaps = _caps | DDSCAPS_PALETTE;

  return DD_OK; //m_lpDDSurface->GetCaps(lpDDSCaps);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetClipper(LPDIRECTDRAWCLIPPER FAR *lplpDDClipper)
{
  _debug_print(("MDirectDrawSurface::GetClipper\n"));
  return DD_OK; //m_lpDDSurface->GetClipper(lplpDDClipper);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey)
{
  _debug_print(("MDirectDrawSurface::GetColorKey\n"));
  return DD_OK; //m_lpDDSurface->GetColorKey(dwFlags, lpDDColorKey);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetDC(HDC * lphDC)
{
  _debug_print(("MDirectDrawSurface::GetDC\n"));
  return DD_OK; //m_lpDDSurface->GetDC(lphDC);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetFlipStatus(DWORD dwFlags)
{
  _debug_print(("MDirectDrawSurface::GetFlipStatus\n"));
  return DD_OK; //m_lpDDSurface->GetFlipStatus(dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetOverlayPosition(LPLONG lplX, LPLONG lplY)
{
  _debug_print(("MDirectDrawSurface::GetOverlayPosition\n"));
  return DD_OK; //m_lpDDSurface->GetOverlayPosition(lplX, lplY);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetPalette(LPDIRECTDRAWPALETTE FAR *lplpDDPalette)
{
  _debug_print(("MDirectDrawSurface::GetPalette\n"));
  return DD_OK; //m_lpDDSurface->GetPalette(lplpDDPalette);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
{
  _debug_print(("MDirectDrawSurface::GetPixelFormat\n"));
  return DD_OK; //m_lpDDSurface->GetPixelFormat(lpDDPixelFormat);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc)
{
  _debug_print(("MDirectDrawSurface::GetSurfaceDesc [this = 0x%08X]\n", this));
  return DD_OK; //m_lpDDSurface->GetSurfaceDesc(lpDDSurfaceDesc)
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::Initialize(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc)
{
  _debug_print(("MDirectDrawSurface::Initialize\n"));
  return DD_OK; //m_lpDDSurface->Initialize(lpDD, lpDDSurfaceDesc);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::IsLost(void)
{
  _debug_print(("MDirectDrawSurface::IsLost\n"));
  return DD_OK; //m_lpDDSurface->IsLost();
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
  //_debug_print(("MDirectDrawSurface::Lock [this = 0x%08X]\n", this));

  if (!_buf)
    return DD_FALSE;

  lpDDSurfaceDesc->lpSurface = _buf;
  lpDDSurfaceDesc->lPitch    = _width;

  return DD_OK; //m_lpDDSurface->Lock(lpDestRect, lpDDSurfaceDesc, dwFlags, hEvent);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::ReleaseDC(HDC hDC)
{
  _debug_print(("MDirectDrawSurface::ReleaseDC\n"));
  return DD_OK; //m_lpDDSurface->ReleaseDC(hDC);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::Restore(void)
{
  _debug_print(("MDirectDrawSurface::Restore [this = 0x%08X]\n", this));
  return DD_OK; //m_lpDDSurface->Restore();
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
{
  _debug_print(("MDirectDrawSurface::SetClipper\n"));
  return DD_OK; //m_lpDDSurface->SetClipper(lpDDClipper);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey)
{
  _debug_print(("MDirectDrawSurface::SetColorKey [this = 0x%08X, dwFlags = 0x%08X]\n", this, dwFlags));
  return DD_OK; //m_lpDDSurface->SetColorKey(dwFlags, lpDDColorKey);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::SetOverlayPosition(LONG lX, LONG lY)
{
  _debug_print(("MDirectDrawSurface::SetOverlayPosition\n"));
  return DD_OK; //m_lpDDSurface->SetOverlayPosition(lX, lY);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::SetPalette(LPDIRECTDRAWPALETTE lpDDPalette)
{
  _debug_print(("MDirectDrawSurface::SetPalette\n"));
  return DD_OK; //m_lpDDSurface->SetPalette(lpDDPalette);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::Unlock(LPVOID lpSurfaceData)
{
  //_debug_print(("MDirectDrawSurface::Unlock [this = 0x%08X]\n", this));
  return DD_OK; //m_lpDDSurface->Unlock(lpSurfaceData);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx)
{
  _debug_print(("MDirectDrawSurface::UpdateOverlay\n"));
  return DD_OK; //m_lpDDSurface->UpdateOverlay(lpSrcRect, lpDDDestSurface, lpDestRect, dwFlags, lpDDOverlayFx);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::UpdateOverlayDisplay(DWORD dwFlags)
{
  _debug_print(("MDirectDrawSurface::UpdateOverlayDisplay\n"));
  return DD_OK; //m_lpDDSurface->UpdateOverlayDisplay(dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface::UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSReference)
{
  _debug_print(("MDirectDrawSurface::UpdateOverlayZOrder\n"));
  return DD_OK; //m_lpDDSurface->UpdateOverlayZOrder(dwFlags, lpDDSReference);
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// MDirectDrawSurface2
// ----------------------------------------------------------------------------
MDirectDrawSurface2::MDirectDrawSurface2(void)
{
  //_alloc_cnt++;
  //_debug_print(("MDirectDrawSurface2::MDirectDrawSurface2 [this = 0x%08X, _alloc_cnt = %u]\n", this, _alloc_cnt));

  _ref_count = 1;
}
// ----------------------------------------------------------------------------
MDirectDrawSurface2::~MDirectDrawSurface2(void)
{
  //_alloc_cnt--;
  //_debug_print(("MDirectDrawSurface2::~MDirectDrawSurface2\n"));
}
// ----------------------------------------------------------------------------
/*** IUnknown methods ***/
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
  //_debug_print(("MDirectDrawSurface2::QueryInterface\n"));
  return DD_OK; //m_lpDDSurface->QueryInterface(riid, ppvObj);
}
// ----------------------------------------------------------------------------
ULONG WINAPI MDirectDrawSurface2::AddRef(void)
{
  //_debug_print(("MDirectDrawSurface2::AddRef\n"));

  _ref_count++;

  return _ref_count; //m_lpDDSurface->AddRef();
}
// ----------------------------------------------------------------------------
ULONG WINAPI MDirectDrawSurface2::Release(void)
{
  //_debug_print(("MDirectDrawSurface2::Release\n"));

  int cnt = --_ref_count;

  if (!cnt)
    delete this;

  return cnt; //m_lpDDSurface->Release();
}
// ----------------------------------------------------------------------------
/*** IDirectDrawSurface methods ***/
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::AddAttachedSurface(LPDIRECTDRAWSURFACE2 lpDDSAttachedSurface)
{
  //_debug_print(("MDirectDrawSurface2::AddAttachedSurface\n"));
  return DD_OK; //m_lpDDSurface->AddAttachedSurface(lpDDSAttachedSurface);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::AddOverlayDirtyRect(LPRECT lpRect)
{
  //_debug_print(("MDirectDrawSurface2::AddOverlayDirtyRect\n"));
  return DD_OK; //m_lpDDSurface->AddOverlayDirtyRect(lpRect);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE2 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
  //_debug_print(("MDirectDrawSurface2::Blt\n"));
  return DD_OK; //m_lpDDSurface->Blt(lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::BltBatch(LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface2::BltBatch\n"));
  return DD_OK; //m_lpDDSurface->BltBatch(lpDDBltBatch, dwCount, dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE2 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface2::BltFast\n"));
  return DD_OK; //m_lpDDSurface->BltFast(dwX, dwY, lpDDSrcSurface, lpSrcRect, dwTrans);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::DeleteAttachedSurface(DWORD dwFlags, LPDIRECTDRAWSURFACE2 lpDDSAttachedSurface)
{
  //_debug_print(("MDirectDrawSurface2::DeleteAttachedSurface\n"));
  return DD_OK; //m_lpDDSurface->DeleteAttachedSurface(dwFlags, lpDDSAttachedSurface);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::EnumAttachedSurfaces(LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback)
{
  //_debug_print(("MDirectDrawSurface2::EnumAttachedSurfaces\n"));
  return DD_OK; //m_lpDDSurface->EnumAttachedSurfaces(lpContext, lpEnumSurfacesCallback);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::EnumOverlayZOrders(DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback)
{
  //_debug_print(("MDirectDrawSurface2::EnumOverlayZOrders\n"));
  return DD_OK; //m_lpDDSurface->EnumOverlayZOrders(dwFlags, lpContext, lpfnCallback);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::Flip(LPDIRECTDRAWSURFACE2 lpDDSurfaceTargetOverride, DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface2::Flip\n"));
  return DD_OK; //m_lpDDSurface->Flip(lpDDSurfaceTargetOverride, dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetAttachedSurface(LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE2 FAR *lplpDDAttachedSurface)
{
  //_debug_print(("MDirectDrawSurface2::GetAttachedSurface [this = 0x%08X]\n", (unsigned long)this));
  return DD_OK; //m_lpDDSurface->GetAttachedSurface(lpDDSCaps, lplpDDAttachedSurface);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetBltStatus(DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface2::GetBltStatus\n"));
  return DD_OK; //m_lpDDSurface->GetBltStatus(dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetCaps(LPDDSCAPS lpDDSCaps)
{
  //_debug_print(("MDirectDrawSurface2::GetCaps\n"));
  return DD_OK; //m_lpDDSurface->GetCaps(lpDDSCaps);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetClipper(LPDIRECTDRAWCLIPPER FAR *lplpDDClipper)
{
  //_debug_print(("MDirectDrawSurface2::GetClipper\n"));
  return DD_OK; //m_lpDDSurface->GetClipper(lplpDDClipper);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey)
{
  //_debug_print(("MDirectDrawSurface2::GetColorKey\n"));
  return DD_OK; //m_lpDDSurface->GetColorKey(dwFlags, lpDDColorKey);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetDC(HDC FAR *lphDC)
{
  //_debug_print(("MDirectDrawSurface2::GetDC\n"));
  return DD_OK; //m_lpDDSurface->GetDC(lphDC);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetFlipStatus(DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface2::GetFlipStatus\n"));
  return DD_OK; //m_lpDDSurface->GetFlipStatus(dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetOverlayPosition(LPLONG lplX, LPLONG lplY)
{
  //_debug_print(("MDirectDrawSurface2::GetOverlayPosition\n"));
  return DD_OK; //m_lpDDSurface->GetOverlayPosition(lplX, lplY);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetPalette(LPDIRECTDRAWPALETTE FAR *lplpDDPalette)
{
  //_debug_print(("MDirectDrawSurface2::GetPalette\n"));
  return DD_OK; //m_lpDDSurface->GetPalette(lplpDDPalette);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
{
  //_debug_print(("MDirectDrawSurface2::GetPixelFormat\n"));
  return DD_OK; //m_lpDDSurface->GetPixelFormat(lpDDPixelFormat);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc)
{
  //_debug_print(("MDirectDrawSurface2::GetSurfaceDesc\n"));
  return DD_OK; //m_lpDDSurface->GetSurfaceDesc(lpDDSurfaceDesc);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::Initialize(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc)
{
  //_debug_print(("MDirectDrawSurface2::Initialize\n"));
  return DD_OK; //m_lpDDSurface->Initialize(lpDD, lpDDSurfaceDesc);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::IsLost(void)
{
  //_debug_print(("MDirectDrawSurface2::IsLost\n"));
  return DD_OK; //m_lpDDSurface->IsLost();
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::Lock(LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
  //_debug_print(("MDirectDrawSurface2::Lock\n"));
  return DD_OK; //m_lpDDSurface->Lock(lpDestRect, lpDDSurfaceDesc, dwFlags, hEvent);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::ReleaseDC(HDC hDC)
{
  //_debug_print(("MDirectDrawSurface2::ReleaseDC\n"));
  return DD_OK; //m_lpDDSurface->ReleaseDC(hDC);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::Restore(void)
{
  //_debug_print(("MDirectDrawSurface2::Restore\n"));
  return DD_OK; //m_lpDDSurface->Restore();
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
{
  //_debug_print(("MDirectDrawSurface2::SetClipper\n"));
  return DD_OK; //m_lpDDSurface->SetClipper(lpDDClipper);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpDDColorKey)
{
  //_debug_print(("MDirectDrawSurface2::SetColorKey\n"));
  return DD_OK; //m_lpDDSurface->SetColorKey(dwFlags, lpDDColorKey);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::SetOverlayPosition(LONG lX, LONG lY)
{
  //_debug_print(("MDirectDrawSurface2::SetOverlayPosition\n"));
  return DD_OK; //m_lpDDSurface->SetOverlayPosition(lX, lY);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::SetPalette(LPDIRECTDRAWPALETTE lpDDPalette)
{
  //_debug_print(("MDirectDrawSurface2::SetPalette\n"));
  return DD_OK; //m_lpDDSurface->SetPalette(lpDDPalette);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::Unlock(LPVOID lpSurfaceData)
{
  //_debug_print(("MDirectDrawSurface2::Unlock\n"));
  return DD_OK; //m_lpDDSurface->Unlock(lpSurfaceData);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE2 lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx)
{
  //_debug_print(("MDirectDrawSurface2::UpdateOverlay\n"));
  return DD_OK; //m_lpDDSurface->UpdateOverlay(lpSrcRect, lpDDDestSurface, lpDestRect, dwFlags, lpDDOverlayFx);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::UpdateOverlayDisplay(DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface2::UpdateOverlayDisplay\n"));
  return DD_OK; //m_lpDDSurface->UpdateOverlayDisplay(dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE2 lpDDSReference)
{
  //_debug_print(("MDirectDrawSurface2::UpdateOverlayDisplay\n"));
  return DD_OK; //m_lpDDSurface->UpdateOverlayZOrder(dwFlags, lpDDSReference);
}
// ----------------------------------------------------------------------------
/*** Added in the v2 interface ***/
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::GetDDInterface(LPVOID FAR *lplpDD)
{
  //_debug_print(("MDirectDrawSurface2::GetDDInterface\n"));
  return DD_OK; //m_lpDDSurface->GetDDInterface(lplpDD);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::PageLock(DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface2::PageLock\n"));
  return DD_OK; //m_lpDDSurface->PageLock(dwFlags);
}
// ----------------------------------------------------------------------------
HRESULT WINAPI MDirectDrawSurface2::PageUnlock(DWORD dwFlags)
{
  //_debug_print(("MDirectDrawSurface2::PageUnlock\n"));
  return DD_OK; //m_lpDDSurface->PageUnlock(dwFlags);
}
// ----------------------------------------------------------------------------
