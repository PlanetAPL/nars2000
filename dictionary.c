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
   @file    dictionary.c
   @author  N. Devillard
   @date    Sep 2007
   @version $Revision: 1.27 $
   @brief   Implements a dictionary for string variables.

   This module implements a simple dictionary object, i.e. a list
   of string/string associations. This object is useful to store e.g.
   informations retrieved from a configuration file (ini files).
*/
/*--------------------------------------------------------------------------*/

/*
    $Id: dictionary.c,v 1.27 2007-11-23 21:39:18 ndevilla Exp $
    $Revision: 1.27 $
*/
/*---------------------------------------------------------------------------
                                Includes
 ---------------------------------------------------------------------------*/
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define MP_INT        int
#define MP_RAT        int
#define __mpfr_struct int

#include "macros.h"
#include "enums.h"
#include "defines.h"
#include "dictionary.h"

typedef unsigned int   uint32_t;

// Include prototypes unless prototyping
#ifndef PROTO
#include "dictionary.pro"
#include "iniparser.pro"
uint32_t hashlittleConv (const uint32_t * key, size_t length, uint32_t initval);
#endif

#ifdef DEBUG
extern void nop (void);
#endif

/** Minimal allocated number of entries in a dictionary */
#define DICTMINSZ   128

/*---------------------------------------------------------------------------
                            Private functions
 ---------------------------------------------------------------------------*/

/* Doubles the allocated size associated to a pointer */
/* 'size' is the current allocated size. */
static void *mem_double
    (void *ptr,
     int  size)

{
    void *newptr;

    newptr = calloc (2 * size, 1);
    if (newptr EQ NULL)
        return NULL;

    memcpy (newptr, ptr, size);
    free (ptr);

    return newptr;
} // End mem_double


/*---------------------------------------------------------------------------
                            Function codes
 ---------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief    Compute the hash key for a string.
  @param    key     Character string to use for key.
  @return   1 unsigned int on at least 32 bits.

  This hash function has been taken from an Article in Dr Dobbs Journal.
  This is normally a collision-free function, distributing keys evenly.
  The key is stored anyway in the struct so that collision can be avoided
  by comparing the key itself in last resort.
 */
/*--------------------------------------------------------------------------*/
UINT dictionary_hash
    (LPWCHAR lpwKey)

{
    // Hash the key
    return hashlittleConv
           ((const uint32_t *) lpwKey,                  // A ptr to the name to hash
             lstrlenW (lpwKey),                         // The # WCHARs pointed to
             0);                                        // Initial value or previous hash
} // End dictionary_hash


/*-------------------------------------------------------------------------*/
/**
  @brief    Create a new dictionary object.
  @param    size    Optional initial size of the dictionary.
  @return   1 newly allocated dictionary objet.

  This function allocates a new dictionary object of given size and returns
  it. If you do not know in advance (roughly) the number of entries in the
  dictionary, give size=0.
 */
/*--------------------------------------------------------------------------*/
LPDICTIONARY dictionary_new
    (int size)

{
    LPDICTIONARY lpDict;        // Ptr to workspace dictionary

    /* If no size was specified, allocate space for DICTMINSZ */
    if (size < DICTMINSZ)
        size = DICTMINSZ;

    lpDict = (LPDICTIONARY) calloc (1, sizeof (DICTIONARY));
    if (lpDict EQ NULL)
        return NULL;

    lpDict->size      = size;
    lpDict->val       = (LPWCHAR *) calloc (size, sizeof (*lpDict->val));
    lpDict->key       = (LPWCHAR *) calloc (size, sizeof (*lpDict->key));
    lpDict->hash      = (LPUINT)    calloc (size, sizeof (*lpDict->hash));
    lpDict->inifile   =
    lpDict->lpwszDPFE = NULL;

    return lpDict;
} // End dictionary_new


/*-------------------------------------------------------------------------*/
/**
  @brief    Delete a dictionary object
  @param    d   dictionary object to deallocate.
  @return   void

  Deallocate a dictionary object and all memory associated to it.
 */
