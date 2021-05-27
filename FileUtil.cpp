//---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>

#ifdef _USE_STDLIB
#include <malloc.h>
#include <memory.h>
#else
#include "StdUtils.h"
#endif  // _USE_STDLIB

#include "StrUtils.h"
#include "FileUtil.h"
//---------------------------------------------------------------------------
#ifndef countof
#define countof(a)  (sizeof(a)/sizeof(a[0]))
#endif  // countof
//---------------------------------------------------------------------------
// Размер буфера для содержимого переменной окружения PATH
#define ENV_VAR_PATH_BUFFER_SIZE  1024
//---------------------------------------------------------------------------
/***************************************************************************/
/* IsRelativePath - Проверка, является ли указанный путь относительным     */
/***************************************************************************/
BOOL IsRelativePath(
  const TCHAR *pszPath
  )
{
  if ((pszPath == NULL) || (*pszPath == _T('\0')))
    return TRUE;
  if (ISPATHDELIMITER(*pszPath))
    return FALSE;
  TCHAR ch = *pszPath | _T(' ');
  return ((ch < _T('a')) || (ch > _T('z')) || (pszPath[1] != _T(':')));
}
/***************************************************************************/
/* IsFileExists - Проверка существования файла                             */
/***************************************************************************/
BOOL IsFileExists(
  const TCHAR *pszFilePath
  )
{
  return (!(::GetFileAttributes(pszFilePath) & FILE_ATTRIBUTE_DIRECTORY));
}
/***************************************************************************/
/* IsDirectoryExists - Проверка существования каталога                     */
/***************************************************************************/
BOOL IsDirectoryExists(
  const TCHAR *pszDirPath
  )
{
  DWORD dwAttributes = ::GetFileAttributes(pszDirPath);
  if (dwAttributes == INVALID_FILE_ATTRIBUTES)
    return FALSE;
  return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY);
}
/***************************************************************************/
/* ForceDirectories - Создание иерархии каталогов                          */
/***************************************************************************/
BOOL ForceDirectories(
  const TCHAR *pszDirPath
  )
{
  if ((pszDirPath == NULL) || (*pszDirPath == _T('\0')))
    return FALSE;

  TCHAR szPath[MAX_PATH];
  void *pPathBuffer = NULL;
  TCHAR *pPath = szPath;

  size_t cchPath = ::lstrlen(pszDirPath) + 1;
  if (cchPath > countof(szPath))
  {
    pPathBuffer = malloc(cchPath * sizeof(TCHAR));
    if (pPathBuffer == NULL)
      return ::SetLastError(ERROR_NOT_ENOUGH_MEMORY), FALSE;
    pPath = (TCHAR *)pPathBuffer;
  }
  memcpy(pPath, pszDirPath, cchPath * sizeof(TCHAR));

  BOOL bError = FALSE;

  TCHAR *p = pPath;
  if (ISPATHDELIMITER(p[0]) && (p[1] == p[0]))
  {
    p += 2;
    for (int i = 0; i < 2; i++)
    {
      p = StrCharSet(p, _T("\\/"));
      if (p == NULL)
        break;
      p++;
    }
  }
  else
  {
    if ((p[0] != _T('\0')) && (p[1] == _T(':')) && ISPATHDELIMITER(p[2]))
      p += 3;
  }
  while ((p != NULL) && (*p != _T('\0')))
  {
    TCHAR chDelim;
    p = StrCharSet(p, _T("\\/"));
    if (p != NULL)
    {
      chDelim = *p;
      *p = _T('\0');
    }
    // Создание каталога
    if (!IsDirectoryExists(pPath) && !::CreateDirectory(pPath, NULL))
    {
      bError = TRUE;
      break;
    }
    if (p != NULL)
    {
      *p = chDelim;
      p++;
    }
  }

  if (pPathBuffer != NULL)
  {
    DWORD dwError;
    if (bError) dwError = ::GetLastError();
    free(pPathBuffer);
    if (bError) ::SetLastError(dwError);
  }
  return !bError;
}
/***************************************************************************/
/* GetFileName - Получение имени файла                                     */
/***************************************************************************/
TCHAR *GetFileName(
  const TCHAR *pszFilePath
  )
{
  if (pszFilePath == NULL)
    return NULL;
  TCHAR *pDelim = StrRCharSet(pszFilePath, _T(":\\/"));
  if (pDelim == NULL)
    return (TCHAR *)pszFilePath;
  return (pDelim + 1);
}
/***************************************************************************/
/* ExtractFilePath - Извлечение пути к файлу                               */
/***************************************************************************/
size_t ExtractFilePath(
  const TCHAR *pszFileName,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  if (pszFileName == NULL)
    return 0;
  TCHAR *pDelim = StrRCharSet(pszFileName, _T(":\\/"));
  if (pDelim == NULL)
    return 0;
  // Перемещение строки в буфер
  return StrCchMoveN(pBuffer, nSize, pszFileName, pDelim - pszFileName + 1);
}
/***************************************************************************/
/* GetFileExt - Получение расширения в имени файла                         */
/***************************************************************************/
TCHAR *GetFileExt(
  const TCHAR *pszFileName
  )
{
  if (pszFileName == NULL)
    return NULL;
  TCHAR *pExt = StrRCharSet(pszFileName, _T(":\\/."));
  if ((pExt != NULL) && (*pExt == _T('.')))
    return pExt;
  return NULL;
}
/***************************************************************************/
/* AddFileExt - Добавление расширения файла                                */
/***************************************************************************/
size_t AddFileExt(
  const TCHAR *pszFileName,
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  if ((pszExt == NULL) || (*pszExt == _T('\0')) ||
      (GetFileExt(pszFileName) != NULL))
    return 0;
  size_t cchFileName = ::lstrlen(pszFileName);
  size_t cchExt = ::lstrlen(pszExt);
  size_t cchRet = cchFileName + cchExt;
  if ((pBuffer == NULL) || (nSize == 0))
    return (cchRet + 1);
  memmove(pBuffer, pszFileName, min(cchFileName, nSize - 1) * sizeof(TCHAR));
  if (cchFileName < nSize - 1)
  {
    memcpy(pBuffer + cchFileName, pszExt,
           min(cchExt, nSize - (cchFileName + 1)) * sizeof(TCHAR));
  }
  pBuffer[min(cchRet, nSize - 1)] = _T('\0');
  if (cchRet < nSize)
    return cchRet;
  return nSize;
}
/***************************************************************************/
/* ChangeFileExt - Изменение расширения файла                              */
/***************************************************************************/
size_t ChangeFileExt(
  const TCHAR *pszFileName,
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  TCHAR *pExt = GetFileExt(pszFileName);
  size_t cchFileName = (pExt != NULL) ? (pExt - pszFileName)
                                      : ::lstrlen(pszFileName);
  size_t cchExt = ::lstrlen(pszExt);
  size_t cchRet = cchFileName + cchExt;
  if ((pBuffer == NULL) || (nSize == 0))
    return (cchRet + 1);
  memmove(pBuffer, pszFileName, min(cchFileName, nSize - 1) * sizeof(TCHAR));
  if (cchFileName < nSize - 1)
  {
    memcpy(pBuffer + cchFileName, pszExt,
           min(cchExt, nSize - (cchFileName + 1)) * sizeof(TCHAR));
  }
  pBuffer[min(cchRet, nSize - 1)] = _T('\0');
  if (cchRet < nSize)
    return cchRet;
  return nSize;
}
/***************************************************************************/
/* InternalCombinePath - Объединение путей                                 */
/***************************************************************************/
size_t InternalCombinePath(
  const TCHAR *pszDir,
  size_t cchDir,
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Указанный путь - относительный?
  if (!IsRelativePath(pszFile))
    return 0;
  size_t cchFile = ::lstrlen(pszFile);
  size_t cchPath = cchDir + cchFile;
  BOOL bAddDelimiter = ((cchDir != 0) && (cchFile != 0) &&
                        !ISPATHDELIMITER(pszDir[cchDir - 1]));
  if (bAddDelimiter) cchPath++;
  if ((pBuffer == NULL) || (nSize == 0))
    return (cchPath + 1);
  BOOL bDirInBuffer = ((pszDir >= pBuffer) && (pszDir < pBuffer + nSize));
  if (bDirInBuffer)
    memmove(pBuffer, pszDir, min(cchDir, nSize - 1) * sizeof(TCHAR));
  if (cchDir < nSize - 1)
  {
    size_t nFileOfs = cchDir;
    if (bAddDelimiter) nFileOfs++;
    memmove(pBuffer + nFileOfs, pszFile,
            min(cchFile, nSize - (nFileOfs + 1)) * sizeof(TCHAR));
    if (bAddDelimiter) pBuffer[cchDir] = _T('\\');
  }
  if (!bDirInBuffer)
    memcpy(pBuffer, pszDir, min(cchDir, nSize - 1) * sizeof(TCHAR));
  pBuffer[min(cchPath, nSize - 1)] = _T('\0');
  if (cchPath < nSize)
    return cchPath;
  return nSize;
}
/***************************************************************************/
/* CombinePath - Объединение путей                                         */
/***************************************************************************/
size_t CombinePath(
  const TCHAR *pszDir,
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Объединение путей
  return InternalCombinePath(pszDir, ::lstrlen(pszDir), pszFile,
                             pBuffer, nSize);
}
/***************************************************************************/
/* ChangeFileName - Изменение имени файла                                  */
/***************************************************************************/
size_t ChangeFileName(
  const TCHAR *pszFilePath,
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  TCHAR *pFile = GetFileName(pszFilePath);
  if (pFile == NULL)
    return 0;
  // Объединение путей
  return InternalCombinePath(pszFilePath, pFile - pszFilePath, pszFile,
                             pBuffer, nSize);
}
/***************************************************************************/
/* ChangeExeExt - Изменение расширения исполнимого файла                   */
/***************************************************************************/
size_t ChangeExeExt(
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Получение имени исполнимого файла
  TCHAR szExePath[MAX_PATH + 1];
  DWORD cch = ::GetModuleFileName(NULL, szExePath, countof(szExePath));
  if ((cch == 0) || (cch >= countof(szExePath)))
    return 0;
  // Изменение расширения файла
  return ChangeFileExt(szExePath, pszExt, pBuffer, nSize);
}
/***************************************************************************/
/* CombineWithExePath - Объединение относительного пути с путем            */
/*                      к исполнимому файлу                                */
/***************************************************************************/
size_t CombineWithExePath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Получение имени исполнимого файла
  TCHAR szExePath[MAX_PATH + 1];
  DWORD cch = ::GetModuleFileName(NULL, szExePath, countof(szExePath));
  if ((cch == 0) || (cch >= countof(szExePath)))
    return 0;
  // Изменение имени файла
  return ChangeFileName(szExePath, pszFile, pBuffer, nSize);
}
/***************************************************************************/
/* CombineWithTempPath - Объединение относительного пути с путем           */
/*                       к временной папке                                 */
/***************************************************************************/
size_t CombineWithTempPath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Получение имени исполнимого файла
  TCHAR szTempPath[MAX_PATH + 1];
  DWORD cch = ::GetTempPath(countof(szTempPath), szTempPath);
  if ((cch == 0) || (cch >= countof(szTempPath)))
    return 0;
  // Объединение путей
  return CombinePath(szTempPath, pszFile, pBuffer, nSize);
}
/***************************************************************************/
/* CombineWithWindowsPath - Объединение относительного пути с путем        */
/*                          к папке Windows                                */
/***************************************************************************/
size_t CombineWithWindowsPath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Получение пути к папке Windows
  TCHAR szWinDir[MAX_PATH];
  size_t cch = ::GetWindowsDirectory(szWinDir, countof(szWinDir));
  if ((cch == 0) || (cch >= countof(szWinDir)))
    return 0;
  // Объединение путей
  return CombinePath(szWinDir, pszFile, pBuffer, nSize);
}
/***************************************************************************/
/* CombineWithSystem32Path - Объединение относительного пути с путем       */
/*                           к системной папке 32-х разрядной Windows      */
/***************************************************************************/
size_t CombineWithSystem32Path(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Получение пути к системной папке 32-х разрядной Windows
  TCHAR szSysDir[MAX_PATH];
  size_t cch = ::GetSystemDirectory(szSysDir, countof(szSysDir));
  if ((cch == 0) || (cch >= countof(szSysDir)))
    return 0;
  // Объединение путей
  return CombinePath(szSysDir, pszFile, pBuffer, nSize);
}
/***************************************************************************/
/* CombineWithSystem16Path - Объединение относительного пути с путем       */
/*                           к системной папке 16-ти разрядной Windows     */
/***************************************************************************/
size_t CombineWithSystem16Path(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Получение пути к папке Windows
  TCHAR szSysDir[MAX_PATH];
  size_t cch;
  cch = ::GetWindowsDirectory(szSysDir, countof(szSysDir));
  if ((cch == 0) || (cch >= countof(szSysDir)))
    return 0;
  // Получение пути к системной папке 16-ти разрядной Windows
  cch = CombinePath(szSysDir, _T("System"), szSysDir, countof(szSysDir));
  if ((cch == 0) || (cch >= countof(szSysDir)))
    return 0;
  // Объединение путей
  return CombinePath(szSysDir, pszFile, pBuffer, nSize);
}
/***************************************************************************/
/* SearchFileInPATH - Поиск файла в списке путей переменной окружения PATH */
/***************************************************************************/
size_t SearchFileInPATH(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Указанный путь - относительный?
  if (!IsRelativePath(pszFile))
    return 0;

  TCHAR szPaths[ENV_VAR_PATH_BUFFER_SIZE];
  void *pPathsBuffer = NULL;
  TCHAR *pPaths;
  DWORD cchPaths;

  pPaths = szPaths;
  cchPaths = countof(szPaths);
  // Получение содержимого переменной окружения PATH
  DWORD cch = ::GetEnvironmentVariable(_T("PATH"), pPaths, cchPaths);
  if (cch > cchPaths)
  {
    // Выделение буфера для содержимого переменной окружения PATH
    pPathsBuffer = malloc(cch * sizeof(TCHAR));
    if (pPathsBuffer == NULL)
      return ::SetLastError(ERROR_NOT_ENOUGH_MEMORY), 0;
    pPaths = (TCHAR *)pPathsBuffer;
    cchPaths = cch;
    // Получение содержимого переменной окружения PATH
    cch = ::GetEnvironmentVariable(_T("PATH"), pPaths, cchPaths);
  }
  if ((cch == 0) || (cch > cchPaths))
  {
    if (pPathsBuffer != NULL)
    {
      DWORD dwError;
      if (cch == 0) dwError = ::GetLastError();
      free(pPathsBuffer);
      if (cch == 0) ::SetLastError(dwError);
    }
    return 0;
  }

  size_t nRet = 0;

  TCHAR szFilePath[MAX_PATH + 1];

  TCHAR *p = pPaths;
  for (;;)
  {
    while ((*p == _T(';')) || (*p == _T(' ')) || (*p == _T('\t'))) p++;
    if (*p == _T('\0'))
      break;
    TCHAR *pPath = p;
    TCHAR *pPathEnd = ++p;
    while ((*p != _T('\0')) && (*p != _T(';')))
    {
      if ((*p != _T(' ')) && (*p != _T('\t'))) pPathEnd = ++p;
      else p++;
    }
    if (*p == _T(';')) p++;
    *pPathEnd = _T('\0');
    // Проверка существования относительного пути в папке
    size_t cchFilePath = CombinePath(pPath, pszFile, szFilePath,
                                     countof(szFilePath));
    if ((cchFilePath != 0) && (cchFilePath < countof(szFilePath)))
    {
      // Проверка существования файла
      if (IsFileExists(szFilePath))
      {
        // Копирование строки в буфер
        nRet = StrCchCopyN(pBuffer, nSize, szFilePath, cchFilePath);
        break;
      }
    }
  }

  if (pPathsBuffer != NULL) free(pPathsBuffer);
  return nRet;
}
/***************************************************************************/
/* ExpandFilePath - Получение полного пути к файлу                         */
/***************************************************************************/
size_t ExpandFilePath(
  const TCHAR *pszPath,
  TCHAR *pBuffer,
  size_t nSize
  )
{
  // Указанный путь - полный?
  if (!IsRelativePath(pszPath))
  {
    // Перемещение строки в буфер
    return StrCchMove(pBuffer, nSize, pszPath);
  }

  TCHAR szFilePath[MAX_PATH + 1];
  size_t cchFilePath;

  for (;;)
  {
    // Расширение переменных окружения в пути к файлу
    szFilePath[0] = _T('\0');
    ::ExpandEnvironmentStrings(pszPath, szFilePath, countof(szFilePath));
    cchFilePath = ::lstrlen(szFilePath);
    if (cchFilePath != 0)
    {
      // Проверка существования файла
      if (IsFileExists(szFilePath))
        break;
    }

    // Проверка существования относительного пути в системной папке
    // 32-х разрядной Windows
    cchFilePath = CombineWithSystem32Path(pszPath, szFilePath,
                                          countof(szFilePath));
    if ((cchFilePath != 0) && (cchFilePath < countof(szFilePath)))
    {
      // Проверка существования файла
      if (IsFileExists(szFilePath))
        break;
    }

    // Проверка существования относительного пути в папке Windows
    cchFilePath = CombineWithWindowsPath(pszPath, szFilePath,
                                         countof(szFilePath));
    if ((cchFilePath != 0) && (cchFilePath < countof(szFilePath)))
    {
      // Проверка существования файла
      if (IsFileExists(szFilePath))
        break;
    }

    // Проверка существования относительного пути в системной папке
    // 16-ти разрядной Windows
    cchFilePath = CombineWithSystem16Path(pszPath, szFilePath,
                                          countof(szFilePath));
    if ((cchFilePath != 0) && (cchFilePath < countof(szFilePath)))
    {
      // Проверка существования файла
      if (IsFileExists(szFilePath))
        break;
    }

    // Поиск файла в списке путей переменной окружения PATH
    return SearchFileInPATH(pszPath, pBuffer, nSize);
  }

  // Копирование строки в буфер
  return StrCchCopyN(pBuffer, nSize, szFilePath, cchFilePath);
}
/***************************************************************************/
/* EnumFiles - Перечисление файлов                                         */
/***************************************************************************/
BOOL EnumFiles(
  const TCHAR *pszPath,
  const TCHAR *pszFileMask,
  PFNFILEENUMCALLBACK pfnCallback,
  void *pData
  )
{
  PWIN32_FIND_DATA pffd;
  size_t cchDir;
  BOOL bAddDelimiter;
  size_t nFilePathSize;
  TCHAR *pFilePath;
  TCHAR *pFileName;
  HANDLE hFind;
  BOOL bBreak;
  DWORD dwError;

  if (pfnCallback == NULL)
    return ::SetLastError(ERROR_INVALID_PARAMETER), FALSE;

  //if (::lstrlen(pszFileMask) >= countof(pffd->cFileName))
  //  return ::SetLastError(ERROR_INVALID_PARAMETER), FALSE;

  pffd = (PWIN32_FIND_DATA)malloc(sizeof(WIN32_FIND_DATA));
  if (pffd == NULL)
    return ::SetLastError(ERROR_NOT_ENOUGH_MEMORY), TRUE;

  cchDir = ::lstrlen(pszPath);
  bAddDelimiter = ((cchDir != 0) && !ISPATHDELIMITER(pszPath[cchDir - 1]));

  // Выделение буфера для пути к файлу
  nFilePathSize = cchDir + countof(pffd->cFileName);
  if (bAddDelimiter) nFilePathSize++;
  pFilePath = (TCHAR *)malloc(nFilePathSize * sizeof(TCHAR));
  if (pFilePath == NULL)
  {
    free(pffd);
    return ::SetLastError(ERROR_NOT_ENOUGH_MEMORY), TRUE;
  }

  memcpy(pFilePath, pszPath, cchDir * sizeof(TCHAR));
  pFileName = pFilePath + cchDir;
  if (bAddDelimiter) *pFileName++ = _T('\\');

  // Получение пути для поиска файлов
  if ((pszFileMask != NULL) && (*pszFileMask != _T('\0')))
  {
    ::lstrcpy(pFileName, pszFileMask);
  }
  else
  {
    // "*.*"
    pFileName[0] = pFileName[2] = _T('*');
    pFileName[1] = _T('.');
    pFileName[3] = _T('\0');
  }

  bBreak = FALSE;

  hFind = ::FindFirstFile(pFilePath, pffd);
  if (hFind != INVALID_HANDLE_VALUE)
  {
    do
    {
      // Пропуск текущего и родительского каталогов
      if (pffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        if ((pffd->cFileName[0] == _T('.')) &&
            ((pffd->cFileName[1] == _T('\0')) ||
             ((pffd->cFileName[1] == _T('.')) &&
              (pffd->cFileName[2] == _T('\0')))))
          continue;
      }
      // Пропуск символьных ссылок
      if (pffd->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
        continue;
      // Получение пути к файлу/подкаталогу
      ::lstrcpy(pFileName, pffd->cFileName);
      if (!pfnCallback(pFilePath, ERROR_SUCCESS, pffd, pData))
      {
        bBreak = TRUE;
        break;
      }
      if (pffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        // Рекурсивное перечисление файлов подкаталога
        if (!EnumFiles(pFilePath, pszFileMask, pfnCallback, pData))
        {
          bBreak = TRUE;
          break;
        }
        if ((dwError = ::GetLastError()) != ERROR_SUCCESS)
        {
          if (!pfnCallback(pFilePath, dwError, pffd, pData))
          {
            bBreak = TRUE;
            break;
          }
        }
      }
    } while (::FindNextFile(hFind, pffd));

    if (bBreak || ((dwError = ::GetLastError()) == ERROR_NO_MORE_FILES))
      dwError = ERROR_SUCCESS;

    ::FindClose(hFind);
  }
  else
  {
    dwError = ::GetLastError();
  }

  free(pFilePath);
  free(pffd);
  return ::SetLastError(dwError), !bBreak;
}
//---------------------------------------------------------------------------
