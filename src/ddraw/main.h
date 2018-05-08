// ----------------------------------------------------------------------------
// main.h
// ----------------------------------------------------------------------------

#pragma once

// ----------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define INITGUID

// ----------------------------------------------------------------------------

#include <windows.h>

#include "d3d9.h"

#include "tools.h"

// ----------------------------------------------------------------------------

#ifdef _DEBUG
#define _debug_print(p) debug_print p
#else
#define _debug_print(p)
#endif

// ----------------------------------------------------------------------------

struct TLVERTEX
{
  float x;
  float y;
  float z;
  float rhw;
  unsigned long color;
  float u;
  float v;
};

// ----------------------------------------------------------------------------

struct render_t
{
  HWND hwnd;

  IDirect3D9*        d3d;
  IDirect3DDevice9*  device;
  IDirect3DTexture9* texture;

  D3DDISPLAYMODE desktop;

  RECT rect;

  int screen_width;
  int screen_height;

  int width;
  int height;

  unsigned long palette[256];

  bool vsync;
  bool windowed;

  bool smooth_scale;
  int  scale_mode;

  RECT cursor_rect;
  bool cursor_clip;
  bool cursor_scale;

  bool reset_rqst;

  TLVERTEX vertex[4];

  int state;
};

// ----------------------------------------------------------------------------

extern render_t g_render;

// ----------------------------------------------------------------------------

void reset_render(void);

void deinit_render(void);

void update_screen(void* buf);

// ----------------------------------------------------------------------------
