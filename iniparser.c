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
   @file    iniparser.c
   @author  N. Devillard
   @date    Sep 2007
   @version 3.0
   @brief   Parser for ini files.
*/
/*--------------------------------------------------------------------------*/
/*
    $Id: iniparser.c,v 2.18 2008-01-03 18:35:39 ndevilla Exp $
    $Revision: 2.18 $
    $Date: 2008-01-03 18:35:39 $
*/
/*---------------------------- Includes ------------------------------------*/
#define STRICT
#include <windows.h>
#include <stdio.h>      // For sscanfW
#include <ctype.h>

#define MP_INT        int
#define MP_RAT        int
#define __mpfr_struct int

#include "macros.h"
#include "defines.h"
#include "types.h"
#include "dictionary.h"

// Include prototypes unless prototyping
#ifndef PROTO
#include "dictionary.pro"
#include "iniparser.pro"
#endif



/*---------------------------- Defines -------------------------------------*/
#define ASCIILINESZ         (1024)
#define INI_INVALID_KEY     ((LPWCHAR) -1)

#define Dbgbrk  DbgBrk
#define Dbgstop DbgStop
void FAR DbgBrk(void);


/*---------------------------------------------------------------------------
                        Private to this module
 ---------------------------------------------------------------------------*/
/**
 * This enum stores the status for each parsed line (internal use only).
 */
typedef enum tagLINE_STATUS
{
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} LINESTATUS;


/*-------------------------------------------------------------------------*/
/**
  @brief    Remove blanks at the beginning and the end of a string.
  @param    s   String to parse.
  @return   ptr to s.

  This function removes blanks from the beginning and end of a string,
  in place, returning a ptr to the first non-blank character.
 */
/*--------------------------------------------------------------------------*/
static LPWCHAR strstrip
    (LPWCHAR s)

{
    LPWCHAR last;

    if (s EQ NULL)
        return NULL;

    // Skip over leading spaces
    while (*s && isspaceW (*s))
        s++;
    // Point to the zero-terminator
    last = s + lstrlenW (s);

    // Skip over trailing spaces
    while (last > s)
    {
        if (!isspaceW (last[-1]))
            break;
        last--;
    } // End WHILE

    // Zap the last space
    *last = '\0';

    return s;
} // End strstrip


/*-------------------------------------------------------------------------*/
/**
  @brief    Get number of sections in a dictionary
  @param    d   Dictionary to examine
  @return   int Number of sections found in dictionary

  This function returns the number of sections found in a dictionary.
  The test to recognize sections is done on the string stored in the
  dictionary: a section name is given as "section" whereas a key is
  stored as "section:key", thus the test looks for entries that do not
  contain a colon.

  This clearly fails in the case a section name contains a colon, but
  this should simply be avoided.

  This function returns -1 in case of error.
 */
/*--------------------------------------------------------------------------*/
int iniparser_getnsec
    (LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    int i,
        nsec;

    Assert (lpDict NE NULL);

    nsec = 0;
    for (i = 0; i < lpDict->size; i++)
    {
        if (lpDict->key[i] EQ NULL)
            continue;

        if (strchrW (lpDict->key[i], SECTION_SEP) EQ NULL)
            nsec++;
    } // End FOR

    return nsec;
} // End iniparser_getnsec


/*-------------------------------------------------------------------------*/
/**
  @brief    Get name for section n in a dictionary.
  @param    d   Dictionary to examine
  @param    n   Section number (from 0 to nsec - 1).
  @return   Pointer to char string

  This function locates the n-th section in a dictionary and returns
  its name as a pointer to a string statically allocated inside the
  dictionary. Do not free or modify the returned string!

  This function returns NULL in case of error.
 */
/*--------------------------------------------------------------------------*/
LPWCHAR iniparser_getsecname
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     int          n)

