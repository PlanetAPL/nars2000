//***************************************************************************
//  NARS2000 -- Hungarian Algorithm For Solving The Assignment Problem
//***************************************************************************

// -*- mode: c, coding: utf-8 -*-

/**
 * O(n³) implementation of the Hungarian algorithm
 *
 * Copyright (C) 2011  Mattias Andrée
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include <windows.h>
#include "headers.h"


//new rowcol[X]
#define new_rowcol(X)       MyGlobalAlloc (GPTR, (X) * sizeof (ROWCOL))
#define free_rowcol(X)      MyGlobalFree  (X)

//new boolean[X]
#define new_booleans(X)     new_bytes ((X) * sizeof (UBOOL))
#define free_booleans(X)    free_bytes (X)

//new byte[X]
#define new_bytes(X)        MyGlobalAlloc (GPTR, X)
#define free_bytes(X)       MyGlobalFree  (X)

//new llong[X]
#define new_llongs(X)       MyGlobalAlloc (GPTR, (X) * sizeof (llong))
#define free_llongs(X)      MyGlobalFree  (X)

//new long[X]
#define new_longs(X)        MyGlobalAlloc (GPTR, (X) * sizeof (long))
#define free_longs(X)       MyGlobalFree  (X)

//new double[X]
#define new_doubles(X)      MyGlobalAlloc (GPTR, (X) * sizeof (double))
#define free_doubles(X)     MyGlobalFree  (X)

//new ?[][X] -- arrays of ptrs
#define new_arrays(X)       new_bytes ((X) * sizeof (LPVOID))
#define free_arrays(X)      free_bytes (X)


typedef enum tagCELLMARK    // Cell markings
{
    UNMARKED = 0,           // 00:  None
    MARKED      ,           // 01:  Marked
    PRIME       ,           // 02:  Prime
} CELLMARK, *LPCELLMARK;


// Prototypes
void   SUFF (kuhn_reduceRows)     (cell *t, long n, long m);
BYTE **SUFF (kuhn_mark)           (cell *t, long n, long m);
long  *SUFF (kuhn_findPrime)      (cell *t, byte **marks, UBOOL *rowCovered, UBOOL *colCovered, long n, long m);
void   SUFF (kuhn_addAndSubtract) (cell *t, UBOOL *rowCovered, UBOOL *colCovered, long n, long m);

UBOOL    kuhn_isDone   (byte **marks, UBOOL *colCovered, long n, long m);
void     kuhn_altMarks (byte **marks, long *altRow, long *altCol, long *colMarks, long *rowPrimes, long *prime, long n, long m);
LPROWCOL kuhn_assign   (byte **marks, long n, long m);
BITSET   new_BitSet    (long size);
void     BitSet_set    (BITSET this, long i);
void     BitSet_unset  (BITSET this, long i);
long     BitSet_any    (BITSET this);
long     lb            (llong value);


/**
 * Calculates an optimal bipartite minimum weight matching using an
 * O(n³)-time implementation of The Hungarian Algorithm, also known
 * as Kuhn's Algorithm.
 *
 * @param   table  The table in which to perform the matching
 * @param   n      The height of the table
 * @param   m      The width of the table
 * @return         The optimal assignment, an array of row–column pairs
 */
