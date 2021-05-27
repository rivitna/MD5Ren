//---------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stddef.h>
#include <tchar.h>
#include "StdUtils.h"
#include "ConUtils.h"
#include "StrUtils.h"
#include "FileUtil.h"
#include "AppUtils.h"
#include "md5.h"
//---------------------------------------------------------------------------
#ifndef countof
#define countof(a)  (sizeof(a)/sizeof(a[0]))
#endif  // countof
//---------------------------------------------------------------------------
#ifndef _DEBUG
#pragma comment(linker, "/ENTRY:Start")
#pragma comment(linker, "/MERGE:.rdata=.text")
#ifdef _WIN64
#pragma comment(linker, "/NODEFAULTLIB:libc.lib")
#else
#pragma comment(linker, "/NODEFAULTLIB")
#endif  // _WIN64
#endif  // _DEBUG
//---------------------------------------------------------------------------
// Размер контрольной суммы MD5
#define MD5_DIGEST_SIZE            16
// Размер шестнадцатиричной строки с контрольной суммой MD5
#define MD5_DIGEST_HEX_STR_LENGTH  (MD5_DIGEST_SIZE << 1)
//---------------------------------------------------------------------------
// Размер буфера для данных файла
#ifdef _WIN64
#define FILE_DATA_BUFFER_SIZE  4096
#else
#define FILE_DATA_BUFFER_SIZE  2048
#endif  // _WIN64
//---------------------------------------------------------------------------
// Список имен файлов
typedef struct _FILENAME_LIST_DATA
{
  TCHAR  *pList;
  size_t  cchCount;
  size_t  cchCapacity;
} FILENAME_LIST_DATA, *PFILENAME_LIST_DATA;

// Статистика сканирования файлов
typedef struct _SCAN_STAT
{
  unsigned int  nNumFiles;
  unsigned int  nNumRenamed;
  unsigned int  nNumErrors;
  unsigned int  nNumWarnings;
} SCAN_STAT, *PSCAN_STAT;

// Контекст сканирования файлов
typedef struct _SCAN_CONTEXT
{
  FILENAME_LIST_DATA  fileNameListData;
  PSCAN_STAT          pStat;
} SCAN_CONTEXT, *PSCAN_CONTEXT;

// Контекст переименования файлов
typedef struct _RENAME_CONTEXT
{
  PSCAN_STAT        pStat;
  CRITICAL_SECTION  csFileLock;
} RENAME_CONTEXT, *PRENAME_CONTEXT;

// Параметры потока переименования файлов
typedef struct _RENAME_THREAD_PARAM
{
  const TCHAR     *pszFileName;
  PRENAME_CONTEXT  pCtx;
} RENAME_THREAD_PARAM, *PRENAME_THREAD_PARAM;
//---------------------------------------------------------------------------
// Переименование файлов
void RenameFiles(
  const TCHAR *pszDirPath
  );
//---------------------------------------------------------------------------
/***************************************************************************/
/* main                                                                    */
/***************************************************************************/
#ifdef _DEBUG
int _tmain(int argc, TCHAR *argv[])
#else
int main(const TCHAR *pszCmdLine)
#endif  // _DEBUG
{
#ifdef _DEBUG
  const TCHAR *pszCmdLine = ::GetCommandLine();
#endif  // _DEBUG

  TCHAR szDirPath[MAX_PATH + 1];
  size_t cch;

  // Получение параметров программы
 cch = GetArgument(pszCmdLine, 1, szDirPath, countof(szDirPath));
  if ((cch == 0) || (cch >= countof(szDirPath)))
  {
    // Справка
    Print(_T("\r\n") \
          _T("Usage: MD5Ren path\r\n") \
          _T("\r\n") \
          _T("  path - Path to scan files.\r\n") \
          _T("\r\n"));
    return 0;
  }

  // Проверка существования указанного пути
  if (!IsDirectoryExists(szDirPath))
  {
    // Вывод текста ошибки на экран
    PrintFmt(_T("Error: Directory \"%s\" not found.\r\n"), szDirPath);
    return 1;
  }

  // Переименование файлов
  RenameFiles(szDirPath);

  return 0;
}
/***************************************************************************/
/* Start                                                                   */
/***************************************************************************/
#ifndef _DEBUG
void Start(void)
{
  int ret = main(::GetCommandLine());
  ::ExitProcess((UINT)ret);
}
#endif  // _DEBUG
//---------------------------------------------------------------------------
/***************************************************************************/
/* GetFileList - Получение списка файлов                                   */
/*               (Возвращенный указатель необходимо освободить при помощи  */
/*                функции free)                                            */
/***************************************************************************/