{
    int i;
    int foundsec;

    Assert (lpDict NE NULL);
    Assert (n >= 0);

    foundsec = 0;
    for (i = 0; i < lpDict->size; i++)
    {
        if (lpDict->key[i] EQ NULL)
            continue;
        if (strchrW (lpDict->key[i], SECTION_SEP) EQ NULL)
        {
            foundsec++;
            if (foundsec > n)
                break;
        } // End IF
    } // End FOR

    if (foundsec <= n)
        return NULL;

    return lpDict->key[i];
} // End iniparser_getsecname


#ifdef DUMPDIC
/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump.
  @param    f   Opened file pointer to dump to.
  @return   void

  This function prints out the contents of a dictionary, one element by
  line, onto the provided file pointer. It is OK to specify @c stderr
  or @c stdout as output files. This function is meant for debugging
  purposes mostly.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dump
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     FILE         *f)

{
    int i;

    Assert (lpDict NE NULL);
    Assert (f      NE NULL);

    for (i = 0; i < lpDict->size; i++)
    {
        if (lpDict->key[i] EQ NULL)
            continue;
        if (lpDict->val[i] NE NULL)
            fprintf (f, "[%s]=[%s]\n", lpDict->key[i], lpDict->val[i]);
        else
            fprintf (f, "[%s]=UNDEF\n", lpDict->key[i]);
    } // End FOR
} // End iniparser_dump
#endif


#ifdef DUMPDIC
/*-------------------------------------------------------------------------*/
/**
  @brief    Save a dictionary to a loadable ini file
  @param    d   Dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given dictionary into a loadable ini file.
  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/
void iniparser_dump_ini
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     FILE         *f)

{
    int  i, j;
    char keym[ASCIILINESZ + 1];
    int  nsec;
    char *secname;
    int  seclen;

    Assert (lpDict NE NULL);
    Assert (f      NE NULL);

    nsec = iniparser_getnsec (lpDict);
    if (nsec < 1)
    {
        /* No section in file: dump all keys as they are */
        for (i = 0; i < lpDict->size; i++)
        {
            if (lpDict->key[i] EQ NULL)
                continue;
            fprintf (f, "%s = %s\n", lpDict->key[i], lpDict->val[i]);
        } // End FOR

        return;
    } // End IF

    for (i = 0; i < nsec; i++)
    {
        secname = iniparser_getsecname (lpDict, i);
        seclen  = lstrlenW (secname);
        fprintf (f, "\n[%s]\n", secname);
        wsprintfW (keym, "%s:", secname);
        for (j = 0; j < lpDict->size; j++)
        {
            if (lpDict->key[j] EQ NULL)
                continue;
            if (!strncmp (lpDict->key[j], keym, seclen + 1))
            {
                fprintf (f,
                         "%-30s = %s\n",
                         lpDict->key[j] + seclen + 1,
                         lpDict->val[j] ? d->val[j] : "");
            } // End IF
        } // End FOR
    } // End FOR

    fprintf (f, "\n");
} // End iniparser_dump_ini
#endif


/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key
  @param    d       Dictionary to search
  @param    key     Key string to look for
  @param    def     Default value to return if key not found.
  @return   pointer to statically allocated character string

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
LPWCHAR iniparser_getstring
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwKey,
     LPWCHAR      lpwDef)

{
    Assert (lpDict NE NULL);
    Assert (lpwKey NE NULL);

    return dictionary_get (lpDict, strlwrW (lpwKey), lpwDef);
} // End iniparser_getstring


/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to an int
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  Supported values for integers include the usual C notation
  so decimal, octal (starting with 0) and hexadecimal (starting with 0x)
  are supported. Examples:

  "42"      ->  42
  "042"     ->  34 (octal -> decimal)
  "0x42"    ->  66 (hexa  -> decimal)

  Warning: the conversion may overflow in various ways. Conversion is
  totally outsourced to strtol(), see the associated man page for overflow
  handling.

  Credits: Thanks to A. Becker for suggesting strtol()
 */
/*--------------------------------------------------------------------------*/
int iniparser_getint
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwKey,
     int          notfound)