LPROWCOL SUFF (kuhn_match) (cell *table, long n, long m)
{
    long     i, j, len;
    byte   **marks;
    LPUBOOL  rowCovered,
             colCovered;
    long    *altRow,
            *altCol,
            *rowPrimes,
            *colMarks,
            *prime;
    LPROWCOL rc;

    // Calculate the NELM
    len = n * m;

#if 0
    {
        WCHAR wszTemp[128];

        Assert (m EQ 5);

        for (i = 0; i < n; i++)
        {
            swprintf (wszTemp,
                      countof (wszTemp),
                      L"%f %f %f %f %f",
                      table[i * m + 0],
                      table[i * m + 1],
                      table[i * m + 2],
                      table[i * m + 3],
                      table[i * m + 4]);
            DbgMsgW (wszTemp);
        } // End FOR
    }
#endif

    SUFF (kuhn_reduceRows) (table, n, m);

#if 0
    {
        WCHAR wszTemp[128];

        Assert (m EQ 5);

        for (i = 0; i < n; i++)
        {
            swprintf (wszTemp,
                      countof (wszTemp),
                      L"%f %f %f %f %f",
                      table[i * m + 0],
                      table[i * m + 1],
                      table[i * m + 2],
                      table[i * m + 3],
                      table[i * m + 4]);
            DbgMsgW (wszTemp);
        } // End FOR
    }
#endif

    marks = SUFF (kuhn_mark) (table, n, m);

#if 0
    {
        WCHAR wszTemp[128];

        Assert (m EQ 5);

        for (i = 0; i < n; i++)
        {
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"%d %d %d %d %d",
                        marks[i][0],
                        marks[i][1],
                        marks[i][2],
                        marks[i][3],
                        marks[i][4]);
            DbgMsgW (wszTemp);
        } // End FOR
    }
#endif

    rowCovered = new_booleans (n);
    colCovered = new_booleans (m);

    for (i = 0; i < n; i++)
        rowCovered[i] = FALSE;

    for (j = 0; j < m; j++)
        colCovered[j] = FALSE;

    altRow = new_longs (len);
    altCol = new_longs (len);

    rowPrimes = new_longs (n);
    colMarks  = new_longs (m);

    for (;;)
    {
        if (kuhn_isDone (marks, colCovered, n, m))
            break;

        for (;;)
        {
            prime = SUFF (kuhn_findPrime) (table, marks, rowCovered, colCovered, n, m);
            if (prime NE NULL)
            {
                kuhn_altMarks (marks, altRow, altCol, colMarks, rowPrimes, prime, n, m);
                for (i = 0; i < n; i++)
                    rowCovered[i] = FALSE;

                for (j = 0; j < m; j++)
                    colCovered[j] = FALSE;

                free_longs (prime);

                break;
            } // End IF

            SUFF (kuhn_addAndSubtract) (table, rowCovered, colCovered, n, m);
        } // End FOR
    } // End FOR

    free_booleans (rowCovered);
    free_booleans (colCovered);
    free_longs    (altRow);
    free_longs    (altCol);
    free_longs    (rowPrimes);
    free_longs    (colMarks);

    rc = kuhn_assign (marks, n, m);

    for (i = 0; i < n; i++)
        free_bytes (marks[i]);
    free_arrays (marks);

    return rc;
} // End SUFF (kuhn_match)


/**
 * Reduces the values on each rows so that, for each row, the
 * lowest cells value is zero, and all cells' values is decrease
 * with the same value [the minium value in the row].
 *
 * @param  t  The table in which to perform the reduction
 * @param  n  The table's height
 * @param  m  The table's width
 */
void SUFF (kuhn_reduceRows) (cell* t, long n, long m)
{
    long i,
         j;
    cell min,
        *ti;

    CELL_INIT (min);

    for (i = 0; i < n; i++)
    {
        ti = &t[i * m];
////////min = *ti;
        CELL_IS (min, ti[0]);
        for (j = 1; j < m; j++)
////////if (min > ti[j])
////////    min = ti[j];
        if (CELL_GT (min, ti[j]))
            CELL_IS (min, ti[j]);

        for (j = 0; j < m; j++)
////////////ti[j] -= min;
            CELL_SUBEQ (ti[j], min);
    } // End FOR

    CELL_CLEAR (min);
} // End SUFF (kuhn_reduceRows)


/**
 * Create a matrix with marking of cells in the table whose
 * value is zero [minimal for the row]. Each marking will
 * be on an unique row and an unique column.
 *
 * @param   t  The table in which to perform the reduction
 * @param   n  The table's height
 * @param   m  The table's width
 * @return     A matrix of markings as described in the summary
 */
