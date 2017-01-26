//***************************************************************************
//  NARS2000 -- Hungarian Double Algorithm -- See hungarian_src.h for more details
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define cell                APLFLOAT
#define SUFF(a)             a##_dbl
#define CELL_SUBEQ(a,b)     a -= b
#define CELL_ADDEQ(a,b)     a += b
#define CELL_GT(a,b)        (a) > (b)
#define CELL_IS(a,b)        (a) = (b)
#define CELL_IZIT0(a)       (a) EQ 0
#define CELL_IS_INF(a)      (a) = DBL_MAX
#define CELL_INIT(a)
#define CELL_CLEAR(a)
#define HUNGARIAN_SUBS

#include "hungarian_src.h"

/**
 * Calculates an optimal bipartite minimum weight matching using an
 * O(n³)-time implementation of The Hungarian Algorithm, also known
 * as Kuhn's Algorithm.
 *
 * @param   table  The table in which to perform the matching
 * @param   n      The height of the table
 * @param   m      The width of the table
 * @param   bMaxFcn TRUE iff the left operand is Max, FALSE if it's Min
 * @return         The optimal assignment, an array of row–column pairs
 */

LPROWCOL kuhn_start_dbl (LPAPLFLOAT oldTable, long n, long m, UBOOL bMaxFcn)
{
    LPAPLFLOAT table;
    long       i, len;
    LPROWCOL   resLong;

    // Calculate the NELM
    len = n * m;

    // Copy the table and convert to cell datatype
    table = MyGlobalAlloc (GPTR, len * sizeof (cell));

    // If this is MaxFcn, ...
    if (bMaxFcn)
        // Negate to do maxFcn
        for (i = 0; i < len; i++)
            table[i] = -oldTable[i];
    else
        // Copy the old table en masse
        CopyMemory (table, oldTable, len * sizeof (cell));

    resLong = kuhn_match_dbl (table, n, m);

    // We no longer need this resource
    MyGlobalFree (table);

    return resLong;
} // End kuhn_start_dbl


//***************************************************************************
//  End of File: hungarian_dbl.c
//***************************************************************************
