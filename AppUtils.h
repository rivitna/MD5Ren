//---------------------------------------------------------------------------
#ifndef __APPUTILS_H__
#define __APPUTILS_H__
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
//---------------------------------------------------------------------------
// Открытие файла отчета
BOOL OpenLog(
  const TCHAR *pszFileName,
  BOOL bAppend
  );
// Закрытие файла отчета
void CloseLog(void);
// Запись в файл отчета
int Log(
  const TCHAR *pszMsg
  );
// Форматированная запись в файл отчета
int LogFmtV(
  const TCHAR *pszFmt,
  va_list arglist
  );
// Форматированная запись в файл отчета
int LogFmt(
  const TCHAR *pszFmt,
  ...
  );
// Добавление в файл отчета новой строки
int LogNewLine(void);
// Вывод строки в кодировке OEM на экран
int PrintOem(
  const char *pszOemText
  );
// Вывод строки на экран
int Print(
  const TCHAR *pszText
  );
// Форматированный вывод строки на экран
int PrintFmtV(
  const TCHAR *pszFmt,
  va_list arglist
  );
// Форматированный вывод строки на экран
int PrintFmt(
  const TCHAR *pszFmt,
  ...
  );
// Перевод на новую строку
int PrintNewLine(void);
// Запись в файл отчета и вывод строки на экран
void LogAndPrint(
  const TCHAR *pszText
  );
// Форматированная запись в файл отчета и вывод на экран
void LogAndPrintFmtV(
  const TCHAR *pszFmt,
  va_list arglist
  );
// Форматированная запись в файл отчета и вывод на экран
void LogAndPrintFmt(
  const TCHAR *pszFmt,
  ...
  );
// Добавление в файл отчета новой строки и перевод на новую строку
void LogAndPrintNewLine(void);
//---------------------------------------------------------------------------
#ifdef __cplusplus
}  // extern "C"
#endif // __cplusplus
//---------------------------------------------------------------------------
#endif // __APPUTILS_H__