BYTE **SUFF (kuhn_mark) (cell *t, long n, long m)
{
    long    i, j;
    byte  **marks = new_arrays(n);
    byte   *marksi;
    LPUBOOL rowCovered,
            colCovered;

    for (i = 0; i < n; i++)
    {
        marksi = marks[i] = new_bytes (m);
        for (j = 0; j < m; j++)
            marksi[j] = UNMARKED;
    } // End FOR

    rowCovered = new_booleans (n);
    colCovered = new_booleans (m);

    for (i = 0; i < n; i++)
        rowCovered[i] = FALSE;

    for (j = 0; j < m; j++)
        colCovered[j] = FALSE;

    for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
    if (!rowCovered[i]
     && !colCovered[j]
//// && t[i * m + j] EQ 0)
     && CELL_IZIT0 (t[i * m + j]))
    {
        marks[i][j]   = MARKED;
        rowCovered[i] =
        colCovered[j] = TRUE;
    } // End FOR/FOR/IF

    free_booleans (rowCovered);
    free_booleans (colCovered);

    return marks;
} // End SUFF (kuhn_mark)


/**
 * Finds a prime
 *
 * @param   t           The table
 * @param   marks       The marking matrix
 * @param   rowCovered  Row cover array
 * @param   colCovered  Column cover array
 * @param   n           The table's height
 * @param   m           The table's width
 * @return              The row and column of the found print, <code>null</code> will be returned if none can be found
 */
long *SUFF (kuhn_findPrime) (cell *t, byte **marks, UBOOL *rowCovered, UBOOL *colCovered, long n, long m)
{
    long   i, j;
    long   p, row, col;
    UBOOL  markInRow;
    BITSET zeroes = new_BitSet (n * m);

    for (i = 0; i < n; i++)
    if (!rowCovered[i])
    for (j = 0; j < m; j++)
    if (!colCovered[j]
//// && t[i * m + j] EQ 0)
     && CELL_IZIT0 (t[i * m + j]))
        BitSet_set (zeroes, i * m + j);

    for (;;)
    {
        p = BitSet_any (zeroes);
        if (p < 0)
        {
            free_llongs  (zeroes.limbs);
            free_longs   (zeroes.first);
            free_longs   (zeroes.next);
            free_longs   (zeroes.prev);

            return NULL;
        } // End IF

        row = p / m;
        col = p % m;

        marks[row][col] = PRIME;

        markInRow = FALSE;
        for (j = 0; j < m; j++)
        if (marks[row][j] EQ MARKED)
        {
            markInRow = TRUE;
            col = j;
        } // End FOR/IF

        if (markInRow)
        {
            rowCovered[row] = TRUE;
            colCovered[col] = FALSE;

            for (i = 0; i < n; i++)
////////////if (t[i * m + col] EQ 0
            if (CELL_IZIT0 (t[i * m + col])
             && row NE i)
            {
                if (!rowCovered[i]
                 && !colCovered[col])
                    BitSet_set   (zeroes, i * m + col);
                else
                    BitSet_unset (zeroes, i * m + col);
            } // End FOR/IF

            for (j = 0; j < m; j++)
////////////if (t[row * m + j] EQ 0
            if (CELL_IZIT0 (t[row * m + j])
             && col NE j)
            {
                if (!rowCovered[row]
                 && !colCovered[j])
                    BitSet_set   (zeroes, row * m + j);
                else
                    BitSet_unset (zeroes, row * m + j);
            } // End FOR/IF

            if (!rowCovered[row]
             && !colCovered[col])
                BitSet_set   (zeroes, row * m + col);
            else
                BitSet_unset (zeroes, row * m + col);
        } else
        {
            long* rc = new_longs (2);
            rc[0] = row;
            rc[1] = col;

            free_llongs  (zeroes.limbs);
            free_longs   (zeroes.first);
            free_longs   (zeroes.next);
            free_longs   (zeroes.prev);

            return rc;
        } // End IF/ELSE
    } // End FOR
} // End SUFF (kuhn_findPrime)


