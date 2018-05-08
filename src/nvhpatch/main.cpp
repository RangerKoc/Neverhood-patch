// ----------------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------------

#pragma comment(linker, "/ENTRY:DllMain")
#pragma comment(linker, "/NODEFAULTLIB")

// ----------------------------------------------------------------------------

#include "main.h"
#include <stdio.h>

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

#define MAX_LIST_COUNT 512

#define HKEY_SAVES      (HKEY)0xff000001
#define HKEY_SAVE       (HKEY)0xff000002
#define HKEY_SOFTWARE   (HKEY)0xff000003
#define HKEY_DREAMWORKS (HKEY)0xff000004
#define HKEY_NEVERHOOD  (HKEY)0xff000005

const char* STR_KEY_SOFTWARE    = "SOFTWARE";
const char* STR_KEY_DREAMWORKS  = "DreamWorks Interactive";
const char* STR_KEY_NEVERHOOD   = "Neverhood";
const char* STR_KEY_SAVED_GAMES = "Software\\DreamWorks Interactive\\Neverhood\\Saved Games";
const char* STR_KEY_DATA_DRIVE  = "Data Drive";
const char* STR_KEY_MUSICON     = "MusicOn";

const char* STR_ADVAPI32_DLL    = "advapi32.dll";

const char* STR_DATA_FILES[] =
{
  "a.blb",
  "c.blb",
  "hd.blb",
  "i.blb",
  "m.blb",
  "s.blb",
  "t.blb",
  NULL
};

// ----------------------------------------------------------------------------

WIN32_FIND_DATA saves_list[MAX_LIST_COUNT];
unsigned long   saves_count;

bool use_data_dir;

char app_dir[MAX_PATH];

//char data_dir[MAX_PATH];

char saves_dir[MAX_PATH];
char saves_pattern[MAX_PATH];

char config_filename[MAX_PATH];
//char save_filename[MAX_PATH];

// ----------------------------------------------------------------------------

void make_save_filename(char* dst, int size, const char* save_name);

void update_saves_list(void);

void export_saves(void);

void hook_api(void);

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH)
  {
    _debug_print(("DLL_PROCESS_ATTACH\n"));

    // 0x5024F
    mem_write_byte(0x450E4F, 0x90); // CLI - 0xFA
    // 0x50272
    mem_write_byte(0x450E72, 0x90); // STI - 0xFB

    saves_count = 0;

    get_app_dir(app_dir, sizeof(app_dir), true);

    str_cpy(saves_dir, sizeof(saves_dir), app_dir);
    str_cat(saves_dir, sizeof(saves_dir), "saves");

    str_cpy(config_filename, sizeof(config_filename), saves_dir);
    str_cat(config_filename, sizeof(config_filename), "\\config.dat");

    str_cpy(saves_pattern, sizeof(saves_pattern), saves_dir);
    str_cat(saves_pattern, sizeof(saves_pattern), "\\*.sav");

    char s[MAX_PATH];
    int i = 0;
    while (STR_DATA_FILES[i])
    {
      str_cpy(s, sizeof(s), app_dir);
      str_cat(s, sizeof(s), "data\\");
      str_cat(s, sizeof(s), STR_DATA_FILES[i]);

      if (!file_exists(s))
        break;

      i++;
    }

    use_data_dir = (STR_DATA_FILES[i] == NULL);

    if (!dir_exists(saves_dir))
    {
      if (!CreateDirectory(saves_dir, NULL))
        MessageBox(NULL, "Unable to create saves directory!", "Error", MB_ICONSTOP);
    }

    if (!file_exists(config_filename))
    {
      export_saves();
      write_file(config_filename, "1", 1);
    }
    

    mem_read_buf(0x4BD80A, s, 13);
    if (!lstrcmpi(s, STR_ADVAPI32_DLL))
      hook_api();
  }

  return TRUE;
}

// ----------------------------------------------------------------------------

