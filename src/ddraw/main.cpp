// ----------------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------------

#pragma comment(linker, "/ENTRY:DllMain")
#pragma comment(linker, "/NODEFAULTLIB")

// ----------------------------------------------------------------------------

#include "main.h"

#include "ddraw\DirectDraw.h"

// ----------------------------------------------------------------------------

#define DEFAULT_WIDTH  640
#define DEFAULT_HEIGHT 480

// ----------------------------------------------------------------------------

enum scale_mode
{
  scale_mode_original,
  scale_mode_integer,
  scale_mode_aspect_ratio,
  scale_mode_stretch,
  scale_mode_manual,
};

// ----------------------------------------------------------------------------

const int max_cursor_width  = 1024;
const int max_cursor_height = 768;

// ----------------------------------------------------------------------------

render_t g_render;

void* backbuf;

WNDPROC wndproc_prev;

// ----------------------------------------------------------------------------

void hook_wndproc(bool set);

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
  DisableThreadLibraryCalls(hInstance);

  switch(fdwReason)
  {
    case DLL_PROCESS_ATTACH:
      {
        _debug_print(("DLL_PROCESS_ATTACH\n"));

        LoadLibrary("nvhpatch.dll");

        // 0x5024F
        mem_write_byte(0x00450E4F, 0x90); // CLI - 0xFA
        // 0x50272
        mem_write_byte(0x00450E72, 0x90); // STI - 0xFB

        memset(&g_render, 0, sizeof(render_t));

        g_render.screen_width  = DEFAULT_WIDTH;
        g_render.screen_height = DEFAULT_HEIGHT;

        g_render.vsync = true;
        //g_render.windowed = true;

        g_render.cursor_scale = true;
        g_render.cursor_clip  = true;

        //g_render.smooth_scale = true;

        //g_render.scale_mode = scale_mode_original;
        //g_render.scale_mode = scale_mode_integer;
        //g_render.scale_mode = scale_mode_aspect_ratio;
        //g_render.scale_mode = scale_mode_stretch;

        char app_dir[MAX_PATH], s[MAX_PATH];
        get_app_dir(app_dir, sizeof(app_dir), true);

        str_cpy(s, sizeof(s), app_dir);
        str_cat(s, sizeof(s), "integer-scale.mode");
        g_render.scale_mode = file_exists(s) ? scale_mode_integer : scale_mode_aspect_ratio;

        str_cpy(s, sizeof(s), app_dir);
        str_cat(s, sizeof(s), "disable-scale-filter.mode");
        g_render.smooth_scale = !file_exists(s);

        str_cpy(s, sizeof(s), app_dir);
        str_cat(s, sizeof(s), "scanlines.mode");
        g_render.scanlines = file_exists(s);

        str_cpy(s, sizeof(s), app_dir);
        str_cat(s, sizeof(s), "windowed2x.mode");
        if (file_exists(s))
        {
          g_render.windowed = true;
          g_render.windowedx2 = true;
        }
        else
        {
          str_cpy(s, sizeof(s), app_dir);
          str_cat(s, sizeof(s), "windowed.mode");
          g_render.windowed = file_exists(s);
        }

        for (int i = 0; i < 4; i++)
        {
          g_render.vertex[i].color = 0xffffffff;
          g_render.vertex[i].rhw   = 1.0f;
        }

        g_render.vertex[1].u = 1.0f;
        g_render.vertex[2].u = 1.0f;
        g_render.vertex[2].v = 1.0f;
        g_render.vertex[3].v = 1.0f;

        backbuf = NULL;

        wndproc_prev = NULL;
      }
      break;

    case DLL_PROCESS_DETACH:
      {
        _debug_print(("DLL_PROCESS_DETACH\n"));

        hook_wndproc(false);

        if (g_render.device)
        {
          g_render.device->Release();
          g_render.device = NULL;
        }

        if (g_render.d3d)
        {
          g_render.d3d->Release();
          g_render.d3d = NULL;
        }

        if (backbuf)
          delete [] backbuf;
      }
      break;
  }

  return TRUE;
}

// ----------------------------------------------------------------------------

HRESULT WINAPI _DirectDrawCreate(GUID * lpGUID, LPDIRECTDRAW * lplpDD, IUnknown * pUnkOuter)
{
  _debug_print(("_DirectDrawCreate\n"));

  *lplpDD = new MDirectDraw();

  if (!g_render.d3d)
  {
    g_render.d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_render.d3d)
    {
      _debug_print(("Direct3DCreate9 fail\n"));
      return DD_FALSE;
    }

    g_render.d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &g_render.desktop);
  }

  return DD_OK;
}

