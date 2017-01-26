//***************************************************************************
//  NARS2000 -- Hungarian RAT Algorithm -- See hungarian_src.h for more details
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

#define cell                APLRAT
#define SUFF(a)             a##_rat
#define CELL_SUBEQ(a,b)     mpq_sub (&(a), &(a), &(b))
#define CELL_ADDEQ(a,b)     mpq_add (&(a), &(a), &(b))
#define CELL_GT(a,b)        mpq_cmp (&(a), &(b)) > 0
#define CELL_IS(a,b)        mpq_set (&(a), &(b))
#define CELL_IZIT0(a)       mpq_cmp_ui (&(a), 0, 1) EQ 0
#define CELL_IS_INF(a)      mpq_set_inf (&(a), 1)
#define CELL_INIT(a)        mpq_init (&(a))
#define CELL_CLEAR(a)       Myq_clear (&(a))

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

LPROWCOL kuhn_start_rat  (LPAPLRAT oldTable, long n, long m, UBOOL bMaxFcn)
{
    LPAPLRAT   table;
    long       i, len;
    LPROWCOL   resLong;

    // Calculate the NELM
    len = n * m;

    // Make a copy of the table
    table = MyGlobalAlloc (GPTR, len * sizeof (cell));

    for (i = 0; i < len; i++)
    {
        // Initialize and copy
        mpq_init_set (&table[i], &oldTable[i]);

        // If this is MaxFcn, ...
        if (bMaxFcn)
            // Negate to do MaxFcn
            mpq_neg (&table[i], &table[i]);
    } // End FOR

    resLong = kuhn_match_rat (table, n, m);

    for (i = 0; i < len; i++)
        Myq_clear (&table[i]);

    // We no longer need this resource
    MyGlobalFree (table);

    return resLong;
} // End kuhn_start_rat


//***************************************************************************
//  End of File: hungarian_rat.c
//***************************************************************************