{
    LPWCHAR lpwStr;

    Assert (lpDict NE NULL);
    Assert (lpwKey NE NULL);

    lpwStr = iniparser_getstring (lpDict, lpwKey, INI_INVALID_KEY);

    if (lpwStr EQ INI_INVALID_KEY)
        return notfound;

    return strtolW (lpwStr, NULL, 0);
} // End iniparser_getint


/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a double
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   double

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.
 */
/*--------------------------------------------------------------------------*/
double iniparser_getdouble
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwKey,
     double       notfound)

{
    LPWCHAR lpwStr;

    Assert (lpDict NE NULL);
    Assert (lpwKey NE NULL);

    lpwStr = iniparser_getstring (lpDict, lpwKey, INI_INVALID_KEY);

    if (lpwStr EQ INI_INVALID_KEY)
        return notfound;

    return atofW (lpwStr);
} // End iniparser_getdouble


/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a boolean
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  A true boolean is found if one of the following is matched:

  - A string starting with 'y'
  - A string starting with 'Y'
  - A string starting with 't'
  - A string starting with 'T'
  - A string starting with '1'

  A false boolean is found if one of the following is matched:

  - A string starting with 'n'
  - A string starting with 'N'
  - A string starting with 'f'
  - A string starting with 'F'
  - A string starting with '0'

  The notfound value returned if no boolean is identified, does not
  necessarily have to be 0 or 1.
 */
/*--------------------------------------------------------------------------*/
UBOOL iniparser_getboolean
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwKey,
     UBOOL        notfound)

{
    LPWCHAR lpwChar;
    UBOOL   ret;

    Assert (lpDict NE NULL);
    Assert (lpwKey NE NULL);

    lpwChar = iniparser_getstring (lpDict, lpwKey, INI_INVALID_KEY);

    if (lpwChar EQ INI_INVALID_KEY)
        return notfound;

    if (lpwChar[0] EQ 'y' || lpwChar[0] EQ 'Y' || lpwChar[0] EQ '1' || lpwChar[0] EQ 't' || lpwChar[0] EQ 'T')
        ret = TRUE;
    else
    if (lpwChar[0] EQ 'n' || lpwChar[0] EQ 'N' || lpwChar[0] EQ '0' || lpwChar[0] EQ 'f' || lpwChar[0] EQ 'F')
        ret = FALSE;
    else
        ret = notfound;

    return ret;
} // End iniparser_getboolean


/*-------------------------------------------------------------------------*/
/**
  @brief    Finds out if a given entry exists in a dictionary
  @param    ini     Dictionary to search
  @param    entry   Name of the entry to look for
  @return   integer 1 if entry exists, 0 otherwise

  Finds out if a given entry exists in the dictionary. Since sections
  are stored as keys with NULL associated values, this is the only way
  of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
UBOOL iniparser_find_entry
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwEntry)

{
    Assert (lpDict   NE NULL);
    Assert (lpwEntry NE NULL);

    return (iniparser_getstring (lpDict, lpwEntry, INI_INVALID_KEY) NE INI_INVALID_KEY);
} // End iniparser_getentry


#if FALSE
/*-------------------------------------------------------------------------*/
/**
  @brief    Set an entry in a dictionary.
  @param    ini     Dictionary to modify.
  @param    entry   Entry to modify (entry name)
  @param    val     New value to associate to the entry.
  @return   int 0 if Ok, -1 otherwise.

  If the given entry can be found in the dictionary, it is modified to
  contain the provided value. If it cannot be found, -1 is returned.
  It is Ok to set val to NULL.
 */
/*--------------------------------------------------------------------------*/
int iniparser_set
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwEntry,
     LPWCHAR      lpwVal)

{
    Assert (lpDict   NE NULL);
    Assert (lpwEntry NE NULL);

    return dictionary_set (lpDict, strlwrW (lpwEntry), lpwVal);
} // End iniparser_set
#endif