// ----------------------------------------------------------------------------

void clip_cursor(HWND hwnd = NULL, bool value = false)
{
  if (!value)
  {
    ClipCursor(NULL);
  }
  else
  {
    //RECT rc = {0, 0, g_render.width, g_render.height};
    RECT rc = g_render.cursor_rect;
    if (g_render.windowed)
    {
      POINT pt = {0, 0};
      ClientToScreen(hwnd, &pt);

      rc.left   += pt.x;
      rc.top    += pt.y;
      rc.right  += pt.x;
      rc.bottom += pt.y;
    }

    ClipCursor(&rc);
  }
}

// ----------------------------------------------------------------------------

unsigned long scale_cursor_pos(unsigned long pos)
{
  union
  {
    unsigned long pos;
    struct
    {
      int x : 16;
      int y : 16;
    };
  } coord;

  coord.pos = pos;

  int x = coord.x - g_render.cursor_rect.left;
  int y = coord.y - g_render.cursor_rect.top;

  int w = g_render.cursor_rect.right  - g_render.cursor_rect.left;
  int h = g_render.cursor_rect.bottom - g_render.cursor_rect.top;

  if (x < 0)
    x = 0;
  if (x >= w)
    x = w - 1;
  if (y < 0)
    y = 0;
  if (y >= h)
    y = h - 1;

  coord.x = (x * g_render.width ) / w;
  coord.y = (y * g_render.height) / h;

  //coord.x = (coord.x * g_render.width ) / g_render.screen_width;
  //coord.y = (coord.y * g_render.height) / g_render.screen_height;

  return coord.pos;
}

// ----------------------------------------------------------------------------

void window_init(void);

LRESULT __stdcall wndproc(HWND hWnd, UINT Msg, WPARAM WParam, LPARAM LParam)
{
  switch (Msg)
  {
    case WM_NCPAINT:
      return DefWindowProc(hWnd, Msg, WParam, LParam);

    // ------------------------------------------

    case WM_CLOSE:
    {
      ExitProcess(0);
      return 0;
    }

    // ------------------------------------------

    case WM_ACTIVATEAPP:
    {
      if (WParam)
      {
        // activate
        if (g_render.device)
        {
          if (g_render.device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
          {
            g_render.d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &g_render.desktop);
            g_render.reset_rqst = true;
          }
        }

        if (!g_render.windowed || g_render.cursor_clip)
          clip_cursor(NULL, true);
      }
      else
      {
        // deactivate
        clip_cursor();
      }
      return 0;
    }

    // ------------------------------------------

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    {
      if (!g_render.windowed || g_render.cursor_clip)
        clip_cursor(hWnd, true);

      //if (g_render.cursor_scale)
        LParam = scale_cursor_pos(LParam);

      break;
    }

    // ------------------------------------------

    case WM_MOUSEMOVE:
    {
      //if (g_render.cursor_scale)
        LParam = scale_cursor_pos(LParam);

      break;
    }

    // ------------------------------------------
/*
    case WM_NCMOUSEMOVE:
    {
      ShowCursor(TRUE);
      return DefWindowProc(hWnd, Msg, WParam, LParam);
    }
*/
    // ------------------------------------------

    case WM_KEYDOWN:
    {
      _debug_print(("WM_KEYDOWN: %02X\n", WParam));
      if (WParam == VK_ESCAPE)
      {
        if (g_render.windowed && g_render.cursor_clip)
          clip_cursor();
      }
      else if (WParam == VK_LEFT)
      {
        wndproc_prev(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, (380 << 16));
      }
      else if (WParam == VK_RIGHT)
      {
        wndproc_prev(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, 639 | (380 << 16));
      }
      else if (WParam == VK_UP)
      {
        wndproc_prev(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, 320);
      }
      else if (WParam == VK_ADD)
      {
        g_render.reset_rqst = true;
      }
      break;
    }

    // ------------------------------------------

    case WM_SYSKEYDOWN:
    {
      if (WParam != VK_F4)
        return 0;
    }

    // ------------------------------------------

    case WM_SYSKEYUP:
    {
      if (WParam == VK_RETURN)
      {
        // ALt + Enter
        _debug_print(("WM_SYSKEYUP: Alt + Enter\n"));

        g_render.windowed = !g_render.windowed;
        g_render.reset_rqst = true;

        return 0;
      }

      if (WParam != VK_F4)
        return 0;
    }

    // ------------------------------------------

    case WM_SHOWWINDOW:
    {
      if (!g_render.windowed && (WParam == TRUE))
        clip_cursor(NULL, true);
      break;
    }

    // ------------------------------------------

    case WM_DISPLAYCHANGE:
    {
      g_render.reset_rqst = true;
      return 0;
    }

    // ------------------------------------------

    default:
      break;
  }

  return CallWindowProc(wndproc_prev, hWnd, Msg, WParam, LParam);
}