LONG WINAPI _RegCloseKey(HKEY hKey)
{
  _debug_print(("RegCloseKey(0x%08X)\n", hKey));
  return 0;
}

// ----------------------------------------------------------------------------

LONG WINAPI _RegCreateKeyEx(HKEY hKey, LPCTSTR lpSubKey, DWORD Reserved, LPTSTR lpClass, DWORD dwOptions, REGSAM samDesired, LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
{
  _debug_print(("RegCreateKeyEx(0x%08X, \"%s\", 0x%08X)\n", hKey, lpSubKey, *phkResult));

  if (!str_pos(lpSubKey, STR_KEY_SAVED_GAMES))
  {
    *phkResult = HKEY_SAVE;
  }

  return 0;
}

// ----------------------------------------------------------------------------

LONG WINAPI _RegDeleteKey(HKEY hKey, LPCTSTR lpSubKey)
{
  _debug_print(("RegDeleteKey(0x%08X, \"%s\")\n", hKey, lpSubKey));

  if (hKey == HKEY_SAVES)
  {
    char s[MAX_PATH];
    make_save_filename(s, sizeof(s), lpSubKey);
    DeleteFile(s);
  }

  return 0;
}

// ----------------------------------------------------------------------------

LONG WINAPI _RegEnumKey(HKEY hKey, DWORD dwIndex, LPTSTR lpName, DWORD cchName)
{
  if (hKey == HKEY_SAVES)
  {
    if (!dwIndex)
      update_saves_list();

    if (dwIndex < saves_count)
    {
      get_file_name(lpName, cchName, saves_list[dwIndex].cFileName, false);

      return 0;
    }
  }

  _debug_print(("RegEnumKeyA(0x%08X, %u, \"%s\", %u)\n", hKey, dwIndex, lpName, cchName));

  return ERROR_NO_MORE_ITEMS;
}

// ----------------------------------------------------------------------------

LONG WINAPI _RegOpenKeyEx(HKEY hKey, LPCTSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
  if (!lstrcmpi(lpSubKey, STR_KEY_SOFTWARE))
  {
    *phkResult = HKEY_SOFTWARE;
  }
  else if (!lstrcmpi(lpSubKey, STR_KEY_DREAMWORKS))
  {
    *phkResult = HKEY_DREAMWORKS;
  }
  else if (!lstrcmpi(lpSubKey, STR_KEY_NEVERHOOD))
  {
    *phkResult = HKEY_NEVERHOOD;
  }
  else if (!lstrcmpi(lpSubKey, STR_KEY_SAVED_GAMES))
  {
    *phkResult = HKEY_SAVES;
  }
  else if (!str_pos(lpSubKey, STR_KEY_SAVED_GAMES))
  {
    char name[32];
    get_file_name(name, sizeof(name), lpSubKey);

    char s[MAX_PATH];
    make_save_filename(s, sizeof(s), name);

    if (!file_exists(s))
      return ERROR_FILE_NOT_FOUND;

    *phkResult = HKEY_SAVE;
  }

  _debug_print(("RegOpenKeyExA(0x%08X, \"%s\", 0x%08X)\n", hKey, lpSubKey, *phkResult));

  return 0;
}

// ----------------------------------------------------------------------------

LONG WINAPI _RegQueryValueEx(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
  if (hKey == HKEY_SAVE)
  {
    char s[MAX_PATH];
    make_save_filename(s, sizeof(s), lpValueName);

    *lpcbData = read_file(s, lpData, *lpcbData);

    //return 0;
  }
  else if (hKey == HKEY_NEVERHOOD)
  {
    if (!lstrcmpi(lpValueName, STR_KEY_MUSICON))
    {
      unsigned char data;
      if (!read_file(config_filename, &data, 1))
        data = '1';

      lpData[0] = data;
      lpData[1] = 0;
      *lpcbData = 1;

      //return 0;
    }
    else if (!lstrcmpi(lpValueName, STR_KEY_DATA_DRIVE))
    {
      if (use_data_dir)
      {
        *lpcbData = str_cpy((char*)lpData, *lpcbData, app_dir);
      }
      else
      {
        lpData[0] = 0;
        *lpcbData = 0;
      }

      //return 0;
    }
  }

  _debug_print(("RegQueryValueEx(0x%08X, \"%s\", 0x%08X, 0x%08X, %u)\n", hKey, lpValueName, lpType, lpData, *lpcbData));

  //return ERROR_FILE_NOT_FOUND;
  return 0;
}

// ----------------------------------------------------------------------------

LONG WINAPI _RegSetValueEx(HKEY hKey, LPCTSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData)
{
  _debug_print(("RegSetValueEx(0x%08X, \"%s\", %u, 0x%08X, %u)\n", hKey, lpValueName, dwType, lpData, cbData));

  if (hKey == HKEY_SAVE)
  {
    char s[MAX_PATH];
    make_save_filename(s, sizeof(s), lpValueName);

    SetFileAttributes(s, FILE_ATTRIBUTE_NORMAL);
    if (!write_file(s, lpData, cbData))
      return ERROR_ACCESS_DENIED;
  }
  else if (hKey == HKEY_NEVERHOOD)
  {
    if (!lstrcmpi(lpValueName, STR_KEY_MUSICON))
    {
      write_file(config_filename, lpData, 1);
    }
  }

  return 0;
}

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------

void make_save_filename(char* dst, int size, const char* save_name)
{
  str_cpy(dst, size, saves_dir);
  str_cat(dst, size, "\\");
  str_cat(dst, size, save_name);
  str_cat(dst, size, ".sav");
}

// ----------------------------------------------------------------------------

void update_saves_list(void)
{
  HANDLE hFind = FindFirstFile(saves_pattern, saves_list);
  if (hFind == INVALID_HANDLE_VALUE)
  {
    saves_count = 0;
    return;
  }

  int i = 0;

  do
  {
    if (!(saves_list[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
         (str_pos(saves_list[i].cFileName, ".sav") == (str_len(saves_list[i].cFileName) - 4)))
      i++;
  } while (FindNextFile(hFind, &saves_list[i]));

  FindClose(hFind);

  saves_count = i;
}

// ----------------------------------------------------------------------------

void export_saves(void)
{
  if (!dir_exists(saves_dir))
    return;

  HKEY key_saves;

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, STR_KEY_SAVED_GAMES, 0, KEY_ENUMERATE_SUB_KEYS, &key_saves))
    return;

  char s[32];
  int i = 0;
  while (!RegEnumKey(key_saves, i, s, sizeof(s)))
  {
    HKEY key;
    if (!RegOpenKeyEx(key_saves, s, 0, KEY_QUERY_VALUE, &key))
    {
      unsigned long size, type;
      if (!RegQueryValueEx(key, s, 0, &type, NULL, &size) && (size > 0))
      {
        unsigned char* buf = new unsigned char [size];

        if (!RegQueryValueEx(key, s, 0, &type, buf, &size) && (size > 0))
        {
          char fn[MAX_PATH];

          make_save_filename(fn, sizeof(fn), s);

          if (!file_exists(fn))
            write_file(fn, buf, size);
        }

        delete [] buf;
      }
      RegCloseKey(key);
    }
    i++;
  }

  RegCloseKey(key_saves);
}

// ----------------------------------------------------------------------------

void hook_api(void)
{
  hook_t hk[] =
  {
    { "RegCloseKey",      _RegCloseKey     },
    { "RegCreateKeyExA",  _RegCreateKeyEx  },
    { "RegDeleteKeyA",    _RegDeleteKey    },
    { "RegEnumKeyA",      _RegEnumKey      },
    { "RegOpenKeyExA",    _RegOpenKeyEx    },
    { "RegQueryValueExA", _RegQueryValueEx },
    { "RegSetValueExA",   _RegSetValueEx   },
    { "", NULL }
  };

  hook_iat(STR_ADVAPI32_DLL, hk);
}

// ----------------------------------------------------------------------------