#if FALSE
/*-------------------------------------------------------------------------*/
/**
  @brief    Delete an entry in a dictionary
  @param    ini     Dictionary to modify
  @param    entry   Entry to delete (entry name)
  @return   void

  If the given entry can be found, it is deleted from the dictionary.
 */
/*--------------------------------------------------------------------------*/
void iniparser_unset
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwEntry)

{
    Assert (lpDict   NE NULL);
    Assert (lpwEntry NE NULL);

    dictionary_unset (lpDict, strlwrW (lpwEntry));
} // End iniparser_unset
#endif


/*-------------------------------------------------------------------------*/
/**
  @brief    Load a single line from an INI file
  @param    input_line  Input line, may be concatenated multi-line input
  @param    section     Output space to store section
  @param    key         Output space to store key
  @param    value       Output space to store value
  @return   LINESTATUS value
 */
/*--------------------------------------------------------------------------*/
static LINESTATUS iniparser_line
    (LPWCHAR  lpwLine,
     LPWCHAR *lplpSection,
     LPWCHAR *lplpKey,
     LPWCHAR *lplpVal)

{
    LINESTATUS lineStatus;
    int        len;
    LPWCHAR    lpwp;

    lpwLine = strstrip (lpwLine);
    len = lstrlenW (lpwLine);

    lineStatus = LINE_UNPROCESSED;
    if (len < 1)
        /* Empty line */
        lineStatus = LINE_EMPTY;
    else
    if (lpwLine[0] EQ L'#')
        /* Comment line */
        lineStatus = LINE_COMMENT;
    else
    if (lpwLine[0] EQ L'[' && lpwLine[len - 1] EQ L']')
    {
        /* Section name */
        lpwLine[len - 1] = L'\0';      // Zap the trailing bracket
        *lplpSection = strlwrW (strstrip (&lpwLine[1]));
        lineStatus = LINE_SECTION;
    } else
////if (sscanfW (lpwLine, "%[^=] = \"%[^\"]\"", key, val) EQ 2
//// || sscanfW (lpwLine, "%[^=] = '%[^\']'",   key, val) EQ 2
//// || sscanfW (lpwLine, "%[^=] = %[^;#]",     key, val) EQ 2)
    if ((lpwp = strchrW (lpwLine, L'=')) NE NULL)
    {
        *lplpKey = &lpwLine[0];
        *lpwp  = L'\0';
        *lplpVal = lpwp + 1;

        /* Usual key=value, with or without comments */
        *lplpKey = strlwrW (strstrip (*lplpKey));
        *lplpVal =          strstrip (*lplpVal);

        if (*lplpVal EQ L'\0')
            *lplpVal = NULL;
        /*
         * sscanfW cannot handle '' or "" as empty values
         * this is done here
         */
        if (!lstrcmpW (*lplpVal, L"\"\"")
         || !lstrcmpW (*lplpVal, L"''"))
            *lplpVal = *lplpKey + lstrlenW (*lplpKey);;
        lineStatus = LINE_VALUE;
    } else
        /* Generate syntax error */
        lineStatus = LINE_ERROR;

    return lineStatus;
} // End iniparser_line


/*-------------------------------------------------------------------------*/
/**
  @brief    Parse an ini file and return an allocated dictionary object
  @param    ininame Name of the ini file to read.
  @return   Pointer to newly allocated dictionary

  This is the parser for ini files. This function is called, providing
  the FILE ptr of file to be read. It returns a dictionary object that
  should not be accessed directly, but through accessor functions
  instead.

  The returned dictionary must be freed using iniparser_freedict().
 */
/*--------------------------------------------------------------------------*/
LPDICTIONARY iniparser_load
    (FILE      *in,                 // Ptr to file handle
     LPWCHAR    lpwszDPFE,          // Ptr to DPFE
     LPERRCODES lpErrCode,          // Ptr to error code (see ERRCODES)
     LPUINT     lpLineNo)           // Ptr to line # in error