// ----------------------------------------------------------------------------

void hook_wndproc(bool set)
{
  if (!g_render.hwnd)
    return;

  if (set)
  {
    if (!wndproc_prev)
      wndproc_prev = (WNDPROC)SetWindowLong(g_render.hwnd, GWL_WNDPROC, (long)wndproc);
  }
  else
  {
    if (wndproc_prev)
    {
      SetWindowLong(g_render.hwnd, GWL_WNDPROC, (long)wndproc_prev);
      wndproc_prev = NULL;
    }
  }
}

// ----------------------------------------------------------------------------

void window_init(void)
{
  unsigned long Style;
  RECT rc;

  //if (g_render.windowed && (g_render.screen_width < (long)g_render.desktop.Width) && (g_render.screen_height < (long)g_render.desktop.Height))
  if (g_render.windowed)
  {
    //Style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX;
    Style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    rc.left   = 0;
    rc.top    = 0;
    rc.right  = g_render.screen_width;
    rc.bottom = g_render.screen_height;

    if (g_render.windowedx2)
    {
      rc.right  *= 2;
      rc.bottom *= 2;
    }

    AdjustWindowRect(&rc, Style, FALSE);

    rc.right -= rc.left;
    rc.bottom -= rc.top;

    rc.left = (g_render.desktop.Width  - rc.right ) / 2;
    rc.top  = (g_render.desktop.Height - rc.bottom) / 2;

    if (rc.left < 0)
      rc.left = 0;
    
    if (rc.top < 0)
      rc.top = 0;
  }
  else
  {
    Style = WS_POPUP;

    rc.left   = 0;
    rc.top    = 0;
    rc.right  = g_render.desktop.Width;
    rc.bottom = g_render.desktop.Height;
  }

  SetWindowLong(g_render.hwnd, GWL_STYLE, Style);
  SetWindowPos(g_render.hwnd, HWND_NOTOPMOST, rc.left, rc.top, rc.right, rc.bottom, SWP_SHOWWINDOW);

  SetClassLong(g_render.hwnd, GCL_HCURSOR, (long)LoadCursor(NULL, IDC_ARROW));
}

// ----------------------------------------------------------------------------

void window_show(void)
{
  ShowWindow(g_render.hwnd, SW_SHOWNORMAL);
  RedrawWindow(0, NULL, 0, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_ALLCHILDREN);
}

// ----------------------------------------------------------------------------

void calc_cursor_rect(int width, int height)
{
  g_render.cursor_rect.left   = (g_render.screen_width  - width ) / 2;
  g_render.cursor_rect.top    = (g_render.screen_height - height) / 2;
  g_render.cursor_rect.right  = g_render.cursor_rect.left + width;
  g_render.cursor_rect.bottom = g_render.cursor_rect.top  + height;
}

// ----------------------------------------------------------------------------

void update_rects(void)
{
  int x1 = 0;
  int x2 = g_render.screen_width;
  int y1 = 0;
  int y2 = g_render.screen_height;

  switch (g_render.scale_mode)
  {
    case scale_mode_original:
    {
      x1 = (g_render.screen_width  - g_render.width ) / 2;
      y1 = (g_render.screen_height - g_render.height) / 2;
      x2 = x1 + g_render.width;
      y2 = y1 + g_render.height;

      break;
    }

    case scale_mode_integer:
    {
      int w = g_render.screen_width  / g_render.width;
      int h = g_render.screen_height / g_render.height;

      if (w < h)
      {
        h = g_render.height * w;
        w = g_render.width  * w;
      }
      else
      {
        w = g_render.width  * h;
        h = g_render.height * h;
      }

      x1 = (g_render.screen_width  - w) / 2;
      y1 = (g_render.screen_height - h) / 2;
      x2 = x1 + w;
      y2 = y1 + h;

      break;
    }

    case scale_mode_aspect_ratio:
    {
      x2 = g_render.screen_width;
      y2 = (g_render.screen_width * g_render.height) / g_render.width;
      if (y2 > g_render.screen_height)
      {
        x2 = (g_render.screen_height * g_render.width) / g_render.height;
        y2 = g_render.screen_height;
      }
      x1 = (g_render.screen_width  - x2) / 2;
      y1 = (g_render.screen_height - y2) / 2;
      x2 += x1;
      y2 += y1;

      break;
    }

    default:
      break;
  }

  g_render.rect.left   = x1;
  g_render.rect.top    = y1;
  g_render.rect.right  = x2;
  g_render.rect.bottom = y2;

  g_render.vertex[0].x = (float)x1 - 0.5f;
  g_render.vertex[0].y = (float)y1 - 0.5f;
  g_render.vertex[1].x = (float)x2 - 0.5f;
  g_render.vertex[1].y = (float)y1 - 0.5f;
  g_render.vertex[2].x = (float)x2 - 0.5f;
  g_render.vertex[2].y = (float)y2 - 0.5f;
  g_render.vertex[3].x = (float)x1 - 0.5f;
  g_render.vertex[3].y = (float)y2 - 0.5f;

  if (!g_render.cursor_scale ||
      (g_render.screen_width  < max_cursor_width ) ||
      (g_render.screen_height < max_cursor_height))
  {
    calc_cursor_rect(g_render.width, g_render.height);
  }
  else
  {
    calc_cursor_rect(max_cursor_width, max_cursor_height);
  }
}

