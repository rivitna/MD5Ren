//---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "StdUtils.h"
//---------------------------------------------------------------------------
/***************************************************************************/
/* AllocMem - Выделение буфера памяти                                      */
/***************************************************************************/
void *AllocMem(
  size_t size
  )
{
  return HeapAlloc(GetProcessHeap(), 0, size);
}
/***************************************************************************/
/* ReAllocMem - Изменение размера буфера памяти                            */
/***************************************************************************/
void *ReAllocMem(
  void *memblock,
  size_t size
  )
{
  if (memblock == NULL)
  {
    // Выделение буфера памяти
    return AllocMem(size);
  }
  return HeapReAlloc(GetProcessHeap(), 0, memblock, size);
}
/***************************************************************************/
/* AllocArray - Выделение памяти для массива элементов и инициализация их  */
/*              нулевыми значениями                                        */
/***************************************************************************/
void *AllocArray(
  size_t num,
  size_t size
  )
{
  return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, num * size);
}
/***************************************************************************/
/* FreeMem - Освобождение выделенного буфера памяти                        */
/***************************************************************************/
void FreeMem(
  void *memblock
  )
{
  HeapFree(GetProcessHeap(), 0, memblock);
}
/***************************************************************************/
/* FillMem - Заполнение буфера                                             */
/***************************************************************************/
#ifndef _WIN64
__declspec(naked)
#endif  // _WIN64
void*
#ifndef _WIN64
__stdcall
#endif  // _WIN64
FillMem(
  void *dest,
  unsigned char c,
  size_t count
  )
{
#ifdef _WIN64
  while (count != 0)
  {
    *(unsigned char *)dest = c;
    dest = (unsigned char *)dest + 1;
    count--;
  }
  return dest;
#else
  __asm
  {
    push  edi
    mov   edi,[esp+8]           // EDI = dest
    movzx eax,BYTE PTR [esp+0Ch]// EAX = c
    mov   edx,1010101h
    mul   edx                   // EAX = заполнитель
    mov   ecx,[esp+10h]         // ECX = count
    mov   edx,ecx               // EDX = count
    shr   ecx,2
    rep   stosd
    mov   ecx,edx
    and   ecx,3
    rep   stosb
    mov   eax,[esp+8]           // EAX = dest
    pop   edi
    ret   12
  }
#endif  // _WIN64
}
/***************************************************************************/
/* FillMemW - Заполнение буфера                                            */
/***************************************************************************/
#ifndef _WIN64
__declspec(naked)
#endif  // _WIN64
wchar_t*
#ifndef _WIN64
__stdcall
#endif  // _WIN64
FillMemW(
  wchar_t *dest,
  wchar_t c,
  size_t count
  )
{
#ifdef _WIN64
  while (count != 0)
  {
    *(wchar_t *)dest = c;
    dest = (wchar_t *)dest + 1;
    count--;
  }
  return dest;
#else
  __asm
  {
    push  edi
    mov   edi,[esp+8]           // EDI = dest
    mov   ax,[esp+0Ch]          // AX = c
    mov   ecx,[esp+10h]         // ECX = count
    rep   stosw
    mov   eax,[esp+8]           // EAX = dest
    pop   edi
    ret   12
  }
#endif  // _WIN64
}
/***************************************************************************/
/* ZeroMem - Обнуление буфера                                              */
/***************************************************************************/
#ifndef _WIN64
__declspec(naked)
#endif  // _WIN64
void __fastcall ZeroMem(
  void *dest,
  size_t count
  )
{
#ifdef _WIN64
  while (count != 0)
  {
    *(unsigned char *)dest = 0;
    dest = (unsigned char *)dest + 1;
    count--;
  }
#else
  __asm
  {
    push  edi
    mov   edi,ecx
    mov   ecx,edx
    xor   eax,eax
    shr   ecx,2
    rep   stosd
    mov   ecx,edx
    and   ecx,3
    rep   stosb
    pop   edi
    ret
  }
#endif  // _WIN64
}
/***************************************************************************/
/* CopyMem - Копирование блока памяти                                      */
/***************************************************************************/
#ifndef _WIN64
__declspec(naked)
#endif  // _WIN64
void*
#ifndef _WIN64
__stdcall
#endif  // _WIN64
CopyMem(
  void *dest,
  const void *src,
  size_t count
  )
{
#ifdef _WIN64
  while (count != 0)
  {
    *(unsigned char *)dest = *(unsigned char *)src;
    dest = (unsigned char *)dest + 1;
    src = (unsigned char *)src + 1;
    count--;
  }
  return dest;
#else
  __asm
  {
    push  esi
    push  edi
    mov   esi,[esp+10h]         // ESI = src
    mov   edi,[esp+0Ch]         // EDI = dest
    mov   ecx,[esp+14h]         // ECX = count
    mov   edx,ecx               // EDX = count
    shr   ecx,2
    rep   movsd
    mov   ecx,edx
    and   ecx,3
    rep   movsb
    mov   eax,[esp+0Ch]         // EAX = dest
    pop   edi
    pop   esi
    ret   12
  }
#endif  // _WIN64
}
/***************************************************************************/
/* MoveMem - Перемещение блока памяти                                      */
/***************************************************************************/
#ifndef _WIN64
__declspec(naked)
#endif  // _WIN64
void*
#ifndef _WIN64
__stdcall
#endif  // _WIN64
MoveMem(
  void *dest,
  const void *src,
  size_t count
  )
{
#ifdef _WIN64
  if ((dest <= src) ||
      ((unsigned char *)dest >= ((unsigned char *)src + count)))
  {
    while (count != 0)
    {
      *(unsigned char *)dest = *(unsigned char *)src;
      dest = (unsigned char *)dest + 1;
      src = (unsigned char *)src + 1;
      count--;
    }
  }
  else
  {
    dest = (unsigned char *)dest + count - 1;
    src = (unsigned char *)src + count - 1;
    while (count != 0)
    {
      *(unsigned char *)dest = *(unsigned char *)src;
      dest = (unsigned char *)dest - 1;
      src = (unsigned char *)src - 1;
      count--;
    }
  }
  return dest;
#else
  __asm
  {
    push  esi
    push  edi
    mov   ecx,[esp+14h]         // ECX = count
    jecxz Done
    mov   edx,ecx               // EDX = count
    mov   esi,[esp+10h]         // ESI = src
    mov   edi,[esp+0Ch]         // EDI = dest
    cmp   edi,esi               // dest < src?
    jb    Copy
    je    Done

    lea   eax,[esi+ecx-1]
    cmp   edi,eax
    ja    Copy
    mov   esi,eax
    lea   edi,[edi+ecx-1]
    std
    and   ecx,3
    rep   movsb
    mov   ecx,edx
    shr   ecx,2
    sub   esi,3
    sub   edi,3
    rep   movsd
    cld
    jmp   Done

Copy:
    shr   ecx,2
    rep   movsd
    mov   ecx,edx
    and   ecx,3
    rep   movsb

Done:
    mov   eax,[esp+0Ch]         // EDI = dest
    pop   edi
    pop   esi
    ret   12
  }
#endif  // _WIN64
}
/***************************************************************************/
/* EqualMem - Сравнение блоков памяти                                      */
/***************************************************************************/
#ifndef _WIN64
__declspec(naked)
#endif  // _WIN64
int /* BOOL */
#ifndef _WIN64
__stdcall
#endif  // _WIN64
EqualMem(
  const void *buf1,
  const void *buf2,
  size_t count
  )
{
#ifdef _WIN64
  while (count != 0)
  {
    if (*(unsigned char *)buf1 != *(unsigned char *)buf2)
      return 0 /* FALSE */;
    buf1 = (unsigned char *)buf1 + 1;
    buf2 = (unsigned char *)buf2 + 1;
    count--;
  }
  return 1 /* TRUE */;
#else
  __asm
  {
    push  esi
    push  edi

    xor   eax,eax

    mov   esi,[esp+0Ch]         // ESI = buf1
    mov   edi,[esp+10h]         // EDI = buf2
    cmp   esi,edi
    je    Equal
    mov   ecx,[esp+14h]         // ECX = count
    mov   edx,ecx               // EDX = count
    shr   ecx,2
    repe  cmpsd
    jne   Done
    mov   ecx,edx
    and   ecx,3
    repe  cmpsb
    jne   Done

Equal:
    inc   eax

Done:
    pop   edi
    pop   esi
    ret   12
  }
#endif  // _WIN64
}
/***************************************************************************/
/* MemChr - Поиск байта в блоке памяти                                     */
/***************************************************************************/
#ifndef _WIN64
__declspec(naked)
#endif  // _WIN64
void*
#ifndef _WIN64
__stdcall
#endif  // _WIN64
MemChr(
  const void *buf,
  int c,
  size_t count
  )
{
#ifdef _WIN64
  while (count != 0)
  {
    if (*(unsigned char *)buf == (unsigned char)c)
      return (void *)buf;
    buf = (unsigned char *)buf + 1;
    count--;
  }
  return NULL;
#else
  __asm
  {
    push  edi

    mov   ecx,[esp+10h]    // ECX = count
    jecxz NotFound
    mov   edi,[esp+8]      // EDI = buf
    mov   al,[esp+0Ch]     // AL = c
    repne scasb
    jne   NotFound
    lea   eax,[edi-1]

Done:
    pop   edi
    ret   12

NotFound:
    xor   eax,eax
    jmp   Done
  }
#endif  // _WIN64
}
/***************************************************************************/
/* SearchMem - Поиск цепочки байтов в блоке памяти                         */
/***************************************************************************/
#ifndef _WIN64
__declspec(naked)
#endif  // _WIN64
void*
#ifndef _WIN64
__stdcall
#endif  // _WIN64
SearchMem(
  const void *buf,
  size_t buflen,
  const void *s,
  size_t slen
  )
{
#ifdef _WIN64
  size_t i;
  size_t j;
  unsigned char *pb;
  unsigned char *ps;

  if ((slen == 0) || (slen > buflen))
    return NULL;
  i = buflen - slen + 1;
  do
  {
    j = slen;
    pb = (unsigned char *)buf;
    ps = (unsigned char *)s;
    while (*pb == *ps)
    {
      j--;
      if (j == 0)
        return (void *)buf;
      pb++;
      ps++;
    }
    buf = (unsigned char *)buf + 1;
    i--;
  } while (i != 0);
  return NULL;
#else
  __asm
  {
    push  ebp
    push  ebx
    push  esi
    push  edi

    mov   ebx,[esp+14h]    // EBX = buf
    mov   ebp,[esp+1Ch]    // EBP = s
    mov   edx,[esp+18h]    // EDX = buflen
    mov   eax,[esp+20h]    // EAX = slen
    or    eax,eax
    jz    NotFound
    sub   edx,eax
    jb    NotFound
    inc   edx              // EDX = число циклов

SearchLoop:
    mov   esi,ebp          // ESI = s
    mov   edi,ebx          // EDI = текущий указатель в буфере
    mov   ecx,eax          // ECX = slen
    shr   ecx,2
    repe  cmpsd
    jne   NextByte
    mov   ecx,eax
    and   ecx,3
    repe  cmpsb
    je    Done
NextByte:
    inc   ebx
    dec   edx
    jnz   SearchLoop

NotFound:
    xor   ebx,ebx

Done:
    mov   eax,ebx

    pop   edi
    pop   esi
    pop   ebx
    pop   ebp
    ret   16
  }
#endif  // _WIN64
}
#ifndef _WIN64
/***************************************************************************/
/* Mul64 - Умножение 64-разрядных чисел                                    */
/***************************************************************************/
__declspec(naked) __int64 __stdcall Mul64(
  __int64 multiplier,
  __int64 multiplicand
  )
{
  __asm
  {
    mov   eax,[esp+4+4]
    mov   ecx,[esp+12+4]
    or    ecx,eax               // test for both hiwords zero.
    mov   ecx,[esp+12]
    jnz   short hard            // both are zero, just mult ALO and BLO

    mov   eax,[esp+4]
    mul   ecx

    ret   16                    // callee restores the stack

hard:
    push  ebx

    mul   ecx                   // eax has AHI, ecx has BLO, so AHI * BLO
    mov   ebx,eax               // save result

    mov   eax,[esp+8]
    mul   dword ptr [esp+16+4]  // ALO * BHI
    add   ebx,eax               // ebx = ((ALO * BHI) + (AHI * BLO))

    mov   eax,[esp+8]           // ecx = BLO
    mul   ecx                   // so edx:eax = ALO*BLO
    add   edx,ebx               // now edx has all the LO*HI stuff

    pop   ebx

    ret   16                    // callee restores the stack
  }
}
/***************************************************************************/
/* DivUI64 - Деление 64-разрядных беззнаковых чисел                        */
/***************************************************************************/
__declspec(naked) unsigned __int64 __stdcall DivUI64(
  unsigned __int64 dividend,
  unsigned __int64 divisor
  )
{
  __asm
  {
    push  ebx
    push  esi

    mov   eax,[esp+20+4]        // check to see if divisor < 4194304K
    or    eax,eax
    jnz   short L1              // nope, gotta do this the hard way
    mov   ecx,[esp+20]          // load divisor
    mov   eax,[esp+12+4]        // load high word of dividend
    xor   edx,edx
    div   ecx                   // get high order bits of quotient
    mov   ebx,eax               // save high bits of quotient
    mov   eax,[esp+12]          // edx:eax <- remainder:lo word of dividend
    div   ecx                   // get low order bits of quotient
    mov   edx,ebx               // edx:eax <- quotient hi:quotient lo
    jmp   short L2              // restore stack and return

// Here we do it the hard way.  Remember, eax contains DVSRHI

L1:
    mov   ecx,eax               // ecx:ebx <- divisor
    mov   ebx,[esp+20]
    mov   edx,[esp+12+4]        // edx:eax <- dividend
    mov   eax,[esp+12]
L3:
    shr   ecx,1                 // shift divisor right one bit; hi bit <- 0
    rcr   ebx,1
    shr   edx,1                 // shift dividend right one bit; hi bit <- 0
    rcr   eax,1
    or    ecx,ecx
    jnz   short L3              // loop until divisor < 4194304K
    div   ebx                   // now divide, ignore remainder
    mov   esi,eax               // save quotient

// We may be off by one, so to check, we will multiply the quotient
// by the divisor and check the result against the orignal dividend
// Note that we must also check for overflow, which can occur if the
// dividend is close to 2**64 and the quotient is off by 1.

    mul   dword ptr [esp+20+4]  // QUOT * HIWORD(DVSR)
    mov   ecx,eax
    mov   eax,[esp+20]
    mul   esi                   // QUOT * LOWORD(DVSR)
    add   edx,ecx               // EDX:EAX = QUOT * DVSR
    jc    short L4              // carry means Quotient is off by 1

// do long compare here between original dividend and the result of the
// multiply in edx:eax.  If original is larger or equal, we are ok, otherwise
// subtract one (1) from the quotient.

    cmp   edx,[esp+12+4]        // compare hi words of result and original
    ja    short L4              // if result > original, do subtract
    jb    short L5              // if result < original, we are ok
    cmp   eax,[esp+12]          // hi words are equal, compare lo words
    jbe   short L5              // if less or equal we are ok, else subtract
L4:
    dec   esi                   // subtract 1 from quotient
L5:
    xor   edx,edx               // edx:eax <- quotient
    mov   eax,esi

// Just the cleanup left to do.  edx:eax contains the quotient.
// Restore the saved registers and return.

L2:
    pop   esi
    pop   ebx
    ret   16
  }
}
/***************************************************************************/
/* RemUI64 - Получения остатка от деления 64-разрядных беззнаковых чисел   */
/***************************************************************************/
__declspec(naked) unsigned __int64 __stdcall RemUI64(
  unsigned __int64 dividend,
  unsigned __int64 divisor
  )
{
  __asm
  {
    push  ebx

    mov   eax,[esp+16+4]        // check to see if divisor < 4194304K
    or    eax,eax
    jnz   short L1              // nope, gotta do this the hard way
    mov   ecx,[esp+16]          // load divisor
    mov   eax,[esp+8+4]         // load high word of dividend
    xor   edx,edx
    div   ecx                   // edx <- remainder, eax <- quotient
    mov   eax,[esp+8]           // edx:eax <- remainder:lo word of dividend
    div   ecx                   // edx <- final remainder
    mov   eax,edx               // edx:eax <- remainder
    xor   edx,edx
    jmp   short L2              // restore stack and return

// Here we do it the hard way.  Remember, eax contains DVSRHI

L1:
    mov   ecx,eax               // ecx:ebx <- divisor
    mov   ebx,[esp+16]
    mov   edx,[esp+8+4]         // edx:eax <- dividend
    mov   eax,[esp+8]
L3:
    shr   ecx,1                 // shift divisor right one bit; hi bit <- 0
    rcr   ebx,1
    shr   edx,1                 // shift dividend right one bit; hi bit <- 0
    rcr   eax,1
    or    ecx,ecx
    jnz   short L3              // loop until divisor < 4194304K
    div   ebx                   // now divide, ignore remainder

// We may be off by one, so to check, we will multiply the quotient
// by the divisor and check the result against the orignal dividend
// Note that we must also check for overflow, which can occur if the
// dividend is close to 2**64 and the quotient is off by 1.

    mov   ecx,eax               // save a copy of quotient in ECX
    mul   dword ptr [esp+16+4]
    xchg  ecx,eax               // put partial product in ECX, get quotient in EAX
    mul   dword ptr [esp+16]
    add   edx,ecx               // EDX:EAX = QUOT * DVSR
    jc    short L4              // carry means Quotient is off by 1

// do long compare here between original dividend and the result of the
// multiply in edx:eax.  If original is larger or equal, we're ok, otherwise
// subtract the original divisor from the result.

    cmp   edx,[esp+8+4]         // compare hi words of result and original
    ja    short L4              // if result > original, do subtract
    jb    short L5              // if result < original, we're ok
    cmp   eax,[esp+8]           // hi words are equal, compare lo words
    jbe   short L5              // if less or equal we're ok, else subtract
L4:
    sub   eax,[esp+16]          // subtract divisor from result
    sbb   edx,[esp+16+4]
L5:

// Calculate remainder by subtracting the result from the original dividend.
// Since the result is already in a register, we will perform the subtract in
// the opposite direction and negate the result to make it positive.

    sub   eax,[esp+8]           // subtract original dividend from result
    sbb   edx,[esp+8+4]
    neg   edx                   // and negate it
    neg   eax
    sbb   edx,0

// Just the cleanup left to do.  dx:ax contains the remainder.
// Restore the saved registers and return.

L2:
    pop   ebx
    ret   16
  }
}
#endif  // _WIN64
//---------------------------------------------------------------------------
