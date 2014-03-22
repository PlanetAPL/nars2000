/*
 * _stat() definitions
 *
 * Derived from the mingw header written by Colin Peters.
 * Modified for Wine use by Jon Griffiths and Francois Gouget.
 * This file is in the public domain.
 */
#ifndef __WINE_SYS_STAT_H
#define __WINE_SYS_STAT_H
#ifndef __WINE_USE_MSVCRT
#define __WINE_USE_MSVCRT
#endif

#include <sys/types.h>

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

#ifndef _DEV_T_DEFINED
typedef unsigned int _dev_t;
#define _DEV_T_DEFINED
#endif

#ifndef _INO_T_DEFINED
typedef unsigned short _ino_t;
#define _INO_T_DEFINED
#endif

#ifndef _TIME_T_DEFINED
typedef long time_t;
#define _TIME_T_DEFINED
#endif

#ifndef _OFF_T_DEFINED
typedef int _off_t;
#define _OFF_T_DEFINED
#endif

#define _S_IEXEC  0x0040
#define _S_IWRITE 0x0080
#define _S_IREAD  0x0100
#define _S_IFIFO  0x1000
#define _S_IFCHR  0x2000
#define _S_IFDIR  0x4000
#define _S_IFREG  0x8000
#define _S_IFMT   0xF000

/* for FreeBSD */
#undef st_atime
#undef st_ctime
#undef st_mtime

#ifndef _STAT_DEFINED
#define _STAT_DEFINED

struct _stat {
  _dev_t st_dev;
  _ino_t st_ino;
  unsigned short st_mode;
  short          st_nlink;
  short          st_uid;
  short          st_gid;
  _dev_t st_rdev;
  _off_t st_size;
  time_t st_atime;
  time_t st_mtime;
  time_t st_ctime;
};

struct stat {
  _dev_t st_dev;
  _ino_t st_ino;
  unsigned short st_mode;
  short          st_nlink;
  short          st_uid;
  short          st_gid;
  _dev_t st_rdev;
  _off_t st_size;
  time_t st_atime;
  time_t st_mtime;
  time_t st_ctime;
};

struct _stati64 {
  _dev_t st_dev;
  _ino_t st_ino;
  unsigned short st_mode;
  short          st_nlink;
  short          st_uid;
  short          st_gid;
  _dev_t st_rdev;
  __int64        st_size;
  time_t st_atime;
  time_t st_mtime;
  time_t st_ctime;
};
#endif /* _STAT_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif

int _fstat(int,struct _stat*);
int _stat(const char*,struct _stat*);
int _fstati64(int,struct _stati64*);
int _stati64(const char*,struct _stati64*);
int _umask(int);

#ifndef _WSTAT_DEFINED
#define _WSTAT_DEFINED
int _wstat(const wchar_t*,struct _stat*);
int _wstati64(const wchar_t*,struct _stati64*);
#endif /* _WSTAT_DEFINED */

#ifdef __cplusplus
}
#endif


#define S_IFMT   _S_IFMT
#define S_IFDIR  _S_IFDIR
#define S_IFCHR  _S_IFCHR
#define S_IFREG  _S_IFREG
#define S_IREAD  _S_IREAD
#define S_IWRITE _S_IWRITE
#define S_IEXEC  _S_IEXEC

#define	S_ISCHR(m)	(((m)&_S_IFMT) == _S_IFCHR)
#define	S_ISDIR(m)	(((m)&_S_IFMT) == _S_IFDIR)
#define	S_ISFIFO(m)	(((m)&_S_IFMT) == _S_IFIFO)
#define	S_ISREG(m)	(((m)&_S_IFMT) == _S_IFREG)

static inline int fstat(int fd, struct stat* ptr) { return _fstat(fd, (struct _stat*)ptr); }
static inline int stat(const char* path, struct stat* ptr) { return _stat(path, (struct _stat*)ptr); }
#ifndef _UMASK_DEFINED
static inline int umask(int fd) { return _umask(fd); }
#define _UMASK_DEFINED
#endif

#endif /* __WINE_SYS_STAT_H */