// ----------------------------------------------------------------------------

void reset_render(void)
{
  D3DPRESENT_PARAMETERS present_params;
  memset(&present_params, 0, sizeof(D3DPRESENT_PARAMETERS));

  present_params.hDeviceWindow = g_render.hwnd;
  present_params.Windowed = g_render.windowed ? TRUE : FALSE;

  present_params.BackBufferCount  = 2;
  present_params.BackBufferFormat = g_render.desktop.Format;

  present_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
  present_params.SwapEffect           = D3DSWAPEFFECT_DISCARD;

  if (present_params.Windowed)
  //if (Windowed && (g_render.screen_width < (long)DesktopDisplayMode.Width) && (g_render.screen_height < (long)DesktopDisplayMode.Height))
  {
    if (g_render.vsync)
      present_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    //present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;

    present_params.BackBufferWidth  = g_render.width;
    present_params.BackBufferHeight = g_render.height;

    //present_params.FullScreen_RefreshRateInHz = 0;

    g_render.screen_width  = g_render.width;
    g_render.screen_height = g_render.height;
  }
  else
  {
    if (g_render.vsync)
      present_params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    //present_params.SwapEffect = D3DSWAPEFFECT_DISCARD; //D3DSWAPEFFECT_FLIP;

    present_params.BackBufferWidth  = g_render.desktop.Width;
    present_params.BackBufferHeight = g_render.desktop.Height;

    //present_params.FullScreen_RefreshRateInHz = RefreshRate;

    g_render.screen_width  = g_render.desktop.Width;
    g_render.screen_height = g_render.desktop.Height;
  }

  update_rects();

  hook_wndproc(true);

  if (!backbuf)
  {
    int size = g_render.width * g_render.height;
    backbuf = new unsigned char [size];
    memset(backbuf, 0, size);
  }

  if (!g_render.device)
  {
    window_init();
    window_show();

    HRESULT result = g_render.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_render.hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING /*| D3DCREATE_MULTITHREADED*/, &present_params, &g_render.device);

    if (result != D3D_OK)
    {
      _debug_print(("g_render.d3d->CreateDevice(..) fail, error = 0x%08X\n", result));
      result = g_render.d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_render.hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING /*| D3DCREATE_MULTITHREADED*/, &present_params, &g_render.device);
    }

    if (result != D3D_OK)
    {
      _debug_print(("g_render.d3d->CreateDevice(..) fail, error = 0x%08X\n", result));
      return;
    }

    result = g_render.device->CreateTexture(g_render.width, g_render.height, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &g_render.texture, NULL);
    //result = g_render.device->CreateTexture(g_render.width, g_render.height, 1, 0, D3DFMT_R5G6B5, D3DPOOL_MANAGED, &g_render.texture, NULL);
    if (result != D3D_OK)
    {
      _debug_print(("g_render.device->CreateTexture(..) fail, error = 0x%08X\n", result));
      return;
    }
  }
  else
  {
    window_init();

    HRESULT result = g_render.device->Reset(&present_params);
    if (result != D3D_OK)
    {
      _debug_print(("g_render.device->Reset(..) fail, error = 0x%08X\n", result));
      return;
    }
  }

  g_render.device->SetVertexShader(NULL);
  g_render.device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

  //g_render.device->SetRenderState(D3DRS_LIGHTING, FALSE);
  g_render.device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  //g_render.device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  g_render.device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  g_render.device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  //g_render.device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
  //g_render.device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
  //g_render.device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
  //g_render.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  //g_render.device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
  //g_render.device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
  //g_render.device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  //g_render.device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
  //g_render.device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

  //g_render.device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  //g_render.device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  g_render.device->SetSamplerState(0, D3DSAMP_MAGFILTER, g_render.smooth_scale ? D3DTEXF_LINEAR : D3DTEXF_POINT);
  g_render.device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
  g_render.device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
  g_render.device->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

  if (!g_render.windowed || g_render.cursor_clip)
    clip_cursor(NULL, true);

  g_render.reset_rqst = false;
}

