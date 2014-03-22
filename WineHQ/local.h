
#define GCL_HBRBACKGROUND       (-10)
#define NM_LAST                 (0U- 99U)

#undef  WM_KEYLAST

#ifndef WM_UNICHAR
#define WM_UNICHAR                      0x0109
#define WM_KEYLAST                      0x0109
#define UNICODE_NOCHAR                  0xFFFF
#else
#define WM_KEYLAST                      0x0108
#endif

#ifdef _WIN64
  typedef          __int64 APLI3264;    // Widest native signed value
  typedef unsigned __int64 APLU3264;    // ...           unsigned ...
#elif defined (_WIN32)
  typedef          __int32 APLI3264;    // Widest native signed value
  typedef unsigned __int32 APLU3264;    // ...           unsigned ...
#else
  #error Need code for this architecture.
#endif


#define strlenW         (UINT_PTR) lstrlenW
#define strcpyW         lstrcpyW

#define WINAPI          __stdcall
typedef int             BOOL,       *PBOOL,    *LPBOOL;
typedef int             INT,        *PINT,     *LPINT;
#define CHAR            char
#define UINT            unsigned int
typedef unsigned long   DWORD,      *PDWORD,   *LPDWORD;
typedef unsigned short  WCHAR,      *PWCHAR;
typedef CHAR           *PSTR,       *LPSTR,     *NPSTR;
typedef const CHAR     *PCSTR,      *LPCSTR;
typedef WCHAR          *PWSTR,      *LPWSTR,    *NWPSTR;
typedef const WCHAR    *PCWSTR,     *LPCWSTR;

#define CP_ACP          0
#define USE_IME         0

INT         WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,INT,LPWSTR,INT);
INT         WINAPI WideCharToMultiByte(UINT,DWORD,LPCWSTR,INT,LPSTR,INT,LPCSTR,LPBOOL);

void DbgBrk (void);
BOOL IzitFE (HWND);
extern int __cdecl dprintfWL0 (unsigned short *lpwszFmt,...);
extern int __cdecl dprintfWL9 (unsigned short *lpwszFmt,...);
#ifdef DEBUG
extern int gDbgLvl;
#endif

#define MP_RAT          int // For APLRAT in "types.h"
#define __mpfr_struct   int // For APLVFP in "types.h"
#include "enums.h"
#include "defines.h"
#define MP_INT          int
#define mp_bitcnt_t     int
#define mp_prec_t       int
#include "types.h"
#include "editctrl.h"
#include "resource.h"

#ifdef DEBUG
  #define   LCLODSAPI   ODSAPI
extern void __cdecl ODSAPI(char *lpStr,struct HWND__ *hWnd,unsigned int message,unsigned int wParam,long lParam);
#else
  #define   LCLODSAPI
#endif