/*--------------------------------------------------------------------------*/
void dictionary_del
    (LPDICTIONARY lpDict)       // Ptr to workspace dictionary

{
    int i;

    Assert (lpDict NE NULL);

    for (i = 0; i < lpDict->size; i++)
    {
        // If the key is non-empty and not within the file contents, ...
        if (lpDict->key[i] NE NULL
         && !(lpDict->inifile <= lpDict->key[i]
           &&                    lpDict->key[i] < (lpDict->inifile + lpDict->size)))
        {
            free (lpDict->key[i]); lpDict->key[i] = NULL;
        } // End IF

        // If the value is non-empty and not within the file contents, ...
        if (lpDict->val[i] NE NULL
         && !(lpDict->inifile <= lpDict->val[i]
           &&                    lpDict->val[i] < (lpDict->inifile + lpDict->size)))
        {
            free (lpDict->val[i]); lpDict->val[i] = NULL;
        } // End IF
    } // End FOR

    free (lpDict->val);     lpDict->val     = NULL;
    free (lpDict->key);     lpDict->key     = NULL;
    free (lpDict->hash);    lpDict->hash    = NULL;
    free (lpDict->inifile); lpDict->inifile = NULL;
    free (lpDict);          lpDict          = NULL;
} // End dictionary_del


/*-------------------------------------------------------------------------*/
/**
  @brief    Get a value from a dictionary.
  @param    d       dictionary object to search.
  @param    key     Key to look for in the dictionary.
  @param    def     Default value to return if key not found.
  @return   1 pointer to internally allocated character string.

  This function locates a key in a dictionary and returns a pointer to its
  value, or the passed 'def' pointer if no such key can be found in
  dictionary. The returned character pointer points to data internal to the
  dictionary object, you should not try to free it or modify it.
 */
/*--------------------------------------------------------------------------*/
LPWCHAR dictionary_get
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwKey,
     LPWCHAR      lpwDef,
     LPINT        lpIndex)      // Ptr to index on output (may be NULL)

{
    UINT hash;
    int  i;

    Assert (lpDict NE NULL);

    // If it's valid, ...
    if (lpIndex NE NULL)
        // Initialize it
        *lpIndex = -1;

    // Hash the key
    hash = dictionary_hash (lpwKey);

    for (i = 0; i < lpDict->size; i++)
    if (lpDict->key[i] NE NULL)
    {
        /* Compare hash */
        if (hash EQ lpDict->hash[i])
        {
            /* Compare string, to avoid hash collisions */
            if (!lstrcmpW (lpwKey, lpDict->key[i]))
            {
                if (lpIndex NE NULL)
                    *lpIndex = i;

                return lpDict->val[i];
            } // End IF
        } // End IF
    } // End FOR/IF

    return lpwDef;
} // End dictionary_get


/*-------------------------------------------------------------------------*/
/**
  @brief    Set a value in a dictionary.
  @param    d       dictionary object to modify.
  @param    key     Key to modify or add.
  @param    val     Value to add.
  @return   int     0 if Ok, anything else otherwise

  If the given key is found in the dictionary, the associated value is
  replaced by the provided one. If the key cannot be found in the
  dictionary, it is added to it.

  It is Ok to provide a NULL value for val, but NULL values for the dictionary
  or the key are considered as errors: the function will return immediately
  in such a case.

  Notice that if you dictionary_set a variable to NULL, a call to
  dictionary_get will return a NULL value: the variable will be found, and
  its value (NULL) is returned. In other words, setting the variable
  content to NULL is equivalent to deleting the variable from the
  dictionary. It is not possible (in this implementation) to have a key in
  the dictionary without value.

  This function returns non-zero in case of failure.
 */
/*--------------------------------------------------------------------------*/
int dictionary_set
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwKey,
     LPWCHAR      lpwVal)

