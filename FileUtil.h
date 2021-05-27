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
// ��������, �������� �� ��������� ���� �������������
BOOL IsRelativePath(
  const TCHAR *pszPath
  );
// �������� ������������� �����
BOOL IsFileExists(
  const TCHAR *pszFilePath
  );
// �������� ������������� ��������
BOOL IsDirectoryExists(
  const TCHAR *pszDirPath
  );
// �������� �������� ���������
BOOL ForceDirectories(
  const TCHAR *pszDirPath
  );
// ��������� ����� �����
TCHAR *GetFileName(
  const TCHAR *pszFilePath
  );
// ���������� ���� � �����
size_t ExtractFilePath(
  const TCHAR *pszFileName,
  TCHAR *pBuffer,
  size_t nSize
  );
// ��������� ���������� � ����� �����
TCHAR *GetFileExt(
  const TCHAR *pszFilePath
  );
// ���������� ���������� � ����� �����
size_t AddFileExt(
  const TCHAR *pszFileName,
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  );
// ��������� ���������� � ����� �����
size_t ChangeFileExt(
  const TCHAR *pszFileName,
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  );
// ����������� �����
size_t CombinePath(
  const TCHAR *pszDir,
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// ��������� ����� �����
size_t ChangeFileName(
  const TCHAR *pszFilePath,
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// ��������� ���������� ����������� �����
size_t ChangeExeExt(
  const TCHAR *pszExt,
  TCHAR *pBuffer,
  size_t nSize
  );
// ����������� �������������� ���� � ����� � ����������� �����
size_t CombineWithExePath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// ����������� �������������� ���� � ����� � ��������� �����
size_t CombineWithTempPath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// ����������� �������������� ���� � ����� � ����� Windows
size_t CombineWithWindowsPath(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// ����������� �������������� ���� � ����� � ��������� �����
// 32-� ��������� Windows
size_t CombineWithSystem32Path(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// ����������� �������������� ���� � ����� � ��������� �����
// 16-�� ��������� Windows
size_t CombineWithSystem16Path(
  const TCHAR *pszFile,
  TCHAR *pBuffer,
  size_t nSize
  );
// ����� ����� � ������ ����� ���������� ��������� PATH
size_t SearchFileInPATH(
  const TCHAR *pszPath,
  TCHAR *pBuffer,
  size_t nSize
  );
// ��������� ������� ���� � �����
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

// ������������ ������
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