BOOL CALLBACK DoAddFile(
  const TCHAR *pszFileName,
  unsigned long dwError,
  const WIN32_FIND_DATA *pFindData,
  void *pData
  )
{
  PSCAN_CONTEXT pScanCtx = (PSCAN_CONTEXT)pData;

  if (dwError != ERROR_SUCCESS)
  {
    pScanCtx->pStat->nNumErrors++;
    // Вывод текста ошибки на экран
    PrintFmt(_T("Error: Unable to scan \"%s\" (code = %lu).\r\n"),
             pszFileName, dwError);
    return TRUE;
  }

  // Подкаталог?
  if (pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
  {
    // Вывод имени каталога на экран
    OutText(pszFileName);
    return TRUE;
  }

  size_t cchFileName = ::lstrlen(pszFileName);
  if (cchFileName == 0)
    return TRUE;
  cchFileName++;

  size_t cchAvailable = pScanCtx->fileNameListData.cchCapacity -
                        pScanCtx->fileNameListData.cchCount;
  if (cchFileName + 1 > cchAvailable)
  {
    // Увеличение емкости списка имен файлов
    const size_t cchPart = (cchFileName + 1 + (256 - 1)) & ~(256 - 1);
    const size_t nNumParts = pScanCtx->fileNameListData.cchCapacity /
                             cchPart;
    const size_t nPartsToAdd = (nNumParts > 64) ? (nNumParts >> 2)
                                                : ((nNumParts > 8) ? 16 : 4);
    size_t cchNewCapacity = pScanCtx->fileNameListData.cchCapacity +
                            nPartsToAdd * cchPart;
    TCHAR *pList = (TCHAR *)realloc(pScanCtx->fileNameListData.pList,
                                    cchNewCapacity * sizeof(TCHAR));
    if (!pList)
    {
      pScanCtx->pStat->nNumErrors++;
      // Вывод текста ошибки на экран
      Print(_T("Error: Not enough memory.\r\n"));
      return FALSE;
    }
    pScanCtx->fileNameListData.pList = pList;
    pScanCtx->fileNameListData.cchCapacity = cchNewCapacity;
  }
  memcpy(pScanCtx->fileNameListData.pList +
         pScanCtx->fileNameListData.cchCount,
         pszFileName, cchFileName * sizeof(TCHAR));
  pScanCtx->fileNameListData.cchCount += cchFileName;
  pScanCtx->fileNameListData.pList[pScanCtx->fileNameListData.cchCount] =
    _T('\0');

  pScanCtx->pStat->nNumFiles++;

  return TRUE;
}

TCHAR *GetFileList(
  const TCHAR *pszDirPath,
  PSCAN_STAT pStat
  )
{
  SCAN_CONTEXT scanCtx;
  ZeroMemory(&scanCtx, sizeof(scanCtx));

  pStat->nNumFiles = 0;
  pStat->nNumRenamed = 0;
  pStat->nNumErrors = 0;
  pStat->nNumWarnings = 0;
  scanCtx.pStat = pStat;

  // Перечисление файлов
  EnumFiles(pszDirPath, NULL, &DoAddFile, &scanCtx);

  TCHAR *pFileNameList = scanCtx.fileNameListData.pList;
  if (pFileNameList)
  {
    // Обрезание "лишней" памяти
    if (scanCtx.fileNameListData.cchCapacity >
        scanCtx.fileNameListData.cchCount + 1)
    {
      void *pList = realloc(pFileNameList,
                            (scanCtx.fileNameListData.cchCount + 1) *
                            sizeof(TCHAR));
      if (pList)
        pFileNameList = (TCHAR *)pList;
    }
  }

  // Очистка экрана с текущей позиции
  OutText(NULL);

  return pFileNameList;
}
/***************************************************************************/
/* GetFileMD5Digest - Получение контрольной суммы MD5 файла                */
/***************************************************************************/
BOOL GetFileMD5Digest(
  const TCHAR *pszFileName,
  unsigned char digest[MD5_DIGEST_SIZE]
  )
{
  HANDLE hFile = ::CreateFile(pszFileName,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              FILE_FLAG_SEQUENTIAL_SCAN,
                              NULL);
  if (hFile == INVALID_HANDLE_VALUE)
    return FALSE;

  BYTE buf[FILE_DATA_BUFFER_SIZE];
  md5_state_t state;
  md5_init(&state);

  DWORD dwBytesRead;
  while (::ReadFile(hFile, buf, sizeof(buf), &dwBytesRead, NULL) &&
         (dwBytesRead != 0))
  {
    md5_append(&state, buf, dwBytesRead);
  }

  md5_finish(&state, digest);

  ::CloseHandle(hFile);
  return TRUE;
}
/***************************************************************************/
/* RenameFileThreadProc - Процедура потока переименования файла            */
/***************************************************************************/
DWORD WINAPI RenameFileThreadProc(
  LPVOID lpParameter
  )
{
  const TCHAR *pszFileName;
  PRENAME_CONTEXT pCtx;

  pszFileName = ((PRENAME_THREAD_PARAM)lpParameter)->pszFileName;
  pCtx = ((PRENAME_THREAD_PARAM)lpParameter)->pCtx;

  unsigned char digest[MD5_DIGEST_SIZE];

  // Получение контрольной суммы MD5 файла
  if (!GetFileMD5Digest(pszFileName, digest))
  {
    ::InterlockedIncrement((LONG *)&pCtx->pStat->nNumErrors);
    // Вывод текста ошибки на экран
    PrintFmt(_T("Error: Unable to compute MD5 for file \"%s\" ") \
             _T("(code = %lu).\r\n"),
             pszFileName, ::GetLastError());
    return 0;
  }

  TCHAR hex_digest[MD5_DIGEST_HEX_STR_LENGTH + 1];

  // Преобразование бинарных данных в шестнадцатиричную строку
  BinToHexStr(digest, sizeof(digest), hex_digest);

  const TCHAR *pName;

  pName = GetFileName(pszFileName);
  if (!pName)
  {
    ::InterlockedIncrement((LONG *)&pCtx->pStat->nNumErrors);
    // Вывод текста ошибки на экран
    PrintFmt(_T("Error: The filename \"%s\" syntax is incorrect.\r\n"),
             pszFileName);
    return 0;
  }

  // Проверка имени файла на соответствие контрольной сумме MD5
  const TCHAR *pExt = GetFileExt(pName);
  size_t cchName = (pExt) ? (pExt - pName) : ::lstrlen(pName);
  if ((cchName == MD5_DIGEST_HEX_STR_LENGTH) &&
      !StrCmpNI(hex_digest, pName, MD5_DIGEST_HEX_STR_LENGTH))
  {
    ::InterlockedIncrement((LONG *)&pCtx->pStat->nNumWarnings);
    // Вывод текста предупреждения на экран
    PrintFmt(_T("Warning: File \"%s\" is already renamed.\r\n"),
             pszFileName);
    return 0;
  }

  size_t cchDirPath = pName - pszFileName;
  TCHAR *pNewFileName;
  const size_t cchMaxNewFileName = cchDirPath + MD5_DIGEST_HEX_STR_LENGTH +
                                   countof(_T(".4294967295"));
  pNewFileName = (TCHAR *)malloc(cchMaxNewFileName * sizeof(TCHAR));
  if (!pNewFileName)
  {
    ::InterlockedIncrement((LONG *)&pCtx->pStat->nNumErrors);
    // Вывод текста ошибки на экран
    Print(_T("Error: Not enough memory.\r\n"));
    return 0;
  }
  memcpy(pNewFileName, pszFileName, cchDirPath * sizeof(TCHAR));
  memcpy(&pNewFileName[cchDirPath], hex_digest,
         (MD5_DIGEST_HEX_STR_LENGTH + 1) * sizeof(TCHAR));

  BOOL bIsNewFileExists = FALSE;

  ::EnterCriticalSection(&pCtx->csFileLock);

  if (IsFileExists(pNewFileName))
  {
    // Получение нового имени в случае существования файла с данным именем
    unsigned int nFileCount = 1;
    TCHAR *p = &pNewFileName[cchDirPath + MD5_DIGEST_HEX_STR_LENGTH];
    *p++ = _T('.');
    do
    {
      UIToStr(nFileCount, p);
      if (!IsFileExists(pNewFileName))
        break;
    }
    while (++nFileCount != 0);
    if (nFileCount == 0)
      bIsNewFileExists = TRUE;
  }

  ::LeaveCriticalSection(&pCtx->csFileLock);

  if (bIsNewFileExists)
  {
    ::InterlockedIncrement((LONG *)&pCtx->pStat->nNumErrors);
    free(pNewFileName);
    // Вывод текста ошибки на экран
    Print(_T("Error: Too many duplicates.\r\n"));
    return 0;
  }

  BOOL bSuccess;

  ::EnterCriticalSection(&pCtx->csFileLock);

  // Переименование файла
  bSuccess = ::MoveFile(pszFileName, pNewFileName);

  ::LeaveCriticalSection(&pCtx->csFileLock);

  if (bSuccess)
  {
    ::InterlockedIncrement((LONG *)&pCtx->pStat->nNumRenamed);
  }
  else
  {
    ::InterlockedIncrement((LONG *)&pCtx->pStat->nNumErrors);
    // Вывод текста ошибки на экран
    PrintFmt(_T("Error: Unable to rename file \"%s\" (code = %lu).\r\n"),
             pszFileName, ::GetLastError());
  }

  free(pNewFileName);

  return 0;
}
/***************************************************************************/
/* GetNumberOfProcessors - Получение количества процессоров                */
/***************************************************************************/
unsigned int GetNumberOfProcessors()
{
  PULONG pulNumberProcessors;
#ifdef _WIN64
  pulNumberProcessors = (PULONG)(__readgsqword(0x30) + 0xB8);
#else
  pulNumberProcessors = (PULONG)(__readfsdword(0x30) + 0x64);
#endif  // _WIN64
  return *pulNumberProcessors;
}
/***************************************************************************/
/* RenameFiles - Переименование файлов                                     */
/***************************************************************************/
void RenameFiles(
  const TCHAR *pFileNameList,
  PSCAN_STAT pStat
  )
{
  size_t nMaxThreads;
  HANDLE *phThreads;
  RENAME_CONTEXT ctx;
  PRENAME_THREAD_PARAM phThreadParams;

  // Определение максимального количества потоков по количеству процессоров
#ifdef _WIN64
  SYSTEM_INFO systemInfo;
  ::GetSystemInfo(&systemInfo);
  nMaxThreads = systemInfo.dwNumberOfProcessors;
#else
  nMaxThreads = GetNumberOfProcessors();
#endif  // _WIN64
  if (nMaxThreads < 2)
    nMaxThreads = 2;

  phThreads = (HANDLE *)malloc(nMaxThreads * sizeof(HANDLE));
  phThreadParams = (PRENAME_THREAD_PARAM)malloc(nMaxThreads *
                                                sizeof(RENAME_THREAD_PARAM));
  if (!phThreads || !phThreadParams)
  {
    if (phThreadParams) free(phThreadParams);
    if (phThreads) free(phThreads);
    pStat->nNumErrors++;
    // Вывод текста ошибки на экран
    Print(_T("Error: Not enough memory.\r\n"));
    return;
  }

  ctx.pStat = pStat;
  // Инициализация критической секции блокировки файловых операций
  ::InitializeCriticalSection(&ctx.csFileLock);

  for (size_t i = 0; i < nMaxThreads; i++)
  {
    phThreads[i] = NULL;
    phThreadParams[i].pCtx = &ctx;
  }

  size_t nNumThreads = 0;
  size_t iThread = 0;
  unsigned int nFileCount = 0;
  const TCHAR *pszFileName = pFileNameList;
  while (*pszFileName)
  {
    // Создание потока переименования файла
    phThreadParams[iThread].pszFileName = pszFileName;
    phThreads[iThread] = ::CreateThread(NULL, 0, &RenameFileThreadProc,
                                        (LPVOID)&phThreadParams[iThread],
                                        0, NULL);
    if (!phThreads[iThread])
    {
      ::InterlockedIncrement((LONG *)&pStat->nNumErrors);
      // Вывод текста ошибки на экран
      PrintFmt(_T("Error: Failed to create thread (code = %lu).\r\n"),
               ::GetLastError());
      break;
    }

    nNumThreads++;

    if (nNumThreads < nMaxThreads)
    {
      iThread++;
    }
    else
    {
      // Ожидание завершившегося потока
      DWORD dwWait = ::WaitForMultipleObjects((DWORD)nMaxThreads, phThreads,
                                              FALSE, INFINITE);
      if (dwWait >= WAIT_OBJECT_0 + nMaxThreads)
      {
        ::InterlockedIncrement((LONG *)&pStat->nNumErrors);
        // Вывод текста ошибки на экран
        Print(_T("Error: Failed to wait until thread has terminated.\r\n"));
        break;
      }
      iThread = dwWait - WAIT_OBJECT_0;
      ::CloseHandle(phThreads[iThread]);
      phThreads[iThread] = NULL;
      nNumThreads--;
    }

    nFileCount++;

    // Вывод информации о количестве обрабатываемых файлов
    TCHAR szStat[32];
    TCHAR *pch = szStat;
    pch += UIToStr((unsigned int)nFileCount, pch);
    *pch++ = _T('/');
    UIToStr(pStat->nNumFiles, pch);
    OutText(szStat);

    pszFileName += ::lstrlen(pszFileName) + 1;
  }

  // Ожидание завершения оставшихся потоков
  if (nNumThreads < nMaxThreads)
  {
    size_t i = 0;
    for (size_t j = nMaxThreads; i < j; i++)
    {
      if (!phThreads[i])
      {
        while ((--j != i) && !phThreads[j]);
        if (j == i)
          break;
        phThreads[i] = phThreads[j];
        phThreads[j] = NULL;
      }
    }
    nNumThreads = i;
  }
  ::WaitForMultipleObjects((DWORD)nNumThreads, phThreads, TRUE, INFINITE);

  for (size_t i = 0; i < nNumThreads; i++)
    ::CloseHandle(phThreads[i]);

  // Уничтожение критической секции блокировки списка файлов
  ::DeleteCriticalSection(&ctx.csFileLock);

  free(phThreadParams);
  free(phThreads);

  // Очистка экрана с текущей позиции
  OutText(NULL);
}
/***************************************************************************/
/* RenameFiles - Переименование файлов                                     */
/***************************************************************************/
void RenameFiles(
  const TCHAR *pszDirPath
  )
{
  SCAN_STAT stat;

  // Получение списка файлов
  TCHAR *pFileNameList = GetFileList(pszDirPath, &stat);
  if (pFileNameList)
  {
    // Переименование файлов
    RenameFiles(pFileNameList, &stat);

    // Уничтожение списка файлов
    free(pFileNameList);
  }

  // Вывод статистики сканирования
  PrintFmt(_T("---------------------\r\n") \
           _T("Scanned:   %10u\r\n") \
           _T("Renamed:   %10u\r\n") \
           _T("Errors:    %10u\r\n") \
           _T("Warnings:  %10u\r\n"),
           stat.nNumFiles,
           stat.nNumRenamed,
           stat.nNumErrors,
           stat.nNumWarnings);
}
//---------------------------------------------------------------------------