{
    int  i;
    UINT hash;

    Assert (lpDict NE NULL);
    Assert (lpwKey NE NULL);

    // Hash the key
    hash = dictionary_hash (lpwKey);

    /* Find if value is already in dictionary */
    if (lpDict->n > 0)
    {
        for (i = 0; i < lpDict->size; i++)
        if (lpDict->key[i] NE NULL)
        {
            if (hash EQ lpDict->hash[i])
            { /* Same hash value */
                if (!lstrcmpW (lpwKey, lpDict->key[i]))
                {   /* Same key */
                    /* Found a value: modify and return */
                    if (lpwVal EQ NULL
                     || (lpDict->inifile <= lpwVal
                      &&                    lpwVal < (lpDict->inifile + lpDict->size)))
                        lpDict->val[i] = lpwVal;
                    else
                    {
                        if (lpDict->val[i] NE NULL)
                        {
                            free (lpDict->val[i]); lpDict->val[i] = NULL;
                        } // End IF

                        if (lpwVal NE NULL)
                        {
                            lpwVal = strdupW (lpwVal);
                            if (lpwVal EQ NULL)
                                return -1;
                        } // End IF

                        lpDict->val[i] = lpwVal;
                    } // End IF/ELSE

                    /* Value has been modified: return */
                    return 0;
                } // End IF
            } // End IF
        } // End FOR/IF
    } // End IF

    /* Add a new value */
    /* See if dictionary needs to grow */
    if (lpDict->n EQ lpDict->size)
    {
        LPWCHAR *val;               // Ptr to list of string values
        LPWCHAR *key;               // Ptr to list of string keys
        LPUINT   hash;              // Ptr to list of hash values for keys

        /* Reached maximum size: reallocate dictionary */
        val  = (LPWCHAR *) mem_double (lpDict->val,  lpDict->size * sizeof (*lpDict->val));
        key  = (LPWCHAR *) mem_double (lpDict->key,  lpDict->size * sizeof (*lpDict->key));
        hash = (LPUINT)    mem_double (lpDict->hash, lpDict->size * sizeof (*lpDict->hash));
        if ((val EQ NULL) || (key EQ NULL) || (hash EQ NULL))
        {
            // Replace the values that succeeded in doubling
            if (val  NE NULL) lpDict->val  = val;
            if (key  NE NULL) lpDict->key  = key;
            if (hash NE NULL) lpDict->hash = hash;

            /* Cannot grow dictionary */
            return -1;
        } // End IF

        lpDict->val  = val;
        lpDict->key  = key;
        lpDict->hash = hash;

        /* Double size */
        lpDict->size *= 2;
    } // End IF

    /* Insert key in the first empty slot */
    for (i = 0; i < lpDict->size; i++)
    if (lpDict->key[i] EQ NULL)
        /* Add key here */
        break;

    // If the key is NULL or within the file contents, it can be
    //   stored directly; otherwise it must be duplicated.
    if (lpwKey EQ NULL
     || (lpDict->inifile <= lpwKey
      &&                    lpwKey < (lpDict->inifile + lpDict->size)))
        lpDict->key[i] = lpwKey;
    else
    {
        lpDict->key[i] = strdupW (lpwKey);
        if (lpDict->key[i] EQ NULL)
            return -1;
    } // End IF/ELSE

    // If the value is NULL or within the file contents, it can be
    //   stored directly; otherwise it must be duplicated.
    if (lpwVal EQ NULL
     || (lpDict->inifile <= lpwVal
      &&                    lpwVal < (lpDict->inifile + lpDict->size)))
        lpDict->val[i] = lpwVal;
    else
    {
        if (lpDict->val[i] NE NULL)
        {
            free (lpDict->val[i]); lpDict->val[i] = NULL;
        } // End IF

        lpDict->val[i] = strdupW (lpwVal);
        if (lpDict->val[i] EQ NULL)
            return -1;
    } // End IF/ELSE

    lpDict->hash[i] = hash;
    lpDict->n++;

    return 0;
} // End dictionary_set