{
    WCHAR        tmp[ASCIILINESZ + 1],
                 *lpwLine,
                 *lpwLineIni,           // Start of (multi-)line
                 *lpwSection,
                 *lpwKey,
                 *lpwVal;
    int          last,
                 line_len,
                 file_len,
                 errs = 0;
    LPDICTIONARY lpDict;                // Ptr to workspace dictionary
    UBOOL        bMultiLine = FALSE;    // TRUE iff this is a multiline

    // Allocate a new dictionary
    lpDict = dictionary_new (0);
    if (!lpDict)
        goto ERROR_EXIT;

    // Get the file length
    //   (why isn't this a single function such as fsize?)
    fseek (in, 0, SEEK_END);
    file_len = ftell (in);
    rewind (in);

    // Save the workspace DPFE
    lpDict->lpwszDPFE = lpwszDPFE;

    // Allocate space for the entire file
    lpDict->inifile = lpwLine = calloc (file_len + 1, sizeof (*lpwLine));

    *lpLineNo = 0;

    last = 0;

    while (lpwLine = &lpwLine[last], fgetsW (lpwLine, file_len - last, in) NE NULL)
    {
        // If not a multiline, ...
        if (!bMultiLine)
            // Initialize start of line
            lpwLineIni = lpwLine;

        (*lpLineNo)++;
        line_len = lstrlenW (lpwLine);
        last = line_len - 1;
        /* Safety check against buffer overflows */
        if (lpwLine[last] NE L'\n')
        {
            *lpErrCode = ERRCODE_BUFFER_OVERFLOW;

            goto ERROR_EXIT;
        } // End IF

        /* Get rid of \n and spaces at end of line */
        while (last >= 0
            && (lpwLine[last] EQ L'\n'
             || isspaceW (lpwLine[last])))
        {
            lpwLine[last] = L'\0';
            last--;
        } // End WHILE

        /* Detect multi-line */
        if (lpwLine[last] EQ L'\\')
        {
            /* Multi-line value */
            bMultiLine = TRUE;
            continue;
        } // End IF

        // Reset multi-line flag
        bMultiLine = FALSE;

        // Set offset to next position after L'\n'
        last = line_len;

        switch (iniparser_line (lpwLineIni, &lpwSection, &lpwKey, &lpwVal))
        {
            case LINE_EMPTY:
            case LINE_COMMENT:
                break;

            case LINE_SECTION:
                errs = dictionary_set (lpDict, lpwSection, NULL);

                break;

            case LINE_VALUE:
                wsprintfW (tmp, L"%s:%s", lpwSection, lpwKey);
                errs = dictionary_set (lpDict, tmp, lpwVal);

                break;

            case LINE_ERROR:
                *lpErrCode = ERRCODE_SYNTAX_ERROR;

                goto ERROR_EXIT;

            case LINE_UNPROCESSED:
            defstop
                Assert (FALSE);

                goto ERROR_EXIT;
        } // End SWITCH

        // Check for error from dictionary_set
        if (errs)
        {
            *lpErrCode = ERRCODE_ALLOC_ERROR;

            goto ERROR_EXIT;
        } // End IF
    } // End WHILE

    goto NORMAL_EXIT;

ERROR_EXIT:
    if (lpDict)
    {
        dictionary_del (lpDict); lpDict = NULL;
    } // End IF
NORMAL_EXIT:
    return lpDict;
} // End iniparser_load


/*-------------------------------------------------------------------------*/
/**
  @brief    Free all memory associated to an ini dictionary
  @param    d Dictionary to free
  @return   void

  Free all memory associated to an ini dictionary.
  It is mandatory to call this function before the dictionary object
  gets out of the current context.
 */
/*--------------------------------------------------------------------------*/
void iniparser_freedict
    (LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    Assert (lpDict NE NULL);

    dictionary_del (lpDict); lpDict = NULL;
} // End iniparser_freedict

/* vim: set ts=4 et sw=4 tw=75 */
