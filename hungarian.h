//***************************************************************************
//  NARS2000 -- Header For Hungarian Algorithm For Solving The Assignment Problem
//***************************************************************************

#define llong   __int64

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

/**
 * Bit set, a set of fixed number of bits/booleans
 */
typedef struct tagBITSET
{
    /**
     * The set of all limbs, a limb consist of 64 bits
     */
    llong* limbs;

    /**
     * Singleton array with the index of the first non-zero limb
     */
    long* first;

    /**
     * Array of the index of the previous non-zero limb for each limb
     */
    long* prev;

    /**
     * Array of the index of the next non-zero limb for each limb
     */
    long* next;

} BITSET, *LPBITSET;


typedef struct tagROWCOL
{
    long row,               // Row index into the result
         col;               // Col ...
} ROWCOL, *LPROWCOL;


//***************************************************************************
//  End of File: hungarian.h
//***************************************************************************