/*-------------------------------------------------------------------------*/
/**
  @brief    Delete a key in a dictionary
  @param    d       dictionary object to modify.
  @param    key     Key to remove.
  @return   void

  This function deletes a key in a dictionary. Nothing is done if the
  key cannot be found.
 */
/*--------------------------------------------------------------------------*/
void dictionary_unset
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     LPWCHAR      lpwKey)

{
    UINT hash;
    int  i;

    Assert (lpDict NE NULL);
    Assert (lpwKey NE NULL);

    // Hash the key
    hash = dictionary_hash (lpwKey);

    for (i = 0; i < lpDict->size; i++)
    if (lpDict->key[i] NE NULL)
    {
        /* Compare hash */
        if (hash EQ lpDict->hash[i])
        {
            /* Compare string, to avoid hash collisions */
            if (!lstrcmpW (lpwKey, lpDict->key[i]))
                /* Found key */
                break;
        } // End IF
    } // End FOR/IF

    if (i >= lpDict->size)
        /* Key not found */
        return;

    // Free the key
    free (lpDict->key[i]); lpDict->key[i] = NULL;

    // Free values if they were allocated
    if (lpDict->val[i] NE NULL)
    {
        free (lpDict->val[i]);
        lpDict->val[i] = NULL;
    } // End IF

    lpDict->hash[i] = 0;
    lpDict->n--;
} // End dictionary_unset


/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump
  @param    f   Opened file pointer.
  @return   void

  Dumps a dictionary onto an opened file pointer. Key pairs are printed out
  as @c [Key]=[Value], one per line. It is Ok to provide stdout or stderr as
  output file pointers.
 */
/*--------------------------------------------------------------------------*/
#ifdef DUMPDIC
void dictionary_dump
    (LPDICTIONARY lpDict,       // Ptr to workspace dictionary
     FILE         *out)

{
    int i;

    Assert (lpDict NE NULL);
    Assert (lpwKey NE NULL)

    if (lpDict->n < 1)
    {
        fprintf (out, "empty dictionary\n");
        return;
    } // End IF

    for (i = 0; i < lpDict->size; i++)
    {
        if (lpDict->key[i])
        {
            fprintf (out, "%20s\t[%s]\n",
                     lpDict->key[i],
                     lpDict->val[i] ? lpDict->val[i] : "UNDEF");
        } // End IF
    } // End FOR
} // End dictionary_dump
#endif


/* Test code */
#ifdef TESTDIC

/** Invalid key token */
#define DICT_INVALID_KEY    ((LPWCHAR)-1)

#define NVALS 20000
int main
    (int  argc,
     char *argv[])

{
    LPDICTIONARY lpDict;        // Ptr to workspace dictionary
    LPWCHAR      lpwVal;
    int          i;
    WCHAR        cval[90];

    /* Allocate dictionary */
    printf ("allocating...\n");
    lpDict = dictionary_new (0);

    /* Set values in dictionary */
    printf ("setting %d values...\n", NVALS);
    for (i = 0; i < NVALS; i++)
    {
        wsprintfW (cval, L"%04d", i);
        dictionary_set (lpDict, cval, L"salut");
    } // End FOR

    printf ("getting %d values...\n", NVALS);
    for (i = 0; i < NVALS; i++)
    {
        wsprintfW (cval, L"%04d", i);
        lpwVal = dictionary_get (lpDict, cval, DICT_INVALID_KEY, NULL);
        if (lpwVal EQ DICT_INVALID_KEY)
            printf ("cannot get value for key [%s]\n", cval);
    } // End FOR

    printf ("unsetting %d values...\n", NVALS);
    for (i = 0; i < NVALS; i++)
    {
        wsprintfW (cval, L"%04d", i);
        dictionary_unset (lpDict, cval);
    } // End FOR

    if (lpDict->n NE 0)
        printf ("error deleting values\n");

    printf ("deallocating...\n");
    dictionary_del (lpDict); lpDict = NULL;

    return 0;
} // End main
#endif
/* vim: set ts=4 et sw=4 tw=75 */
