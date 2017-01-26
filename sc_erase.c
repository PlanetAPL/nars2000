//***************************************************************************
//  NARS2000 -- System Command:  )ERASE
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

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "headers.h"


//***************************************************************************
//  $CmdErase_EM
//
//  Execute the system command:  )ERASE var[ var]
//***************************************************************************

UBOOL CmdErase_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    APLUINT      uRht;              // Loop counter
////LPAPLCHAR    lpMemDataRht,      // Ptr to right arg char data
    LPAPLCHAR    lpMemDataStart;    // Ptr to start of identifier
    LPSYMENTRY   lpSymEntry;        // Ptr to STE
    STFLAGS      stFlags;           // STE flags
    UBOOL        bNotErasedHeader = FALSE; // TRUE iff the NotErased header
                                    //   has been displayed
    APLU3264     iNotErasedWidth = 0; // Width (so far) of NotErased line

    // Loop through the right arg looking for identifiers
    uRht = 0;
    while (TRUE)
    {
        // Skip over white space
        while (lpwszTail[uRht] EQ L' ')
            uRht++;
        if (lpwszTail[uRht])
        {
            // Save the starting ptr
            lpMemDataStart = &lpwszTail[uRht];

            // Skip over black space
            while (lpwszTail[uRht] && lpwszTail[uRht] NE L' ')
                uRht++;
            // Lookup the name in the symbol table
            // SymTabLookupName sets the .ObjName enum,
            //   and the .Inuse flag
            ZeroMemory (&stFlags, sizeof (stFlags));
            lpSymEntry = SymTabLookupNameLength (lpMemDataStart,
                                                (APLU3264) (&lpwszTail[uRht] - lpMemDataStart),
                                                &stFlags);
            // If found, attempt to expunge the name
            // If not found, ignore it if it's a valid name
            // If not expunged, display the name after the NotErased header
            if (lpSymEntry)
            {
                // Attempt to expunge the global value
                if (!ExpungeName (GetGlobalSTE (lpSymEntry), TRUE))
                {
                    HGLOBAL   htGlbName;            // STE name global memory handle
                    LPAPLCHAR lpwGlbName;           // Ptr to STE name

                    // Get the STE name global memory handle
                    htGlbName = lpSymEntry->stHshEntry->htGlbName;

                    // Lock the memory to get a ptr to it
                    lpwGlbName = MyGlobalLockWsz (htGlbName);

                    // Display an error message
                    ExpungeError (lpwGlbName,                       // Ptr to name in global memory
                                  lstrlenW (lpwGlbName),            // Length of the name
                                 &bNotErasedHeader,                 // TRUE iff the NotErased header has been displayed
                                 &iNotErasedWidth);                 // Current width of the NotErased line
                    // We no longer need this ptr
                    MyGlobalUnlock (htGlbName); lpwGlbName = NULL;
                } // End IF
            } else
            {
                // Display an error message
                ExpungeError (lpMemDataStart,                       // Ptr to name in global memory
                   (APLU3264) (&lpwszTail[uRht] - lpMemDataStart),  // Length of the name
                             &bNotErasedHeader,                     // TRUE iff the NotErased header has been displayed
                             &iNotErasedWidth);                     // Current width of the NotErased line
            } // End IF/ELSE
        } else
            break;
    } // End WHILE

    // If we displayed any unerased names, end the line
    if (bNotErasedHeader)
        AppendLine (L"", TRUE, TRUE);
    return TRUE;
} // End CmdErase_EM


//***************************************************************************
//  ExpungeError
//
//  Display the NotErased header followed by the STE name
//    wrapping at the page width
//***************************************************************************

void ExpungeError
    (LPAPLCHAR lpwGlbName,          // Ptr to name in global memory
     APLU3264  iLen,                // Length of the name
     LPUBOOL   lpbNotErasedHeader,  // TRUE iff the NotErased header has been displayed
     APLU3264 *lpiNotErasedWidth)   // Current width of the NotErased line

{
    APLUINT   uQuadPW;              // []PW
    APLCHAR   aplChar;              // Temporary character

    // Get the current value of []PW
    uQuadPW = GetQuadPW ();

    // If the header has not been displayed as yet, do so now
    if (!*lpbNotErasedHeader)
    {
        AppendLine (ERRMSG_NOT_ERASED, FALSE, FALSE);
        *lpbNotErasedHeader = TRUE;
        *lpiNotErasedWidth = strcountof (ERRMSG_NOT_ERASED);
    } // End IF

    if ((*lpiNotErasedWidth + 2 + iLen) > uQuadPW)
    {
        // End the line
        AppendLine (L"", TRUE, TRUE);
        *lpiNotErasedWidth = 0;
    } // End IF

    // Output leading blanks if not at line start
    if (*lpiNotErasedWidth)
    {
        AppendLine (L"  ", TRUE, FALSE);
        *lpiNotErasedWidth += 2;
    } // End IF

    // Save and zap the char after the name
    aplChar = lpwGlbName[iLen];
    lpwGlbName[iLen] = WC_EOS;

    // Output the STE name
    AppendLine (lpwGlbName, TRUE, FALSE);
    *lpiNotErasedWidth += iLen;

    // Restore the char we zapped
    lpwGlbName[iLen] = aplChar;
} // End ExpungeError


//***************************************************************************
//  GetGlobalSTE
//
//  Return a ptr to the global STE for a given STE
//***************************************************************************

LPSYMENTRY GetGlobalSTE
    (LPSYMENTRY lpSymEntry)         // Ptr to the STE

{
    // Loop through the shadow chain of previous STEs
    while (lpSymEntry->stPrvEntry)
        lpSymEntry = lpSymEntry->stPrvEntry;
    return lpSymEntry;
} // End GetGlobalSTE

//***************************************************************************
//  End of File: sc_erase.c
//***************************************************************************