/**
 * Depending on whether the cells' rows and columns are covered,
 * the the minimum value in the table is added, subtracted or
 * neither from the cells.
 *
 * @param  t           The table to manipulate
 * @param  rowCovered  Array that tell whether the rows are covered
 * @param  colCovered  Array that tell whether the columns are covered
 * @param  n           The table's height
 * @param  m           The table's width
 */
void SUFF (kuhn_addAndSubtract) (cell *t, UBOOL *rowCovered, UBOOL *colCovered, long n, long m)
{
    long i, j;
    cell min;

    CELL_INIT (min);

    // Initialize with the identity element for the cell minimum
    CELL_IS_INF (min);

    for (i = 0; i < n; i++)
    if (!rowCovered[i])
    for (j = 0; j < m; j++)
    if (!colCovered[j]
//// && min > t[i * m + j])
////    min = t[i * m + j];
     && CELL_GT (min, t[i * m + j]))
        CELL_IS (min, t[i * m + j]);

    for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
    {
        if (rowCovered[i])
////////////t[i * m + j] += min;
            CELL_ADDEQ (t[i * m + j], min);
        if (!colCovered[j])
////////////t[i * m + j] -= min;
            CELL_SUBEQ (t[i * m + j], min);
    } // End FOR/FOR

    CELL_CLEAR (min);
} // End SUFF (kuhn_addAndSubtract)

#ifdef HUNGARIAN_SUBS
/**
 * Determines whether the marking is complete, that is
 * if each row has a marking which is on a unique column.
 *
 * @param   marks       The marking matrix
 * @param   colCovered  An array which tells whether a column is covered
 * @param   n           The table's height
 * @param   m           The table's width
 * @return              Whether the marking is complete
 */
UBOOL kuhn_isDone (byte **marks, UBOOL *colCovered, long n, long m)
{
    long i,
         j,
         count;

    for (j = 0; j < m; j++)
    for (i = 0; i < n; i++)
    if (marks[i][j] EQ MARKED)
    {
        colCovered[j] = TRUE;
        break;
    } // End FOR/FOR/IF

    count = 0;
    for (j = 0; j < m; j++)
    if (colCovered[j])
        count++;

    return count EQ n;
} // End kuhn_isDone


/**
 * Removes all prime marks and modifies the marking
 *
 * @param  marks      The marking matrix
 * @param  altRow     Marking modification path rows
 * @param  altCol     Marking modification path columns
 * @param  colMarks   Markings in the columns
 * @param  rowPrimes  Primes in the rows
 * @param  prime      The last found prime
 * @param  n          The table's height
 * @param  m          The table's width
 */
void kuhn_altMarks (byte **marks, long *altRow, long *altCol, long *colMarks, long *rowPrimes, long *prime, long n, long m)
{
    long  index = 0, i, j;
    long  row, col;
    byte *markx,
         *marksi;

    *altRow = prime[0];
    *altCol = prime[1];

    for (i = 0; i < n; i++)
    {
        rowPrimes[i] = -1;
        colMarks [i] = -1;
    } // End FOR

    for (i = n; i < m; i++)
        colMarks[i] = -1;

    for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
    if (marks[i][j] EQ MARKED)
        colMarks[j] = i;
    else
    if (marks[i][j] EQ PRIME)
        rowPrimes[i] = j;

    for (;;)
    {
        row = colMarks[altCol[index]];
        if (row < 0)
            break;

        index++;
        altRow[index] = row;
        altCol[index] = altCol[index - 1];

        col = rowPrimes[altRow[index]];

        index++;
        altRow[index] = altRow[index - 1];
        altCol[index] = col;
    } // End FOR

    for (i = 0; i <= index; i++)
    {
        markx = marks[altRow[i]] + altCol[i];
        if (markx[0] EQ MARKED)
            markx[0] = UNMARKED;
        else
            markx[0] = MARKED;
    } // End FOR

    for (i = 0; i < n; i++)
    {
        marksi = marks[i];
        for (j = 0; j < m; j++)
        if (marksi[j] EQ PRIME)
            marksi[j] = UNMARKED;
    } // End FOR
} // End kuhn_altMarks


