//---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>

#ifdef _USE_STDLIB
#include <malloc.h>
#else
#include "StdUtils.h"
#endif  // _USE_STDLIB

#include "StrUtils.h"
#include "ConUtils.h"
//---------------------------------------------------------------------------
/***************************************************************************/
/* ShowCursor - Отображение/скрытие курсора                                */
/***************************************************************************/
BOOL ShowConsoleCursor(
  BOOL bVisible
  )
{
  HANDLE hOutput;
  CONSOLE_CURSOR_INFO cci;
  hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleCursorInfo(hOutput, &cci);
  cci.bVisible = bVisible;
  return SetConsoleCursorInfo(hOutput, &cci);
}
/***************************************************************************/
/* OutText - Вывод текста на экран консоли                                 */
/***************************************************************************/
BOOL OutText(
  const TCHAR *pszText
  )
{
  HANDLE hOutput;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD dwCharsWritten;
  DWORD dwLen;
  unsigned int cchText;
#ifndef _UNICODE
  char buf[1024];
  char *pchBuf = buf;
  BOOL bSuccess = FALSE;
#endif  // _UNICODE

  hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  if (!GetConsoleScreenBufferInfo(hOutput, &csbi))
    return FALSE;
  dwLen = (csbi.dwSize.Y - csbi.dwCursorPosition.Y - 1) * csbi.dwSize.X +
          (csbi.dwSize.X - csbi.dwCursorPosition.X);
  FillConsoleOutputCharacter(hOutput, _T(' '), dwLen, csbi.dwCursorPosition,
                             &dwCharsWritten);
  if ((pszText == NULL) || (*pszText == _T('\0')))
    return TRUE;
  cchText = lstrlen(pszText);
#ifdef _UNICODE
  if (!WriteConsoleOutputCharacter(hOutput, pszText, cchText,
                                   csbi.dwCursorPosition, &dwCharsWritten))
    return FALSE;
  FillConsoleOutputAttribute(hOutput, csbi.wAttributes, cchText,
                             csbi.dwCursorPosition, &dwCharsWritten);
  return TRUE;
#else
  if (cchText >= sizeof(buf))
  {
    pchBuf = (char *)malloc(cchText + 1);
    if (pchBuf == NULL)
      return FALSE;
  }
  if (CharToOem(pszText, pchBuf))
  {
    cchText = lstrlen(pchBuf);
    if (WriteConsoleOutputCharacter(hOutput, pchBuf, cchText,
                                    csbi.dwCursorPosition, &dwCharsWritten))
    {
      FillConsoleOutputAttribute(hOutput, csbi.wAttributes, cchText,
                                 csbi.dwCursorPosition, &dwCharsWritten);
      bSuccess = TRUE;
    }
  }
  if (pchBuf != buf) free(pchBuf);
  return bSuccess;
#endif  // _UNICODE
}
/***************************************************************************/
/* GetTextAttr - Получение атрибутов текста                                */
/***************************************************************************/
int GetTextAttr(void)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    return -1;
  return csbi.wAttributes;
}
/***************************************************************************/
/* SetTextAttr - Установка атрибутов текста                                */
/***************************************************************************/
int SetTextAttr(
  int attr
  )
{
  int oldattr;
  if (attr == -1)
    return -1;
  oldattr = GetTextAttr();
  if (SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)attr))
    return oldattr;
  return -1;
}
//---------------------------------------------------------------------------
