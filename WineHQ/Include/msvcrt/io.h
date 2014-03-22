/*
 * System I/O definitions.
 *
 * Derived from the mingw header written by Colin Peters.
 * Modified for Wine use by Jon Griffiths and Francois Gouget.
 * This file is in the public domain.
 */
#ifndef __WINE_IO_H
#define __WINE_IO_H
#ifndef __WINE_USE_MSVCRT
#define __WINE_USE_MSVCRT
#endif

#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif
#endif

#ifndef _MSC_VER
# ifndef __int64
#  define __int64 long long
# endif
#endif

/* The following are also defined in dos.h */
#define _A_NORMAL 0x00000000
#define _A_RDONLY 0x00000001
#define _A_HIDDEN 0x00000002
#define _A_SYSTEM 0x00000004
#define _A_VOLID  0x00000008
#define _A_SUBDIR 0x00000010
#define _A_ARCH   0x00000020

#ifndef _TIME_T_DEFINED
typedef long time_t;
#define _TIME_T_DEFINED
#endif

#ifndef _FSIZE_T_DEFINED
typedef unsigned long _fsize_t;
#define _FSIZE_T_DEFINED
#endif

#ifndef _FINDDATA_T_DEFINED
#define _FINDDATA_T_DEFINED
struct _finddata_t
{
  unsigned attrib;
  time_t   time_create;
  time_t   time_access;
  time_t   time_write;
  _fsize_t size;
  char             name[260];
};

struct _finddatai64_t
{
  unsigned attrib;
  time_t time_create;
  time_t time_access;
  time_t time_write;
  __int64        size;
  char           name[260];
};
#endif /* _FINDDATA_T_DEFINED */

#ifndef _WFINDDATA_T_DEFINED
#define _WFINDDATA_T_DEFINED
struct _wfinddata_t {
  unsigned attrib;
  time_t   time_create;
  time_t   time_access;
  time_t   time_write;
  _fsize_t size;
  wchar_t  name[260];
};

struct _wfinddatai64_t {
  unsigned attrib;
  time_t  time_create;
  time_t  time_access;
  time_t  time_write;
  __int64         size;
  wchar_t name[260];
};
#endif /* _WFINDDATA_T_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif

int         _access(const char*,int);
int         _chmod(const char*,int);
int         _chsize(int,long);
int         _close(int);
int         _commit(int);
int         _creat(const char*,int);
int         _dup(int);
int         _dup2(int,int);
int         _eof(int);
__int64     _filelengthi64(int);
long        _filelength(int);
int         _findclose(long);
long        _findfirst(const char*,struct _finddata_t*);
long        _findfirsti64(const char*, struct _finddatai64_t*);
int         _findnext(long,struct _finddata_t*);
int         _findnexti64(long, struct _finddatai64_t*);
long        _get_osfhandle(int);
int         _isatty(int);
int         _locking(int,int,long);
long        _lseek(int,long,int);
__int64     _lseeki64(int,__int64,int);
char*       _mktemp(char*);
int         _open(const char*,int,...);
int         _open_osfhandle(long,int);
int         _pipe(int*,unsigned int,int);
int         _read(int,void*,unsigned int);
int         _setmode(int,int);
int         _sopen(const char*,int,int,...);
long        _tell(int);
__int64     _telli64(int);
int         _umask(int);
int         _unlink(const char*);
int         _write(int,const void*,unsigned int);

int         remove(const char*);
int         rename(const char*,const char*);

#ifndef _WIO_DEFINED
#define _WIO_DEFINED
int         _waccess(const wchar_t*,int);
int         _wchmod(const wchar_t*,int);
int         _wcreat(const wchar_t*,int);
long        _wfindfirst(const wchar_t*,struct _wfinddata_t*);
long        _wfindfirsti64(const wchar_t*, struct _wfinddatai64_t*);
int         _wfindnext(long,struct _wfinddata_t*);
int         _wfindnexti64(long, struct _wfinddatai64_t*);
wchar_t*_wmktemp(wchar_t*);
int         _wopen(const wchar_t*,int,...);
int         _wrename(const wchar_t*,const wchar_t*);
int         _wsopen(const wchar_t*,int,int,...);
int         _wunlink(const wchar_t*);
#endif /* _WIO_DEFINED */

#ifdef __cplusplus
}
#endif


static inline int access(const char* path, int mode) { return _access(path, mode); }
static inline int chmod(const char* path, int mode) { return _chmod(path, mode); }
static inline int chsize(int fd, long size) { return _chsize(fd, size); }
static inline int close(int fd) { return _close(fd); }
static inline int creat(const char* path, int mode) { return _creat(path, mode); }
static inline int dup(int od) { return _dup(od); }
static inline int dup2(int od, int nd) { return _dup2(od, nd); }
static inline int eof(int fd) { return _eof(fd); }
static inline long filelength(int fd) { return _filelength(fd); }
static inline int isatty(int fd) { return _isatty(fd); }
static inline int locking(int fd, int mode, long size) { return _locking(fd, mode, size); }
static inline long lseek(int fd, long off, int where) { return _lseek(fd, off, where); }
static inline char* mktemp(char* pat) { return _mktemp(pat); }
static inline int read(int fd, void* buf, unsigned int size) { return _read(fd, buf, size); }
static inline int setmode(int fd, int mode) { return _setmode(fd, mode); }
static inline long tell(int fd) { return _tell(fd); }
#ifndef _UMASK_DEFINED
static inline int umask(int fd) { return _umask(fd); }
#define _UMASK_DEFINED
#endif
#ifndef _UNLINK_DEFINED
static inline int unlink(const char* path) { return _unlink(path); }
#define _UNLINK_DEFINED
#endif
static inline int write(int fd, const void* buf, unsigned int size) { return _write(fd, buf, size); }

#if defined(__GNUC__) && (__GNUC__ < 4)
extern int open(const char*,int,...) __attribute__((alias("_open")));
extern int sopen(const char*,int,int,...) __attribute__((alias("_sopen")));
#else
#define open _open
#define sopen _sopen
#endif /* __GNUC__ */

#endif /* __WINE_IO_H */