/**
 * Creates a list of the assignment cells
 *
 * @param   marks  Matrix markings
 * @param   n      The table's height
 * @param   m      The table's width
 * @return         The assignment, an array of row–column pairs
 */
LPROWCOL kuhn_assign (byte **marks, long n, long m)
{
    long     i, j;
    LPROWCOL assignment = new_rowcol (n);

    for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
    if (marks[i][j] EQ MARKED)
    {
        assignment[i].row = i;
        assignment[i].col = j;
    } // End FOR/FOR/IF

    return assignment;
} // End kuhn_assign


/**
 * Constructor for BitSet
 *
 * @param   size  The (fixed) number of bits to bit set should contain
 * @return        The a unique BitSet instance with the specified size
 */
BITSET new_BitSet (long size)
{
    BITSET this;
    long   c = size >> 6L;
    long   i;

    if (size & 63L)
        c++;

    this.limbs = new_llongs (c);
    this.prev  = new_longs  (c + 1L);
    this.next  = new_longs  (c + 1L);
    this.first = new_longs  (1);
    this.first[0] = 0;

    for (i = 0; i < c; i++)
    {
        this.limbs[i] = 0LL;
        this.prev [i] = this.next[i] = 0L;
    } // End FOR

    this.prev[c] = this.next[c] = 0L;

    return this;
} // End new_BitSet


/**
 * Turns on a bit in a bit set
 *
 * @param  this  The bit set
 * @param  i     The index of the bit to turn on
 */
void BitSet_set (BITSET this, long i)
{
    long  j = i >> 6L;
    llong old = this.limbs[j];

    this.limbs[j] |= 1LL << (llong)(i & 63L);

    if ((!this.limbs[j]) ^ (!old))
    {
        j++;
        this.prev[this.first[0]] = j;
        this.prev[j] = 0;
        this.next[j] = this.first[0];
        this.first[0] = j;
    } // End IF
} // End BitSet_set


/**
 * Turns off a bit in a bit set
 *
 * @param  this  The bit set
 * @param  i     The index of the bit to turn off
 */
void BitSet_unset (BITSET this, long i)
{
    long  j = i >> 6L;
    llong old = this.limbs[j];

    this.limbs[j] &= ~(1LL << (llong)(i & 63L));

    if ((!this.limbs[j]) ^ (!old))
    {
        long p,
             n;

        j++;
        p = this.prev[j];
        n = this.next[j];
        this.prev[n] = p;
        this.next[p] = n;

        if (this.first[0] EQ j)
            this.first[0] = n;
    } // End IF
} // End BitSet_unset


/**
 * Gets the index of any set bit in a bit set
 *
 * @param   this  The bit set
 * @return        The index of any set bit
 */
long BitSet_any (BITSET this)
{
    long i;

    if (this.first[0] EQ 0L)
        return -1;

    i = this.first[0] - 1L;
    return lb (this.limbs[i] & - this.limbs[i]) + (i << 6L);
} // End BitSet_any


/**
 * Calculates the floored binary logarithm of a positive integer
 *
 * @param   value  The integer whose logarithm to calculate
 * @return         The floored binary logarithm of the integer
 */
long lb (llong value)
{
    long  rc = 0L;
    llong v = value;

    if (v & 0xFFFFFFFF00000000LL)  {  rc |= 32L;  v >>= 32LL;  }
    if (v & 0x00000000FFFF0000LL)  {  rc |= 16L;  v >>= 16LL;  }
    if (v & 0x000000000000FF00LL)  {  rc |=  8L;  v >>=  8LL;  }
    if (v & 0x00000000000000F0LL)  {  rc |=  4L;  v >>=  4LL;  }
    if (v & 0x000000000000000CLL)  {  rc |=  2L;  v >>=  2LL;  }
    if (v & 0x0000000000000002LL)     rc |=  1L;

    return rc;
} // End lb
#endif


//***************************************************************************
//  End of File: hungarian_src.h
//***************************************************************************
