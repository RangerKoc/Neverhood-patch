// ----------------------------------------------------------------------------
// apihook.cpp
// ----------------------------------------------------------------------------

#include <windows.h>
#include "apihook.h"

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------

void hook_iat(const char* library, const hook_t* hooks)
{
  unsigned char* base = (unsigned char*)GetModuleHandle(NULL);

  IMAGE_DOS_HEADER* dos_hdr = (IMAGE_DOS_HEADER*)base;
  IMAGE_NT_HEADERS* nt_hdr  = (IMAGE_NT_HEADERS*)(base + dos_hdr->e_lfanew);

  IMAGE_IMPORT_DESCRIPTOR* dir = (IMAGE_IMPORT_DESCRIPTOR*)(base + nt_hdr->OptionalHeader.DataDirectory[1].VirtualAddress);

  int i_dir = 0;

  while (dir[i_dir].Name)
  {
    char* s = (char*)(base + dir[i_dir].Name);

    if (!lstrcmpi(s, library))
    {
      DWORD* tmp = (DWORD*)(base + dir[i_dir].Characteristics);

      int i_tmp   = 0;
      int i_thunk = 0;

      while (tmp[i_tmp])
      {
        s = (char*)(base + tmp[i_tmp] + 2);

        int i_hook = 0;

        while (hooks[i_hook].func)
        {
          if (!lstrcmp(s, hooks[i_hook].name))
          {
            IMAGE_THUNK_DATA* thunk = (IMAGE_THUNK_DATA*)(base + dir[i_dir].FirstThunk + (sizeof(IMAGE_THUNK_DATA) * i_thunk));
            DWORD protect;

            VirtualProtect(thunk, sizeof(IMAGE_THUNK_DATA), PAGE_EXECUTE_READWRITE, &protect);

            thunk->u1.Function      = (DWORD)hooks[i_hook].func;
            thunk->u1.Ordinal       = (DWORD)hooks[i_hook].func;
            thunk->u1.AddressOfData = (DWORD)hooks[i_hook].func;

            VirtualProtect(thunk, sizeof(IMAGE_THUNK_DATA), protect, &protect);
          }

          i_hook++;
        }

        i_tmp++;
        i_thunk++;
      }
    }

    i_dir++;
  }
}

// ----------------------------------------------------------------------------