// ----------------------------------------------------------------------------

void deinit_render(void)
{
  _debug_print(("DeInitDevice()\n"));

  if (g_render.texture)
  {
    g_render.texture->Release();
    g_render.texture = NULL;
  }

  if (g_render.device)
  {
    g_render.device->Release();
    g_render.device = NULL;
  }

  ShowWindow(g_render.hwnd, SW_HIDE);
  //RedrawWindow(0, NULL, 0, RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_ALLCHILDREN);
}

// ----------------------------------------------------------------------------

void update_screen(void* buf)
{
  if (g_render.reset_rqst)
    reset_render();

  D3DLOCKED_RECT lock;

  if (g_render.texture->LockRect(0, &lock, NULL, 0) == D3D_OK)
  {
    unsigned long* buf_des = static_cast<unsigned long*>(lock.pBits);
    unsigned char* buf_src = static_cast<unsigned char*>(backbuf);

    for (int y = 0; y < g_render.height; y++)
    {
      for (int x = 0; x < g_render.width; x++)
      {
        if (buf_src[x])
        {
          buf_des[x] = g_render.palette[buf_src[x]];
        }
        else
        {
          buf_des[x] = 0;
          //buf_des[x] = 0xffffffff;
          //buf_des[x] = g_render.palette[255];
        }
      }

      buf_des += lock.Pitch / 4;
      buf_src += g_render.width;
    }

    g_render.texture->UnlockRect(0);
  }
  else
  {
    _debug_print(("g_render.texture->LockRect(..) fail\n"));
    return;
  }

  {
    bool is_black = true;

    for (int i = 0; i < 254; i++)
      if (g_render.palette[i] != 0xff000000)
        is_black = false;

    if (is_black)
    {
      memset(backbuf, 0, g_render.width * g_render.height);
    }
    else
    {
      memcpy(backbuf, buf, g_render.width * g_render.height);
    }
  }

  TLVERTEX vertex[5];
  memset(vertex, 0, sizeof(vertex));

  vertex[0].color = 0xff000000;
  vertex[0].x     = -0.5f;
  vertex[0].y     = -0.5f;
  vertex[0].rhw   = 1.0f;

  vertex[1].color = vertex[0].color;
  vertex[1].x     = g_render.screen_width - 0.5f;
  vertex[1].y     = -0.5f;
  vertex[1].rhw   = 1.0f;

  vertex[2].color = vertex[0].color;
  vertex[2].x     = g_render.screen_width - 0.5f;
  vertex[2].y     = g_render.screen_height - 0.5f;
  vertex[2].rhw   = 1.0f;

  vertex[3].color = vertex[0].color;
  vertex[3].x     = -0.5f;
  vertex[3].y     = g_render.screen_height - 0.5f;
  vertex[3].rhw   = 1.0f;

  vertex[4].color = vertex[0].color;
  vertex[4].x     = -0.5f;
  vertex[4].y     = -0.5f;
  vertex[4].rhw   = 1.0f;

  g_render.device->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);

  g_render.device->BeginScene();

  g_render.device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 4, vertex, sizeof(TLVERTEX));

  g_render.device->SetTexture(0, g_render.texture);
  g_render.device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, g_render.vertex, sizeof(TLVERTEX));
  g_render.device->SetTexture(0, NULL);

  if (g_render.scanlines && !g_render.windowed)
  {
    for (float y = g_render.vertex[0].y; y <= g_render.vertex[2].y; y += 2)
    {
      vertex[0].color = 0x5f000000;
      vertex[0].y     = y;
      vertex[1].color = vertex[0].color;
      vertex[1].y     = y;

      g_render.device->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vertex, sizeof(TLVERTEX));
    }
  }

  g_render.device->EndScene();
  g_render.device->Present(NULL, NULL, 0, NULL);
}

// ----------------------------------------------------------------------------
