// ----------------------------------------------------------------------------
// mini crt
// ----------------------------------------------------------------------------

#include <windows.h>

// ----------------------------------------------------------------------------
/*
int main(int argc, char * argv[])
{
  return 0;
}
*/
// ----------------------------------------------------------------------------










// ----------------------------------------------------------------------------

void * __cdecl operator new(size_t n)
{
  return HeapAlloc(GetProcessHeap(), 0, n);
}

// ----------------------------------------------------------------------------

void * __cdecl operator new[](size_t n)
{
  return operator new(n);
}

// ----------------------------------------------------------------------------

void __cdecl operator delete(void * p)
{
  HeapFree(GetProcessHeap(), 0, p);
}

// ----------------------------------------------------------------------------

void __cdecl operator delete[](void* p)
{
  operator delete(p);
}

// ----------------------------------------------------------------------------










// ----------------------------------------------------------------------------

#pragma function(memcmp)
int __cdecl memcmp(const void * src1, const void * src2, size_t len)
{
  if (len == 0)
    return 0;

  const char * src_ptr1 = (const char *)src1;
  const char * src_ptr2 = (const char *)src2;

  while (--len && (*src_ptr1++ == *src_ptr2++));

  return (*((unsigned char *)src1) - *((unsigned char *)src2));
}

// ----------------------------------------------------------------------------

#pragma function(memset)
void * __cdecl memset(void * des, int val, size_t len)
{
#if defined (_M_IX86)
  __asm
  {
    push edi

    mov edi, des
    mov ecx, len
    mov eax, val
    rep stosb

    pop edi
  }

#elif defined (_M_IA64) || defined (_M_AMD64)
  __declspec(dllimport) void RtlFillMemory(void *, size_t, char);

  RtlFillMemory(des, len, (char)val);

#else
  char * des_ptr = (char *)des;
  char v = (char)val;

  while (len--)
    *des_ptr++ = v;

#endif

  return des;
}

// ----------------------------------------------------------------------------

#pragma function(memcpy)
void * __cdecl memcpy(void * des, const void * src, size_t len)
{
#if defined (_M_IX86)
  __asm
  {
    push edi
    push esi

    mov esi, src
    mov edi, des
    mov ecx, len
    rep movsb

    pop esi
    pop edi
  }

#elif defined (_M_IA64) || defined (_M_AMD64)
  __declspec(dllimport) void RtlCopyMemory(void *, const void *, size_t);

  RtlCopyMemory(des, src, len);

#else
  if (des == src)
    return;

  char * src_ptr = (char *)src;
  char * des_ptr = (char *)des;

  while (len--)
    *src_ptr++ = *des_ptr++;

#endif

  return des;
}

// ----------------------------------------------------------------------------

void * __cdecl memmove(void * des, const void * src, size_t len)
{
#if defined (_M_IX86)
  __asm
  {
    push edi
    push esi

    mov esi, src
    mov edi, des
    mov ecx, len

    cmp edi, esi
    je $exit // esi = edi
    ja $down // esi > edi

    rep movsb
    jmp $exit

  $down:
    lea esi, [esi+ecx-4]
    lea edi, [edi+ecx-4]

    shr ecx, 2
    std
    rep movsd

    mov ecx, len
    and ecx, 3
    add esi, 3
    add edi, 3
    rep movsb
    cld

  $exit:
    pop esi
    pop edi
  }

#elif defined (_M_IA64) || defined (_M_AMD64)
  __declspec(dllimport) void RtlMoveMemory(void *, const void *, size_t);

  RtlMoveMemory(des, src, len);

#else
  if (src == des)
    return;

  char * src_ptr = (char *)src;
  char * des_ptr = (char *)des;

  if (des_ptr < src_ptr || des_ptr > (src_ptr + len))
  {
    while (len--)
      *des_ptr++ = *src_ptr++;
  }
  else
  {
    des_ptr = des_ptr + len - 1;
    src_ptr = src_ptr + len - 1;

    while (len--)
      *des_ptr-- = *src_ptr--;
  }

#endif

  return des;
}

// ----------------------------------------------------------------------------










// ----------------------------------------------------------------------------

/*
long Round(float f)
{
  long l;
  __asm
  {
    fld f
    fistp l
  }
  return l;
}
*/

// ----------------------------------------------------------------------------

extern "C"
{
  int _fltused = 0;

#ifdef _M_IX86
  /**/
  __declspec(naked) void _ftol2()
  {
    __asm
    {
      push        ebp
      mov         ebp,esp
      sub         esp,20h
      and         esp,0FFFFFFF0h
      fld         st(0)
      fst         dword ptr [esp+18h]
      fistp       qword ptr [esp+10h]
      fild        qword ptr [esp+10h]
      mov         edx,dword ptr [esp+18h]
      mov         eax,dword ptr [esp+10h]
      test        eax,eax
      je          integer_QnaN_or_zero
    arg_is_not_integer_QnaN:
      fsubp       st(1),st
      test        edx,edx
      jns         positive
      fstp        dword ptr [esp]
      mov         ecx,dword ptr [esp]
      xor         ecx,80000000h
      add         ecx,7FFFFFFFh
      adc         eax,0
      mov         edx,dword ptr [esp+14h]
      adc         edx,0
      jmp         localexit
    positive:
      fstp        dword ptr [esp]
      mov         ecx,dword ptr [esp]
      add         ecx,7FFFFFFFh
      sbb         eax,0
      mov         edx,dword ptr [esp+14h]
      sbb         edx,0
      jmp         localexit
    integer_QnaN_or_zero:
      mov         edx,dword ptr [esp+14h]
      test        edx,7FFFFFFFh
      jne         arg_is_not_integer_QnaN
      fstp        dword ptr [esp+18h]
      fstp        dword ptr [esp+18h]
    localexit:
      leave
      ret
    }
  }
  /**/
#endif /* _M_IX86 */
}

// ----------------------------------------------------------------------------
