//---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#ifdef _USE_STDLIB
#include <malloc.h>
#include <memory.h>
#else
#include "StdUtils.h"
#endif  // _USE_STDLIB

#include "StrUtils.h"
//---------------------------------------------------------------------------
/***************************************************************************/
/* StrCharA - Поиск в строке первого символа                               */
/***************************************************************************/
char *StrCharA(
  const char *s,
  char ch
  )
{
  while (*s != '\0')
  {
    if (*s == ch)
      return (char *)s;
    s++;
  }
  return NULL;
}
/***************************************************************************/
/* StrCharW - Поиск в строке первого символа                               */
/***************************************************************************/
wchar_t *StrCharW(
  const wchar_t *s,
  wchar_t ch
  )
{
  while (*s != L'\0')
  {
    if (*s == ch)
      return (wchar_t *)s;
    s++;
  }
  return NULL;
}
/***************************************************************************/
/* StrCharSetA - Поиск в строке первого символа из набора                  */
/***************************************************************************/
char *StrCharSetA(
  const char *s,
  const char *cs
  )
{
  const char *p;
  while (*s != '\0')
  {
    p = cs;
    while (*p != '\0')
    {
      if (*s == *p)
        return (char *)s;
      p++;
    }
    s++;
  }
  return NULL;
}
/***************************************************************************/
/* StrCharSetW - Поиск в строке первого символа из набора                  */
/***************************************************************************/
wchar_t *StrCharSetW(
  const wchar_t *s,
  const wchar_t *cs
  )
{
  const wchar_t *p;
  while (*s != L'\0')
  {
    p = cs;
    while (*p != L'\0')
    {
      if (*s == *p)
        return (wchar_t *)s;
      p++;
    }
    s++;
  }
  return NULL;
}
/***************************************************************************/
/* StrRCharA - Поиск в строке последнего символа                           */
/***************************************************************************/
char *StrRCharA(
  const char *s,
  char ch
  )
{
  char *pch = NULL;
  while (*s != '\0')
  {
    if (*s == ch) pch = (char *)s;
    s++;
  }
  return pch;
}
/***************************************************************************/
/* StrRCharW - Поиск в строке последнего символа                           */
/***************************************************************************/
wchar_t *StrRCharW(
  const wchar_t *s,
  wchar_t ch
  )
{
  wchar_t *pch = NULL;
  while (*s != L'\0')
  {
    if (*s == ch) pch = (wchar_t *)s;
    s++;
  }
  return pch;
}
/***************************************************************************/
/* StrRCharSetA - Поиск в строке последнего символа из набора              */
/***************************************************************************/
char *StrRCharSetA(
  const char *s,
  const char *cs
  )
{
  char *pch = NULL;
  const char *p;
  while (*s != '\0')
  {
    p = cs;
    while (*p != '\0')
    {
      if (*s == *p)
      {
        pch = (char *)s;
        break;
      }
      p++;
    }
    s++;
  }
  return pch;
}
/***************************************************************************/
/* StrRCharSetW - Поиск в строке последнего символа из набора              */
/***************************************************************************/
wchar_t *StrRCharSetW(
  const wchar_t *s,
  const wchar_t *cs
  )
{
  wchar_t *pch = NULL;
  const wchar_t *p;
  while (*s != L'\0')
  {
    p = cs;
    while (*p != L'\0')
    {
      if (*s == *p)
      {
        pch = (wchar_t *)s;
        break;
      }
      p++;
    }
    s++;
  }
  return pch;
}
/***************************************************************************/
/* ToUpperA - Преобразование символа в верхний регистр                     */
/***************************************************************************/
char ToUpperA(
  char ch
  )
{
  if (((unsigned char)ch >= 'a') && ((unsigned char)ch <= 'z'))
    return (ch - ('a' - 'A'));
  return ch;
}
/***************************************************************************/
/* ToUpperW - Преобразование символа в верхний регистр                     */
/***************************************************************************/
wchar_t ToUpperW(
  wchar_t ch
  )
{
  if ((ch >= L'a') && (ch <= L'z'))
    return (ch - (L'a' - L'A'));
  return ch;
}
/***************************************************************************/
/* StrCmpNIA - Сравнение строк без учета регистра                          */
/***************************************************************************/
int StrCmpNIA(
  const char *s1,
  const char *s2,
  size_t count
  )
{
  int c1, c2;
  if (count == 0)
    return 0;
  do
  {
    if (((c1 = (unsigned char)(*(s1++))) >= 'A') && (c1 <= 'Z'))
      c1 += 'a' - 'A';
    if (((c2 = (unsigned char)(*(s2++))) >= 'A') && (c2 <= 'Z'))
      c2 += 'a' - 'A';
  }
  while ((--count != 0) && (c1 != 0) && (c1 == c2));
  return (c1 - c2);
}
/***************************************************************************/
/* StrCmpNIW - Сравнение строк без учета регистра                          */
/***************************************************************************/
int StrCmpNIW(
  const wchar_t *s1,
  const wchar_t *s2,
  size_t count
  )
{
  wchar_t c1, c2;
  if (count == 0)
    return 0;
  do
  {
    if (((c1 = (*(s1++))) >= L'A') && (c1 <= L'Z'))
      c1 += L'a' - L'A';
    if (((c2 = (*(s2++))) >= L'A') && (c2 <= L'Z'))
      c2 += L'a' - L'A';
  }
  while ((--count != 0) && (c1 != L'\0') && (c1 == c2));
  return (int)(c1 - c2);
}
/***************************************************************************/
/* StrCmpIA - Сравнение строк без учета регистра                           */
/***************************************************************************/
int StrCmpIA(
  const char *s1,
  const char *s2
  )
{
  int c1, c2;
  do
  {
    if (((c1 = (unsigned char)(*(s1++))) >= 'A') && (c1 <= 'Z'))
      c1 += 'a' - 'A';
    if (((c2 = (unsigned char)(*(s2++))) >= 'A') && (c2 <= 'Z'))
      c2 += 'a' - 'A';
  }
  while ((c1 != 0) && (c1 == c2));
  return (c1 - c2);
}
/***************************************************************************/
/* StrCmpIW - Сравнение строк без учета регистра                           */
/***************************************************************************/
int StrCmpIW(
  const wchar_t *s1,
  const wchar_t *s2
  )
{
  wchar_t c1, c2;
  do
  {
    if (((c1 = (*(s1++))) >= L'A') && (c1 <= L'Z'))
      c1 += L'a' - L'A';
    if (((c2 = (*(s2++))) >= L'A') && (c2 <= L'Z'))
      c2 += L'a' - L'A';
  }
  while ((c1 != L'\0') && (c1 == c2));
  return (int)(c1 - c2);
}
/***************************************************************************/
/* StrToULNA - Преобразование строки в беззнаковое целое                   */
/***************************************************************************/
unsigned long StrToULNA(
  const char *s,
  size_t count,
  char **endptr,
  int base
  )
{
  const char *p;
  unsigned long number;
  unsigned long digval;
  unsigned long maxval;
  size_t digitsread;
  if ((base < 2) || (base > 36))
  {
    if (endptr != NULL) *endptr = (char *)s;
    return 0UL;
  }
  p = s;
  number = 0;
  maxval = 0xFFFFFFFFUL / base;
  digitsread = 0;
  for (; (count != 0) && ((*p == ' ') || (*p == '\t')); count--, p++);
  if ((count != 0) && (*p == '+'))
  {
    p++;
    count--;
  }
  for (; count != 0; count--, p++)
  {
    digval = (*p | ' ') - '0';
    if ((long)digval < 0)
      break;
    if ((base > 10) && (digval > 9))
    {
      digval -= 'a' - '0' - 10;
      if ((long)digval < 10)
        break;
    }
    if ((long)digval >= base)
      break;
    digitsread++;
    if ((number > maxval) ||
        ((number == maxval) && (digval > 0xFFFFFFFFUL % base)))
      break;
    number = number * base + digval;
  }
  if (endptr != NULL) *endptr = (char *)((digitsread == 0) ? s : p);
  return number;
}
/***************************************************************************/
/* StrToULNW - Преобразование строки в беззнаковое целое                   */
/***************************************************************************/
unsigned long StrToULNW(
  const wchar_t *s,
  size_t count,
  wchar_t **endptr,
  int base
  )
{
  const wchar_t *p;
  unsigned long number;
  unsigned long digval;
  unsigned long maxval;
  size_t digitsread;
  if ((base < 2) || (base > 36))
  {
    if (endptr != NULL) *endptr = (wchar_t *)s;
    return 0UL;
  }
  p = s;
  number = 0;
  maxval = 0xFFFFFFFFUL / base;
  digitsread = 0;
  for (; (count != 0) && ((*p == L' ') || (*p == L'\t')); count--, p++);
  if ((count != 0) && (*p == L'+'))
  {
    p++;
    count--;
  }
  for (; count != 0; count--, p++)
  {
    digval = (*p | L' ') - L'0';
    if ((long)digval < 0)
      break;
    if ((base > 10) && (digval > 9))
    {
      digval -= L'a' - L'0' - 10;
      if ((long)digval < 10)
        break;
    }
    if ((long)digval >= base)
      break;
    digitsread++;
    if ((number > maxval) ||
        ((number == maxval) && (digval > 0xFFFFFFFFUL % base)))
      break;
    number = number * base + digval;
  }
  if (endptr != NULL) *endptr = (wchar_t *)((digitsread == 0) ? s : p);
  return number;
}
/***************************************************************************/
/* StrToULA - Преобразование строки в беззнаковое целое                    */
/***************************************************************************/
unsigned long StrToULA(
  const char *s,
  char **endptr,
  int base
  )
{
  // Преобразование строки в беззнаковое целое
  return StrToULNA(s, (size_t)-1, endptr, base);
}
/***************************************************************************/
/* StrToULW - Преобразование строки в беззнаковое целое                    */
/***************************************************************************/
unsigned long StrToULW(
  const wchar_t *s,
  wchar_t **endptr,
  int base
  )
{
  // Преобразование строки в беззнаковое целое
  return StrToULNW(s, (size_t)-1, endptr, base);
}
/***************************************************************************/
/* StrToUIA - Преобразование строки в беззнаковое целое                    */
/***************************************************************************/
unsigned int StrToUIA(
  const char *s
  )
{
  // Преобразование строки в беззнаковое целое
  return (unsigned int)StrToULA(s, NULL, 10);
}
/***************************************************************************/
/* StrToUIW - Преобразование строки в беззнаковое целое                    */
/***************************************************************************/
unsigned int StrToUIW(
  const wchar_t *s
  )
{
  // Преобразование строки в беззнаковое целое
  return (unsigned int)StrToULW(s, NULL, 10);
}
/***************************************************************************/
/* UIToStrA - Преобразование беззнакового целого в строку                  */
/***************************************************************************/
size_t UIToStrA(
  unsigned int nValue,
  char *pBuffer
  )
{
  size_t nCount;
  char *pch, *pch2;
  char ch;
  pch = pBuffer;
  do
  {
    *pch++ = '0' + (nValue % 10);
    nValue /= 10;
  }
  while (nValue != 0);
  *pch = '\0';
  nCount = pch - pBuffer;
  pch2 = pBuffer;
  while (pch2 < --pch)
  {
    ch = *pch;
    *pch = *pch2;
    *pch2++ = ch;
  }
  return nCount;
}
/***************************************************************************/
/* UIToStrW - Преобразование беззнакового целого в строку                  */
/***************************************************************************/
size_t UIToStrW(
  unsigned int nValue,
  wchar_t *pBuffer
  )
{
  size_t nCount;
  wchar_t *pch, *pch2;
  wchar_t ch;
  pch = pBuffer;
  do
  {
    *pch++ = L'0' + (nValue % 10);
    nValue /= 10;
  }
  while (nValue != 0);
  *pch = L'\0';
  nCount = pch - pBuffer;
  pch2 = pBuffer;
  while (pch2 < --pch)
  {
    ch = *pch;
    *pch = *pch2;
    *pch2++ = ch;
  }
  return nCount;
}
/***************************************************************************/
/* UI64ToStrA - Преобразование беззнакового целого в строку                */
/***************************************************************************/
size_t UI64ToStrA(
  unsigned __int64 nValue,
  char chThousandSeparator,
  char *pBuffer
  )
{
  size_t nCount;
  char *pch, *pch2;
  char ch;
  pch = pBuffer;
  nCount = 0;
  do
  {
    if (chThousandSeparator != '\0')
    {
      if (nCount == 3)
      {
        *pch++ = chThousandSeparator;
        nCount = 0;
      }
      nCount++;
    }
#if defined(_USE_STDLIB) || defined(_WIN64)
    *pch++ = '0' + (int)(nValue % 10);
    nValue /= 10;
#else
    *pch++ = '0' + (int)RemUI64(nValue, 10);
    nValue = DivUI64(nValue, 10);
#endif  // _USE_STDLIB || _WIN64
  }
  while (nValue != 0);
  *pch = '\0';
  nCount = pch - pBuffer;
  pch2 = pBuffer;
  while (pch2 < --pch)
  {
    ch = *pch;
    *pch = *pch2;
    *pch2++ = ch;
  }
  return nCount;
}
/***************************************************************************/
/* UI64ToStrW - Преобразование беззнакового целого в строку                */
/***************************************************************************/
size_t UI64ToStrW(
  unsigned __int64 nValue,
  wchar_t chThousandSeparator,
  wchar_t *pBuffer
  )
{
  size_t nCount;
  wchar_t *pch, *pch2;
  wchar_t ch;
  pch = pBuffer;
  nCount = 0;
  do
  {
    if (chThousandSeparator != L'\0')
    {
      if (nCount == 3)
      {
        *pch++ = chThousandSeparator;
        nCount = 0;
      }
      nCount++;
    }
#if defined(_USE_STDLIB) || defined(_WIN64)
    *pch++ = L'0' + (wchar_t)(nValue % 10);
    nValue /= 10;
#else
    *pch++ = L'0' + (wchar_t)RemUI64(nValue, 10);
    nValue = DivUI64(nValue, 10);
#endif  // _USE_STDLIB || _WIN64
  }
  while (nValue != 0);
  *pch = L'\0';
  nCount = pch - pBuffer;
  pch2 = pBuffer;
  while (pch2 < --pch)
  {
    ch = *pch;
    *pch = *pch2;
    *pch2++ = ch;
  }
  return nCount;
}
/***************************************************************************/
/* BinToHexStrA - Преобразование бинарных данных в шестнадцатиричную строку*/
/***************************************************************************/
void BinToHexStrA(
  const void *pData,
  size_t cbData,
  char *pBuffer
  )
{
  const char HEX_DIGITS[] = "0123456789ABCDEF";
  const unsigned char *pb = (const unsigned char *)pData;
  char *pch = pBuffer;
  for (; cbData != 0; pb++, cbData--)
  {
    *pch++ = HEX_DIGITS[*pb >> 4];
    *pch++ = HEX_DIGITS[*pb & 0xF];
  }
  *pch = '\0';
}
/***************************************************************************/
/* BinToHexStrW - Преобразование бинарных данных в шестнадцатиричную строку*/
/***************************************************************************/
void BinToHexStrW(
  const void *pData,
  size_t cbData,
  wchar_t *pBuffer
  )
{
  const wchar_t HEX_DIGITS[] = L"0123456789ABCDEF";
  const unsigned char *pb = (const unsigned char *)pData;
  wchar_t *pch = pBuffer;
  for (; cbData != 0; pb++, cbData--)
  {
    *pch++ = HEX_DIGITS[*pb >> 4];
    *pch++ = HEX_DIGITS[*pb & 0xF];
  }
  *pch = L'\0';
}
/***************************************************************************/
/* HexDigitValueA - Преобразование шестнадцатиричной цифры в число         */
/***************************************************************************/
int HexDigitValueA(
  char ch
  )
{
  int digval = (ch | ' ') - '0';
  if (digval < 0)
    return -1;
  if (digval > 9)
  {
    digval -= 'a' - '0' - 10;
    if ((digval < 10) || (digval > 15))
      return -1;
  }
  return digval;
}
/***************************************************************************/
/* HexDigitValueW - Преобразование шестнадцатиричной цифры в число         */
/***************************************************************************/
int HexDigitValueW(
  wchar_t ch
  )
{
  int digval = (ch | L' ') - L'0';
  if (digval < 0)
    return -1;
  if (digval > 9)
  {
    digval -= L'a' - L'0' - 10;
    if ((digval < 10) || (digval > 15))
      return -1;
  }
  return digval;
}
/***************************************************************************/
/* HexStrToBinA - Преобразование шестнадцатиричной строки в бинарные данные*/
/***************************************************************************/
size_t HexStrToBinA(
  const char *s,
  void *pBuffer,
  size_t cbSize
  )
{
  int n1, n2;
  unsigned char *p = (unsigned char *)pBuffer;
  for (; cbSize != 0; cbSize--)
  {
    if (((n1 = HexDigitValueA(*s++)) < 0) ||
        ((n2 = HexDigitValueA(*s++)) < 0))
      break;
    *p++ = (n1 << 4) | n2;
  }
  return (p - (unsigned char *)pBuffer);
}
/***************************************************************************/
/* HexStrToBinW - Преобразование шестнадцатиричной строки в бинарные данные*/
/***************************************************************************/
size_t HexStrToBinW(
  const wchar_t *s,
  void *pBuffer,
  size_t cbSize
  )
{
  int n1, n2;
  unsigned char *p = (unsigned char *)pBuffer;
  for (; cbSize != 0; cbSize--)
  {
    if (((n1 = HexDigitValueW(*s++)) < 0) ||
        ((n2 = HexDigitValueW(*s++)) < 0))
      break;
    *p++ = (n1 << 4) | n2;
  }
  return (p - (unsigned char *)pBuffer);
}
/***************************************************************************/
/* StrCchCopyN - Копирование строки в буфер                                */
/***************************************************************************/
size_t StrCchCopyN(
  TCHAR *pDest,
  size_t cchDest,
  const TCHAR *pSrc,
  size_t cchSrc
  )
{
  size_t cchCopy;
  if ((pDest == NULL) || (cchDest == 0))
    return (cchSrc + 1);
  cchCopy = min(cchSrc, cchDest - 1);
  memcpy(pDest, pSrc, cchCopy * sizeof(TCHAR));
  pDest[cchCopy] = _T('\0');
  if (cchSrc < cchDest)
    return cchSrc;
  return cchDest;
}
/***************************************************************************/
/* StrCchCopy - Копирование строки в буфер                                 */
/***************************************************************************/
size_t StrCchCopy(
  TCHAR *pDest,
  size_t cchDest,
  const TCHAR *pSrc
  )
{
  // Копирование строки в буфер
  return StrCchCopyN(pDest, cchDest, pSrc, lstrlen(pSrc));
}
/***************************************************************************/
/* StrCchMoveN - Перемещение строки в буфер                                */
/***************************************************************************/
size_t StrCchMoveN(
  TCHAR *pDest,
  size_t cchDest,
  const TCHAR *pSrc,
  size_t cchSrc
  )
{
  size_t cchMove;
  if ((pDest == NULL) || (cchDest == 0))
    return (cchSrc + 1);
  cchMove = min(cchSrc, cchDest - 1);
  memmove(pDest, pSrc, cchMove * sizeof(TCHAR));
  pDest[cchMove] = _T('\0');
  if (cchSrc < cchDest)
    return cchSrc;
  return cchDest;
}
/***************************************************************************/
/* StrCchMove - Перемещение строки в буфер                                 */
/***************************************************************************/
size_t StrCchMove(
  TCHAR *pDest,
  size_t cchDest,
  const TCHAR *pSrc
  )
{
  // Перемещение строки в буфер
  return StrCchMoveN(pDest, cchDest, pSrc, lstrlen(pSrc));
}
/***************************************************************************/
/* StrSet - Заполнение буфера указанным символом                           */
/***************************************************************************/
TCHAR *StrSet(
  TCHAR *s,
  TCHAR ch,
  size_t count
  )
{
#ifdef _UNICODE
  return (wchar_t *)wmemset(s, ch, count);
#else
  return (char *)memset(s, ch, count);
#endif  // _UNICODE
}
/***************************************************************************/
/* StrDel - Удаление подстроки из строки                                   */
/***************************************************************************/
void StrDel(
  TCHAR *s,
  size_t index,
  size_t count
  )
{
  size_t cch;
  if (count == 0)
    return;
  cch = lstrlen(s);
  if (index >= cch)
    return;
  if (count > cch - index) count = cch - index;
  memmove(s + index, s + index + count,
          (cch - (index + count) + 1) * sizeof(TCHAR));
}
/***************************************************************************/
/* StrCat - Объединение строк                                              */
/*          (Возвращенный указатель необходимо освободить при помощи       */
/*           функции free)                                                 */
/***************************************************************************/
TCHAR *StrCat(
  const TCHAR *s1,
  const TCHAR *s2
  )
{
  size_t cch1;
  size_t cch2;
  TCHAR *s;
  TCHAR *p;
  cch1 = lstrlen(s1);
  cch2 = lstrlen(s2);
  s = (TCHAR *)malloc((cch1 + cch2 + 1) * sizeof(TCHAR));
  if (s == NULL)
    return SetLastError(ERROR_NOT_ENOUGH_MEMORY), NULL;
  p = s;
  if (cch1 != 0)
  {
    memcpy(p, s1, cch1 * sizeof(TCHAR));
    p += cch1;
  }
  if (cch2 != 0)
  {
    memcpy(p, s2, cch2 * sizeof(TCHAR));
    p += cch2;
  }
  *p = _T('\0');
  return s;
}
/***************************************************************************/
/* FindArgument - Поиск аргумента                                          */
/***************************************************************************/
TCHAR *FindArgument(
  const TCHAR *s,
  size_t nArgNum
  )
{
  TCHAR chQuote;
  if ((s == NULL) || (*s == _T('\0')))
    return NULL;
  for (;;)
  {
    while ((*s == _T(' ')) || (*s == _T('\t'))) s++;
    if (*s == _T('\0'))
      return NULL;
    if (nArgNum == 0)
      break;
    if (*s == _T('\"'))
    {
      chQuote = _T('\"');
      s++;
    }
    else
    {
      chQuote = _T(' ');
    }
    do
    {
      while ((*s != _T('\0')) &&
             (*s != chQuote) && ((*s != _T('\t')) || (chQuote != _T(' '))))
        s++;
      if ((*s == _T('\"')) && (chQuote == _T('\"'))) s++;
    }
    while ((*s != _T('\0')) && (*s != _T(' ')) && (*s != _T('\t')));
    nArgNum--;
  }
  return (TCHAR *)s;
}
/***************************************************************************/
/* GetArgument - Получение аргумента                                       */
/***************************************************************************/
size_t GetArgument(
  const TCHAR *s,
  size_t nArgNum,
  TCHAR *pBuffer,
  size_t cchBuffer
  )
{
  TCHAR chQuote;
  const TCHAR *pchStart;
  const TCHAR *pchEnd;
  size_t cchArg;
  size_t cchCopy;
  if ((s == NULL) || (*s == _T('\0')))
    return 0;
  do
  {
    while ((*s == _T(' ')) || (*s == _T('\t'))) s++;
    if (*s == _T('\0'))
      return 0;
    if (*s == _T('\"'))
    {
      chQuote = _T('\"');
      s++;
    }
    else
    {
      chQuote = _T(' ');
    }
    pchStart = s;
    do
    {
      while ((*s != _T('\0')) &&
             (*s != chQuote) && ((*s != _T('\t')) || (chQuote != _T(' '))))
        s++;
      pchEnd = s;
      if ((*s == _T('\"')) && (chQuote == _T('\"'))) s++;
    }
    while ((*s != _T('\0')) && (*s != _T(' ')) && (*s != _T('\t')));
  }
  while (nArgNum-- != 0);
  cchArg = pchEnd - pchStart;
  if ((cchBuffer == 0) || (pBuffer == NULL))
    return (cchArg + 1);
  cchCopy = min(cchArg, cchBuffer - 1);
  memmove(pBuffer, pchStart, cchCopy * sizeof(TCHAR));
  pBuffer[cchCopy] = _T('\0');
  if (cchArg >= cchBuffer)
    return cchBuffer;
  return cchArg;
}
/***************************************************************************/
/* DelArgument - Удаление аргумента                                        */
/***************************************************************************/
void DelArgument(
  TCHAR *s,
  size_t nArgNum
  )
{
  TCHAR chQuote;
  TCHAR *p;
  TCHAR *pchStart;
  if ((s == NULL) || (*s == _T('\0')))
    return;
  p = s;
  do
  {
    pchStart = p;
    while ((*p == _T(' ')) || (*p == _T('\t'))) p++;
    if (*p == _T('\0'))
      return;
    if (*p == _T('\"'))
    {
      chQuote = _T('\"');
      p++;
    }
    else
    {
      chQuote = _T(' ');
    }
    do
    {
      while ((*p != _T('\0')) &&
             (*p != chQuote) && ((*p != _T('\t')) || (chQuote != _T(' '))))
        p++;
      if ((*p == _T('\"')) && (chQuote == _T('\"'))) p++;
    }
    while ((*p != _T('\0')) && (*p != _T(' ')) && (*p != _T('\t')));
  }
  while (nArgNum-- != 0);
  while ((*p == _T(' ')) || (*p == _T('\t'))) p++;
  if (*p != _T('\0')) p--;
  // Удаление подстроки из строки
  StrDel(s, pchStart - s, p - pchStart);
}
/***************************************************************************/
/* GetArgumentCount - Получение количества аргументов                      */
/***************************************************************************/
size_t GetArgumentCount(
  const TCHAR *s
  )
{
  size_t i = 0;
  while (GetArgument(s, i, NULL, 0) != 0) i++;
  return i;
}
//---------------------------------------------------------------------------
