/***************************************************************************
Copyright (c) 2000-2008 by Nicolas Devillard.
MIT License

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

****************************************************************************/

/*-------------------------------------------------------------------------*/
/**
   @file    dictionary.h
   @author  N. Devillard
   @date    Sep 2007
   @version $Revision: 1.12 $
   @brief   Implements a dictionary for string variables.

   This module implements a simple dictionary object, i.e. a list
   of string/string associations. This object is useful to store e.g.
   informations retrieved from a configuration file (ini files).
*/
/*--------------------------------------------------------------------------*/

/*
    $Id: dictionary.h,v 1.12 2007-11-23 21:37:00 ndevilla Exp $
    $Author: ndevilla $
    $Date: 2007-11-23 21:37:00 $
    $Revision: 1.12 $
*/

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>          // For FILE * in prototypes

/*---------------------------------------------------------------------------
                                New types
 ---------------------------------------------------------------------------*/

// Section separator
#define SECTION_SEP         L':'
#define SECTION_SEP_STR     L":"

typedef enum tagERR_CODES
{
    ERRCODE_BUFFER_OVERFLOW,        // 00:  Buffer overflow
    ERRCODE_SYNTAX_ERROR,           // 01:  Syntax error
    ERRCODE_ALLOC_ERROR,            // 02:  Memory allocation error
} ERRCODES, *LPERRCODES;


/*-------------------------------------------------------------------------*/
/**
  @brief    Dictionary object

  This object contains a list of string/string associations. Each
  association is identified by a unique string key. Looking up values
  in the dictionary is speeded up by the use of a (hopefully collision-free)
  hash function.
 */
/*-------------------------------------------------------------------------*/
typedef struct tagDICTIONARY
{
    int      n;                 // 00:  Number of entries in dictionary
    int      size;              // 04:  Storage size of inifile
    LPWCHAR *val;               // 08:  Ptr to list of string values
    LPWCHAR *key;               // 0C:  Ptr to list of string keys
    LPUINT   hash;              // 10:  Ptr to list of hash values for keys
    LPWCHAR  inifile,           // 14:  Zero-terminated strings of the inifile
             lpwszDPFE;         // 18:  Ptr to the workspace DPFE
} DICTIONARY, *LPDICTIONARY;
#endif
