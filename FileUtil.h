//---------------------------------------------------------------------------
#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
//---------------------------------------------------------------------------
#define ISPATHDELIMITER(c)  ((c == _T('\\')) || (c == _T('/')))
//---------------------------------------------------------------------------
// Проверка, является ли указанный путь относительным
BOOL IsRelativePath(
  const TCHAR *pszPath
  );
// Проверка существования файла
BOOL IsFileExists(
  const TCHAR *pszFilePath
  );
// Проверка существования каталога
BOOL IsDirectoryExists(
  const TCHAR *pszDirPath
  );
// Создание иерархии каталогов
BOOL ForceDirectories(
  const TCHAR *pszDirPath
  );
// Получение имени файла
TCHAR *GetFileName(
  const TCHAR *pszFilePath
  );
// Извлечение пути к файлу
size_t ExtractFilePath(
  const TCHAR *pszFileName,
  TCHAR *pBuffer,
  size_t nSize
  );
// Получение расширения в имени файла
TCHAR *GetFileExt(
  const TCHAR *pszFilePath
  );
// Добавление расширения к имени файла
size_t AddFileExt(
  const TCHAR *pszFileName,
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  );
// Изменение расширения в имени файла
size_t ChangeFileExt(
  const TCHAR *pszFileName,
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  );
// Объединение путей
size_t CombinePath(
  const TCHAR *pszDir,
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// Изменение имени файла
size_t ChangeFileName(
  const TCHAR *pszFilePath,
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// Изменение расширения исполнимого файла
size_t ChangeExeExt(
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  );
// Объединение относительного пути с путем к исполнимому файлу
size_t CombineWithExePath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// Объединение относительного пути с путем к временной папке
size_t CombineWithTempPath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// Объединение относительного пути с путем к папке Windows
size_t CombineWithWindowsPath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// Объединение относительного пути с путем к системной папке
// 32-х разрядной Windows
size_t CombineWithSystem32Path(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// Объединение относительного пути с путем к системной папке
// 16-ти разрядной Windows
size_t CombineWithSystem16Path(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// Поиск файла в списке путей переменной окружения PATH
size_t SearchFileInPATH(
  const TCHAR *pszPath,
  TCHAR *pBuffer,
  size_t nSize
  );
// Получение полного пути к файлу
size_t ExpandFilePath(
  const TCHAR *pszPath,
  TCHAR *pBuffer,
  size_t nSize
  );

typedef BOOL (CALLBACK *PFNFILEENUMCALLBACK)(
  const TCHAR *pszFileName,
  unsigned long dwError,
  const WIN32_FIND_DATA *pFindData,
  void *pData
  );

// Перечисление файлов
BOOL EnumFiles(
  const TCHAR *pszPath,
  const TCHAR *pszFileMask,
  PFNFILEENUMCALLBACK pfnCallback,
  void *pData
  );
//---------------------------------------------------------------------------
#ifdef __cplusplus
}  // extern "C"
#endif // __cplusplus
//---------------------------------------------------------------------------
#endif  // __FILEUTIL_H__
