// ----------------------------------------------------------------------------
// tools.cpp
// ----------------------------------------------------------------------------

#include <windows.h>

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

void debug_print(const char* format, ...)
{
  va_list arg_list;
  va_start(arg_list, format);

  char s[1024];
  wvsprintfA(s, format, arg_list);

  va_end(arg_list);

  OutputDebugStringA(s);
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

int str_len(const char* str)
{
  int cnt = -1;
  while (str[++cnt]);
  return cnt;
}

// ----------------------------------------------------------------------------

int str_cpy(char* dst, int size, const char* src)
{
  if (size <= 0)
    return 0;

  char* d = dst;
  const char* s = src;

  while (--size && *s)
    *d++ = *s++;

  *d = 0;

  return d - dst;
}

// ----------------------------------------------------------------------------

int str_cpy(char* dst, int size, const char* src, int cnt)
{
  if (size <= 0)
    return 0;

  char* d = dst;
  const char* s = src;

  int i = (cnt < size) ? (cnt + 1) : size;
  while (--i && *s)
    *d++ = *s++;

  *d = 0;

  return d - dst;
}

// ----------------------------------------------------------------------------

int str_cat(char* dst, int size, const char* src)
{
  int n = size;
  char* d = dst;

  while (n && *d)
  {
    d++;
    n--;
  }

  if (n > 0)
  {
    while (--n && *src)
      *d++ = *src++;

    *d = 0;
  }

  while (*src++)
    d++;

  return d - dst;
}

// ----------------------------------------------------------------------------

int str_pos(const char* str1, const char* str2)
{
  int i = 0;

  while (str1[i])
  {
    const char* s1 = str1 + i;
    const char* s2 = str2;

    while (*s1 && *s2 && (*s1 == *s2))
      s1++, s2++;

    if (!*s2)
      return i;

    i++;
  }

  return -1;
}

// ----------------------------------------------------------------------------
/*
int str_cmp(const char* str1, const char* str2)
{
  int ret = 0;

  while( !(ret = *(unsigned char *)str1 - *(unsigned char *)str2) && *str2)
    ++str1, ++str2;

  if (ret < 0)
    ret = -1;
  else if (ret > 0)
    ret = 1;

  return ret;
}
*/
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

int get_app_dir(char* dst, int size, bool trail_slash)
{
  int ln = (int)GetModuleFileNameA(0, dst, size);
  char* s = dst + ln;

  while (--s > dst)
  {
    if (*s == '\\')
    {
      if (trail_slash)
        s++;
      *s = 0;
      break;
    }
  }

  return s - dst;
}

// ----------------------------------------------------------------------------
/*
int get_file_name(char* dst, bool ext)
{
  int len =  0;
  int pos = -1;
  int dot = -1;

  while (char ch = dst[len])
  {
    if (ch == '\\' || ch == '/')
      pos = len;
    if (ch == '.' && dot < 0)
      dot = len;

    len++;
  }

  pos++;
  
  int ln = (!ext && dot >= 0) ? (dot - pos) : (len - pos);
  if (ln == len)
    return len;

  if (pos > 0)
    for (int i = 0; i < ln; i++, pos++)
      dst[i] = dst[pos];

  dst[ln] = 0;

  return ln;
}
*/
// ----------------------------------------------------------------------------

int get_file_name(char* dst, int size, const char* src, bool ext)
{
  int ln = str_len(src);
  int pos = ln;
  int dot = -1;

  while (pos--)
  {
    char ch = src[pos];
    if (ch == '\\' || ch == '/')
      break;
    if (ch == '.' && dot < 0)
      dot = pos;
  }
  pos++;
  ln = (!ext && dot >= 0) ? (dot - pos) : (ln - pos);

  /*if (!dst || dst == src)
  {
    if (pos > 0 && ln > 0)
      memmove(src, src + pos, ln);

    src[ln] = 0;
  }
  else*/
  if (size == 0)
  {
    ln = 0;
  }
  else
  {
    if (size < 0)
    {
      size = ln;
    }
    else
    {
      size--;
      if (ln > size)
        ln = size;
    }

    //memcpy(dst, src + pos, ln);
    //dst[ln] = 0;
    str_cpy(dst, size, src + pos, ln);
  }

  return ln;
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

#define _is_file(x) (((x) != INVALID_FILE_ATTRIBUTES) && !((x) & FILE_ATTRIBUTE_DIRECTORY))

#define _is_dir(x)  (((x) != INVALID_FILE_ATTRIBUTES) && ((x) & FILE_ATTRIBUTE_DIRECTORY))

// ----------------------------------------------------------------------------

bool file_exists(const char* filename)
{
  unsigned long a = GetFileAttributesA(filename);
  return _is_file(a);
}

// ----------------------------------------------------------------------------

bool dir_exists(const char* dirname)
{
  unsigned long a = GetFileAttributesA(dirname);
  return _is_dir(a);
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

int read_file(const char* filename, void* buf, int maxsize)
{
  void* handle = CreateFileA(filename,
                             GENERIC_READ,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             0,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             0);

  if (handle == INVALID_HANDLE_VALUE)
    return 0;

  unsigned long size = GetFileSize(handle, 0);

  if (!buf)
    return size;

  unsigned long size_read = 0;

  ReadFile(handle, buf, maxsize, &size_read, 0);
  CloseHandle(handle);

  return size_read;
}

// ----------------------------------------------------------------------------

bool write_file(const char* filename, const void* buf, int size)
{
  if (!buf)
    return false;

  void* handle = CreateFileA(filename,
    GENERIC_WRITE,
    FILE_SHARE_READ,
    0,
    CREATE_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    0);

  if (handle == INVALID_HANDLE_VALUE)
    return false;

  unsigned long size_written;
  bool res = WriteFile(handle, buf, size, &size_written, 0) && (size_written == static_cast<unsigned long>(size));

  CloseHandle(handle);

  return res;
}

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------

void mem_read_buf(unsigned long addr, void* buf, int len)
{
  unsigned long vp_value;
  VirtualProtect((void*)addr, len, /*PAGE_EXECUTE_READWRITE*/ PAGE_READWRITE, &vp_value);

  memcpy(buf, (void*)addr, len);

  VirtualProtect((void*)addr, len, vp_value, &vp_value);
}

// ----------------------------------------------------------------------------

void mem_write_buf(unsigned long addr, void* buf, int len)
{
  unsigned long vp_value;

  VirtualProtect((void*)addr, len, PAGE_EXECUTE_READWRITE, &vp_value);

  memcpy((void*)addr, buf, len);

  VirtualProtect((void*)addr, len, vp_value, &vp_value);
}

// ----------------------------------------------------------------------------

void mem_write_byte(unsigned long addr, unsigned char value)
{
  mem_write_buf(addr, &value, sizeof(unsigned char));
}

// ----------------------------------------------------------------------------
