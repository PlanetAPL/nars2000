/*
 * C++ exception handling facility
 *
 * Copyright 2000 Francois Gouget.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#ifndef __WINE_EH_H
#define __WINE_EH_H
#ifndef __WINE_USE_MSVCRT
#define __WINE_USE_MSVCRT
#endif

#if !defined(__cplusplus) && !defined(__WINE_MSVCRT_TEST)
#error "eh.h is meant only for C++ applications"
#endif

struct _EXCEPTION_POINTERS;

typedef void (*terminate_handler)(void);
typedef void (*terminate_function)(void);
typedef void (*unexpected_handler)(void);
typedef void (*unexpected_function)(void);
typedef void (*_se_translator_function)(unsigned int code, struct _EXCEPTION_POINTERS *info);

terminate_function set_terminate(terminate_function func);
unexpected_function set_unexpected(unexpected_function func);
_se_translator_function _set_se_translator(_se_translator_function func);

void        terminate(void);
void        unexpected(void);

#endif /* __WINE_EH_H */
