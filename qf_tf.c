//***************************************************************************
//  NARS2000 -- System Function -- Quad TF
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
#include "headers.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros


//***************************************************************************
//  $SysFnTF_EM_YY
//
//  System function:  []TF -- Transfer Form
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnTF_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnTF_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return SysFnMonTF_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return SysFnDydTF_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    return NULL;
} // End SysFnTF_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnMonTF_EM_YY
//
//  Monadic []TF -- error
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnMonTF_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnMonTF_EM_YY
    (LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnValenceError_EM (lptkFunc APPEND_NAME_ARG);
} // End SysFnMonTF_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydTF_EM_YY
//
//  Dyadic []TF -- Transfer Form
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydTF_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydTF_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeLft;       // Left arg storage type
    APLNELM           aplNELMLft;       // Left arg NELM
    APLRANK           aplRankLft;       // Left arg rank
    HGLOBAL           hGlbLft = NULL;   // Left arg global memory handle
    APLLONGEST        aplLongestLft;    // Left arg immediate value
    UBOOL             bRet = TRUE;      // TRUE iff result is valid
    LPPERTABDATA      lpMemPTD;         // Ptr to PerTabData global memory
    HGLOBAL           hGlbRht = NULL;   // Right arg global memory handle
    LPAPLCHAR         lpMemRht = NULL;  // Ptr to right arg global memory
    APLSTYPE          aplTypeRht;       // Right arg storage type
    APLNELM           aplNELMRht;       // Right arg NELM
    APLRANK           aplRankRht;       // Right arg rank
    APLLONGEST        aplLongestRht,    // First value from right arg
                      aplLongestTmp;    // First value temporary
    LPWCHAR           lpwszTemp;        // Ptr to temporary storage
    LPPL_YYSTYPE      lpYYRes = NULL;   // Ptr to the result
    VARS_TEMP_OPEN

    // Get the attributes (Type, NELM, and Rank)
    //   of the left arg
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);

    // Check for LEFT RANK ERROR
    if (IsMultiRank (aplRankLft))
        goto LEFT_RANK_EXIT;

    // Check for LEFT LENGTH ERROR
    if (!IsSingleton (aplNELMLft))
        goto LEFT_LENGTH_EXIT;

    // Check for LEFT DOMAIN ERROR
    if (!IsNumeric (aplTypeLft))
        goto LEFT_DOMAIN_EXIT;

    // Get left arg's global ptrs
    aplLongestLft = GetGlbPtrs (lptkLftArg, &hGlbLft);

    //Split cases based upon the left arg storage type
    switch (aplTypeLft)
    {
        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplLongestLft = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);

            // Fall through to common code

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an integer using System []CT
            aplLongestLft = GetNextRatIntGlb (hGlbLft, 0, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an integer using System []CT
            aplLongestLft = GetNextVfpIntGlb (hGlbLft, 0, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    // Calc the absolute value so we can compare against valid values
    aplLongestTmp = abs64 (aplLongestLft);

    // Check for LEFT DOMAIN ERROR
    if (!bRet
     || (aplLongestTmp NE 1
      && aplLongestTmp NE 2))
        goto LEFT_DOMAIN_EXIT;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary format save area
    lpwszTemp = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT DOMAIN ERROR
    if (!IsSimpleChar (aplTypeRht))
        goto RIGHT_DOMAIN_EXIT;

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If the right arg is a global, ...
    if (hGlbRht)
        // Skip over the header to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    else
        // Otherwise it's an immediate value
        lpMemRht = (LPAPLCHAR) &aplLongestRht;

    // Skip over initial chars in case we need to insert a type identifier
    //   or []Z{is} (three chars)
#ifdef DEBUG
    lpwszTemp[0] =
    lpwszTemp[1] =
    lpwszTemp[2] = L'\xFFFF';
#endif
    lpwszTemp += 3;

    // Copy the data to temporary storage so we can terminate it properly
    //   as well as translate it to/from APL2 charset
    CopyMemoryW (lpwszTemp, lpMemRht, (APLU3264) aplNELMRht);
    lpwszTemp[aplNELMRht] = WC_EOS;

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    // Skip over leading blanks
    while (aplNELMRht && *lpwszTemp EQ L' ')
    {
        lpwszTemp++;
        aplNELMRht--;
    } // End WHILE

    // Skip over trailing blanks
    while (aplNELMRht && lpwszTemp[aplNELMRht - 1] EQ L' ')
        aplNELMRht--;

    // Split cases based upon the left arg
    if (aplLongestTmp EQ 1)
        lpYYRes = SysFnDydTF1_EM_YY (lptkFunc, lptkRhtArg, lpwszTemp, aplNELMRht, !SIGN_APLLONGEST (aplLongestLft));
    else
        lpYYRes = SysFnDydTF2_EM_YY (lptkFunc, lptkRhtArg, lpwszTemp, aplNELMRht, !SIGN_APLLONGEST (aplLongestLft));

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    EXIT_TEMP_OPEN

    return lpYYRes;
} // End SysFnDydTF_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydTF1_EM_YY
//
//  Return the migration transfer form (Type 1) of a given argument, or
//    return the name of a successful inverse migration transfer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydTF1_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydTF1_EM_YY
    (LPTOKEN lptkFunc,                          // Ptr to function token
     LPTOKEN lptkRhtArg,                        // Ptr to right arg token
     LPWCHAR lpwszTemp,                         // Ptr to right arg memory as temporary storage
     APLNELM aplNELMRht,                        // Right arg NELM
     UBOOL   bTranslateAPL2)                    // TRUE iff input/output should be translated to/from APL2

{
    APLNELM           aplNELMRes;               // Result    ...
    LPPL_YYSTYPE      lpYYRes = NULL;           // Ptr to the result
    APLUINT           uCnt;                     // Loop counter
    APLUINT           ByteRes;                  // # bytes in the result
    HGLOBAL           hGlbRes = NULL;           // Result global memory handle
    LPVOID            lpMemRes = NULL;          // Ptr to result global memory
    HGLOBAL           hGlbItm = NULL;           // Right arg item global memory handle
    LPVOID            lpMemItm = NULL;          // Ptr to right arg item global memory
    APLSTYPE          aplTypeItm;               // Right arg item storage type
    APLNELM           aplNELMItm;               // Right arg item NELM
    APLRANK           aplRankItm;               // Right arg item rank
    APLLONGEST        aplLongestItm;            // Right arg item immediate value
    HGLOBAL           hGlbDfnHdr = NULL;        // User-defined function operator global memory handle
    LPDFN_HEADER      lpMemDfnHdr = NULL;       // Ptr to user-defined function/operator global memory

    // The two operations are distinguished by the presence or absence
    //   of a blank in the right arg after the name
    for (uCnt = 0; uCnt < aplNELMRht; uCnt++)
    if (lpwszTemp[uCnt] EQ L' ')
        break;

    // If there's a blank, ...
    if (uCnt < aplNELMRht)
    {
        // Inverse migration transfer form

        // Translate the data from APL2 charset to NARS
        if (bTranslateAPL2)
            TranslateAPL2ToNARS (lpwszTemp, aplNELMRht, TRUE);

        // Split cases based upon the first char
        switch (lpwszTemp[0])
        {
            case L'F':
                // The migration transfer form of a function is as follows:
                //     Ffcnname rank shape values
                //   where 'rank' is a number, 'shape' is 'rank' numbers and 'values' are x/shape characters
                if (!TransferInverseFcn1_EM (lpwszTemp, 0, 0, lptkFunc, FALSE))
                    goto EMPTY_EXIT;
                break;

            case L'C':
                // The migration transfer form of a character array is as follows:
                //     Cvarname rank shape values
                //   where 'rank' is a number, 'shape' is 'rank' numbers and 'values' are x/shape characters
                if (!TransferInverseChr1_EM (lpwszTemp, 0, 0, lptkFunc, FALSE))
                    goto EMPTY_EXIT;
                break;

            case L'N':
                // The migration transfer form of a numeric array is as follows:
                //     Nvarname rank shape values
                //   where 'rank' is a number, 'shape' is 'rank' numbers and 'values' are x/shape numbers
                if (!TransferInverseNum1_EM (lpwszTemp, 0, 0, lptkFunc, FALSE))
                    goto EMPTY_EXIT;
                break;

            default:
                goto EMPTY_EXIT;
        } // End SWITCH

        // Return the name of the var we just created
        *(strchrW (&lpwszTemp[1], L' ')) = WC_EOS;

        // Get the name length
        aplNELMRes = lstrlenW (&lpwszTemp[1]);

        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Now we can allocate the storage for the result
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbRes)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = 1;
#undef  lpHeader
        // Skip over the header to the dimensions
        lpMemRes = VarArrayBaseToDim (lpMemRes);

        // Save the dimension and skip over it
        *((LPAPLDIM) lpMemRes)++ = aplNELMRes;

        // Copy the name to the result
        CopyMemoryW (lpMemRes, &lpwszTemp[1], (APLU3264) aplNELMRes);

        // Translate the name from NARS to APL2 charset
        if (bTranslateAPL2)
            TranslateNARSToAPL2 (lpMemRes, aplNELMRes, TRUE);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } else
    {
        STFLAGS        stFlags;             // ST flags for name lookup
        LPSYMENTRY     lpSymEntry;          // Ptr to SYMENTRY for name lookup
        LPWCHAR        lpwName;             // Ptr to name portion of array
        HGLOBAL        hGlbTxtLine;         // Line text ...
        LPFCNLINE      lpFcnLines;          // Ptr to array of function line structs (FCNLINE[numFcnLines])
        UINT           uNumLines,           // # function lines
                       uLine,               // Loop counter
                       uMaxLineLen;         // Length of the longest line
        LPMEMTXT_UNION lpMemTxtLine;        // Ptr to header/line text global memory

        // Generate migration transfer form for a given named object

        // Translate the name from APL2 charset to NARS
        if (bTranslateAPL2)
            TranslateAPL2ToNARS (lpwszTemp, aplNELMRht, TRUE);

        // Get ptr to the name
        //   and skip over it
        lpwName = lpwszTemp;
        lpwszTemp = &lpwName[lstrlenW (lpwName)];

        // Set the flags for what we're looking up/appending
        ZeroMemory (&stFlags, sizeof (stFlags));
        stFlags.Inuse   = TRUE;

        // If it's a system name, ...
        if (IsSysName (lpwName))
        {
            // Convert the name to lowercase
            CharLowerBuffW (lpwName, lstrlenW (lpwName));

            // Tell 'em we're looking for system names
            stFlags.ObjName = OBJNAME_SYS;
        } else
            // Tell 'em we're looking for user names
            stFlags.ObjName = OBJNAME_USR;

        // Look up the name
        lpSymEntry = SymTabLookupName (lpwName, &stFlags);
        if (!lpSymEntry)
            goto EMPTY_EXIT;

        // Search for the global entry
        while (lpSymEntry->stPrvEntry)
            lpSymEntry = lpSymEntry->stPrvEntry;

        // Back off one position so we may insert a type identifier
        lpwszTemp--;

        // Split cases based upon the name type
        switch (lpSymEntry->stFlags.stNameType)
        {
            case NAMETYPE_UNK:
                goto EMPTY_EXIT;

            case NAMETYPE_VAR:
                // If the item is a global, ...
                if (!lpSymEntry->stFlags.Imm)
                {
                    // Get the global memory handle
                    hGlbItm = lpSymEntry->stData.stGlbData;

                    // Lock the memory to get a ptr to it
                    lpMemItm = MyGlobalLock (hGlbItm);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemItm)
                    // Get the type, NELM, and Rank
                    aplTypeItm = lpHeader->ArrType;
                    aplNELMItm = lpHeader->NELM;
                    aplRankItm = lpHeader->Rank;
#undef  lpHeader
                    // Skip over the header to the dimensions
                    lpMemItm = VarArrayBaseToDim (lpMemItm);
                } else
                    aplTypeItm = TranslateImmTypeToArrayType (lpSymEntry->stFlags.ImmType);

                // Split cases based upon the right arg item storage type
                switch (aplTypeItm)
                {
                    case ARRAY_BOOL:
                    case ARRAY_INT:
                    case ARRAY_FLOAT:
                    case ARRAY_APA:
                        // Prepend the type
                        lpwName[0] = L'N';

                        // Skip over the name
                        lpwszTemp = &lpwName[lstrlenW (lpwName)];

                        // Delete trailing blanks
                        while (lpwszTemp[-1] EQ L' ')
                            lpwszTemp--;

                        // Catenate a blank separator
                        *lpwszTemp++ = L' ';

                        // Split cases based upon whether or not the item is immediate
                        if (lpSymEntry->stFlags.Imm)
                        {
                            // Save the rank
                            *lpwszTemp++ = L'0';
                            *lpwszTemp++ = L' ';

                            // Format and save the value
                            if (IsImmInt (lpSymEntry->stFlags.ImmType))
                                lpwszTemp =
                                  FormatAplintFC (lpwszTemp,                    // Ptr to output save area
                                                  lpSymEntry->stData.stLongest, // The value to format
                                                  UTF16_OVERBAR);               // Char to use as overbar
                            else
                                lpwszTemp =
                                  FormatFloatFC (lpwszTemp,                     // Ptr to output save area
                                                 lpSymEntry->stData.stFloat,    // The value to format
                                                 DEF_MAX_QUADPP64,              // Precision to use
                                                 L'.',                          // Char to use as decimal separator
                                                 UTF16_OVERBAR,                 // Char to use as overbar
                                                 FLTDISPFMT_RAWFLT,             // Float display format
                                                 FALSE);                        // TRUE iff we're to substitute text for infinity
                        } else
                        {
                            // Format & save the rank
                            lpwszTemp =
                              FormatAplintFC (lpwszTemp,                        // Ptr to output save area
                                              aplRankItm,                       // The value to format
                                              UTF16_OVERBAR);                   // Char to use as overbar
                            // Format & save the shape
                            for (uCnt = 0; uCnt < aplRankItm; uCnt++)
                                lpwszTemp =
                                  FormatAplintFC (lpwszTemp,                    // Ptr to output save area
                                                  *((LPAPLDIM) lpMemItm)++,     // The value to format
                                                  UTF16_OVERBAR);               // Char to use as overbar
                            // Loop through the elements formatting and saving them
                            for (uCnt = 0; uCnt < aplNELMItm; uCnt++)
                            {
                                GetNextValueMem (lpMemItm,          // Ptr to item global memory data
                                                 aplTypeItm,        // Item storage type
                                                 uCnt,              // Index into item
                                                 NULL,              // Ptr to result LPSYMENTRY or HGLOBAL (may be NULL)
                                                &aplLongestItm,     // Ptr to result immediate value (may be NULL)
                                                 NULL);             // Ptr to result immediate type (see IMM_TYPES) (may be NULL)
                                if (IsSimpleInt (aplTypeItm))
                                    lpwszTemp =
                                      FormatAplintFC (lpwszTemp,                // Ptr to output save area
                                                      aplLongestItm,            // The value to format
                                                      UTF16_OVERBAR);           // Char to use as overbar
                                else
                                    lpwszTemp =
                                      FormatFloatFC (lpwszTemp,                     // Ptr to output save area
                                                     *(LPAPLFLOAT) &aplLongestItm,  // The value to format
                                                     DEF_MAX_QUADPP64,              // Precision to use
                                                     L'.',                          // Char to use as decimal separator
                                                     UTF16_OVERBAR,                 // Char to use as overbar
                                                     FLTDISPFMT_RAWFLT,             // Float display format
                                                     FALSE);                        // TRUE iff we're to substitute text for infinity
                            } // End FOR
                        } // End IF/ELSE

                        break;

                    case ARRAY_CHAR:
                        // Prepend the type
                        lpwName[0] = L'C';

                        // Skip over the name
                        lpwszTemp = &lpwName[lstrlenW (lpwName)];

                        // Delete trailing blanks
                        while (lpwszTemp[-1] EQ L' ')
                            lpwszTemp--;

                        // Catenate a blank separator
                        *lpwszTemp++ = L' ';

                        // Split cases based upon whether or not the item is immediate
                        if (lpSymEntry->stFlags.Imm)
                        {
                            // Save the rank
                            *lpwszTemp++ = L'0';
                            *lpwszTemp++ = L' ';

                            // Save the value
                            *lpwszTemp++ = lpSymEntry->stData.stChar;
                        } else
                        {
                            // Format & save the rank
                            lpwszTemp =
                              FormatAplintFC (lpwszTemp,                        // Ptr to output save area
                                              aplRankItm,                       // The value to format
                                              UTF16_OVERBAR);                   // Char to use as overbar
                            // Format & save the shape
                            for (uCnt = 0; uCnt < aplRankItm; uCnt++)
                                lpwszTemp =
                                  FormatAplintFC (lpwszTemp,                    // Ptr to output save area
                                                  *((LPAPLDIM) lpMemItm)++,     // The value to format
                                                  UTF16_OVERBAR);               // Char to use as overbar
                            // Copy the values to temp storage
                            CopyMemoryW (lpwszTemp, lpMemItm, (APLU3264) aplNELMItm);

                            // Skip over the copied values
                            lpwszTemp += aplNELMItm;
                        } // End IF/ELSE

                        // Append a dummy blank
                        *lpwszTemp++ = L' ';

                        break;

                    default:
                        goto EMPTY_EXIT;
                } // End SWITCH

                // If the item was locked, ...
                if (hGlbItm && lpMemItm)
                {
                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbItm); lpMemItm = NULL;
                } // End IF

                // Back up to the last blank
                if (lpwszTemp[-1] EQ L' ')
                    lpwszTemp--;
                break;

            case NAMETYPE_FN0:
            case NAMETYPE_FN12:
            case NAMETYPE_OP1:
            case NAMETYPE_OP2:
            case NAMETYPE_OP3:
            case NAMETYPE_TRN:
                // Split off names that are not user-defined functions/operators
                if (!lpSymEntry->stFlags.UsrDfn)
                    goto EMPTY_EXIT;

                // Prepend the type
                lpwName[0] = L'F';

                // Skip over the name
                lpwszTemp = &lpwName[lstrlenW (lpwName)];

                // Delete trailing blanks
                while (lpwszTemp[-1] EQ L' ')
                    lpwszTemp--;

                // Catenate a blank separator
                *lpwszTemp++ = L' ';

                // Get the user-defined function/operator global memory handle
                hGlbDfnHdr = lpSymEntry->stData.stGlbData;

                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLock (hGlbDfnHdr);

                // Lock the memory to get a ptr to it
                lpMemTxtLine = MyGlobalLock (lpMemDfnHdr->hGlbTxtHdr);

                // Get the length of the function header text
                uMaxLineLen = lpMemTxtLine->U;

                // We no longer need this ptr
                MyGlobalUnlock (lpMemDfnHdr->hGlbTxtHdr); lpMemTxtLine = NULL;

                // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                // Get # function lines
                uNumLines = lpMemDfnHdr->numFcnLines;

                // Run through the function lines looking for the longest
                for (uLine = 0; uLine < uNumLines; uLine++)
                {
                    // Get the line text global memory handle
                    hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                    if (hGlbTxtLine)
                    {
                        // Lock the memory to get a ptr to it
                        lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

                        // Find the length of the longest line
                        uMaxLineLen = max (uMaxLineLen, lpMemTxtLine->U);

                        // We no longer need this ptr
                        MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
                    } // End IF

                    // Skip to the next struct
                    lpFcnLines++;
                } // End FOR

                // Save the rank
                *lpwszTemp++ = L'2';
                *lpwszTemp++ = L' ';

                // Format & save the # rows
                lpwszTemp =
                  FormatAplintFC (lpwszTemp,                                    // Ptr to output save area
                                  uNumLines + 1,                                // The value to format
                                  UTF16_OVERBAR);                               // Char to use as overbar
                // Format & save the # cols
                lpwszTemp =
                  FormatAplintFC (lpwszTemp,                                    // Ptr to output save area
                                  uMaxLineLen,                                  // The value to format
                                  UTF16_OVERBAR);                               // Char to use as overbar
                // Copy the header to the result as either a row or as an allocated HGLOBAL
                lpwszTemp = SysFnCR_Copy_EM (2, lpwszTemp, lpMemDfnHdr->hGlbTxtHdr, uMaxLineLen, lptkFunc);
                if (lpwszTemp EQ NULL)
                    goto ERROR_EXIT;

                // Get ptr to array of function line structs (FCNLINE[numFcnLines])
                lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

                // Run through the function lines copying each line text to the result
                for (uLine = 0; uLine < uNumLines; uLine++)
                {
                    // Get the line text global memory handle
                    hGlbTxtLine = lpFcnLines->hGlbTxtLine;

                    if (hGlbTxtLine)
                    {
                        // Copy the header to the result as either a row or as an allocated HGLOBAL
                        lpwszTemp = SysFnCR_Copy_EM (2, lpwszTemp, hGlbTxtLine, uMaxLineLen, lptkFunc);
                        if (lpwszTemp EQ NULL)
                            goto ERROR_EXIT;
                    } // End IF

                    // Skip to the next struct
                    lpFcnLines++;
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;

                break;

            defstop
                break;
        } // End SWITCH

        // Get the string length
        aplNELMRes = lpwszTemp - lpwName;

        // Calculate space needed for the result
        ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

        // Check for overflow
        if (ByteRes NE (APLU3264) ByteRes)
            goto WSFULL_EXIT;

        // Now we can allocate the storage for the result
        hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
        if (!hGlbRes)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = aplNELMRes;
        lpHeader->Rank       = 1;
#undef  lpHeader
        // Skip over the header to the dimensions
        lpMemRes = VarArrayBaseToDim (lpMemRes);

        // Save and skip over the dimension
        *((LPAPLDIM) lpMemRes)++ = aplNELMRes;

        // Copy the memory to the result
        CopyMemoryW (lpMemRes, lpwName, (APLU3264) aplNELMRes);

        // Translate the data from NARS to APL2 charset
        if (bTranslateAPL2)
            TranslateNARSToAPL2 (lpMemRes, aplNELMRes, TRUE);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF/ELSE

    goto YYALLOC_EXIT;

EMPTY_EXIT:
    // Return an empty char vector
    hGlbRes = hGlbV0Char;
YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbDfnHdr && lpMemDfnHdr)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    if (hGlbItm && lpMemItm)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbItm); lpMemItm = NULL;
    } // End IF

    return lpYYRes;
} // End SysFnDydTF1_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SysFnDydTF2_EM_YY
//
//  Return the extended transfer form (Type 2) of a given argument, or
//    return the name of a successful inverse extended transfer
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnDydTF2_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnDydTF2_EM_YY
    (LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPWCHAR lpwszTemp,                     // Ptr to right arg memory as temporary storage
     APLNELM aplNELMRht,                    // Right arg NELM
     UBOOL   bTranslateAPL2)                // TRUE iff input/output should be translated to/from APL2

{
    APLUINT       ByteRes;                  // # bytes in the result
    LPWCHAR       lpwTemp;                  // Ptr to temporary
    WCHAR         wch;                      // Temporary char
    LPPL_YYSTYPE  lpYYRes = NULL;           // Ptr to the result
    HGLOBAL       hGlbRes = NULL;           // Result global memory handle
    LPVOID        lpMemRes = NULL;          // Ptr to result global memory
    APLNELM       aplNELMRes;               // Result NELM
    STFLAGS       stFlags;                  // ST flags for name lookup
    LPSYMENTRY    lpSymEntry;               // Ptr to SYMENTRY for name lookup
    SYMENTRY      steQuadZ;                 // STE for []Z
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    UBOOL         bOldInTF;                 // Previous value of bInTF

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Save the previous value of InTF
    bOldInTF = lpMemPTD->bInTF;

    // Translate the name/data from APL2 charset to NARS
    if (bTranslateAPL2)
        TranslateAPL2ToNARS (lpwszTemp, aplNELMRht, TRUE);

    // The three cases (NameToTransferForm2 vs. TransferFormToVar2 vs. TransferFormToFcn2)
    //   are distinguished by the first occurrence of either a blank or a left arrow.
    //   If a left arrow occurs first, then it's a var; if a blank occurs first, it's a fcn;
    //   if neither occurs, it's NameToTransferForm2.
    lpwTemp = strpbrkW (lpwszTemp, WS_UTF16_LEFTARROW L" ");
    if (lpwTemp EQ NULL)
    {
        //***************************************************************
        // It's Name to Type 2 Transfer Form
        //***************************************************************

        // Set the flags for what we're looking up/appending
        ZeroMemory (&stFlags, sizeof (stFlags));
        stFlags.Inuse   = TRUE;

        // If it's a system name, ...
        if (IsSysName (lpwszTemp))
        {
            // Convert the name to lowercase (already lowercase)
            CharLowerBuffW (lpwszTemp, lstrlenW (lpwszTemp));

            // Tell 'em we're looking for system names
            stFlags.ObjName = OBJNAME_SYS;
        } else
            // Tell 'em we're looking for user names
            stFlags.ObjName = OBJNAME_USR;

        // Look up the name
        lpSymEntry = SymTabLookupName (lpwszTemp, &stFlags);
        if (!lpSymEntry)
            goto EMPTY_EXIT;

        // Split cases based upon the name type
        switch (lpSymEntry->stFlags.stNameType)
        {
            case NAMETYPE_UNK:
                goto EMPTY_EXIT;

            case NAMETYPE_VAR:
                // Display the var
                lpwTemp =
                  DisplayTransferVar2 (lpwszTemp, lpSymEntry);

                break;

            case NAMETYPE_FN0:
            case NAMETYPE_FN12:
            case NAMETYPE_OP1:
            case NAMETYPE_OP2:
            case NAMETYPE_OP3:
            case NAMETYPE_TRN:
                // Display the fcn
                lpwTemp =
                  DisplayTransferFcn2 (lpwszTemp, lpSymEntry, NULL);

                break;

            defstop
                break;
        } // End SWITCH

        // Get the string length
        aplNELMRes = lpwTemp - lpwszTemp;
    } else
    {
        // If the first one is a left arrow, ...
        if (*lpwTemp EQ UTF16_LEFTARROW)
        {
            //***************************************************************
            // It's Transfer Form To Type 2 Var
            //***************************************************************

            //***************************************************************
            // The extended transfer form of an array is as follows:
            //     varname{is}values
            //   or
            //     varname{is}shape{rho}values
            //   where 'shape' is the shape of the array, and 'values' are x/shape elements
            //***************************************************************
            if (!TransferInverseArr2_EM (lpwszTemp, aplNELMRht, 0, 0, lptkFunc, NULL, FALSE))
                goto EMPTY_EXIT;
        } else
        // If the first one is a blank, ...
        if (*lpwTemp EQ L' ')
        {
            //***************************************************************
            // It's Transfer Form To Type 2 Fcn
            //***************************************************************

            //***************************************************************
            // The extended transfer form of a function is as follows:
            //     []FX '...' '...' ...
            //   or
            //     attrs []FX '...' '...' ...
            //   where 'attrs' is a optional numeric attributes
            //***************************************************************

            // Back up three chars and insert []Z{is} so as to capture
            //   the result of []FX
            lpwszTemp -= 3;
            lpwszTemp[0] = UTF16_QUAD;
            lpwszTemp[1] = L'z';
            lpwszTemp[2] = WC_EOS;

            // Set the flags for what we're looking up/appending
            ZeroMemory (&stFlags, sizeof (stFlags));
            stFlags.Inuse   = TRUE;

////////////// If it's a system name, ...
////////////if (IsSysName (lpwszTemp))
////////////{
////////////    // Convert the name to lowercase (already lowercase)
////////////    CharLowerBuffW (lpwszTemp, lstrlenW (lpwszTemp));
////////////
                // Tell 'em we're looking for system names
                stFlags.ObjName = OBJNAME_SYS;
////////////} else
////////////    // Tell 'em we're looking for user names
////////////    stFlags.ObjName = OBJNAME_USR;

            // Look up the name
            lpSymEntry = SymTabLookupName (lpwszTemp, &stFlags);
            if (!lpSymEntry)
                goto EMPTY_EXIT;

            // Save the STE to restore later
            steQuadZ = *lpSymEntry;

            lpwszTemp[2] = UTF16_LEFTARROW;

            // Mark as fixing a function via []TF
            lpMemPTD->bInTF = TRUE;

            if (!TransferInverseFcn2_EM (lpwszTemp, 0, 0, lptkFunc, NULL, FALSE))
            {
                // If the old value is a global, free it
                if (lpSymEntry->stFlags.Value && !lpSymEntry->stFlags.Imm)
                    FreeResultGlobalVar (lpSymEntry->stData.stGlbData);

                // Restore the original value
                *lpSymEntry = steQuadZ;

                goto EMPTY_EXIT;
            } // End IF

            // If []Z has no value or it's immediate, ...
            if (!lpSymEntry->stFlags.Value
             || lpSymEntry->stFlags.Imm)
            {
                // Restore the original value
                *lpSymEntry = steQuadZ;

                goto EMPTY_EXIT;
            } // End IF

            // Capture the result from []Z
            hGlbRes = lpSymEntry->stData.stGlbData;

            // Restore the original value
            *lpSymEntry = steQuadZ;

            goto YYALLOC_EXIT;
        } else
            // We should never get here
            DbgStop ();

        // Return the name of the var/fcn we just created
        wch = *lpwTemp;
        *lpwTemp = WC_EOS;

        // Get the name length
        aplNELMRes = lstrlenW (lpwszTemp);
    } // End IF/ELSE

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Now we can allocate the storage for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbRes)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader
    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Save the dimension and skip over it
    *((LPAPLDIM) lpMemRes)++ = aplNELMRes;

    // Copy the name to the result
    CopyMemoryW (lpMemRes, lpwszTemp, (APLU3264) aplNELMRes);

    // Translate the name/data from NARS to APL2 charset
    if (bTranslateAPL2)
        TranslateNARSToAPL2 (lpMemRes, aplNELMRes, TRUE);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    goto YYALLOC_EXIT;

EMPTY_EXIT:
    // Return an empty char vector
    hGlbRes = hGlbV0Char;
YYALLOC_EXIT:
    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    if (!lptkFunc)
        AppendLine (ERRMSG_WS_FULL APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    // Restore previous value of bInTF
    lpMemPTD->bInTF = bOldInTF;

    return lpYYRes;
} // End SysFnDydTF2_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $TransferInverseFcn1_EM
//
//  Transfer form inverse of a Type-1 function
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- TransferInverseFcn1_EM"
#else
#define APPEND_NAME
#endif

UBOOL TransferInverseFcn1_EM
    (LPWCHAR lpwszTemp,                     // Ptr to incoming data
     UINT    uOldRecNo,                     // Starting record # from .atf file
     UINT    uRecNo,                        // Ending   ...
     LPTOKEN lptkFunc,                      // Ptr to function token (may be NULL if called from )IN)
     UBOOL   bSysCmd)                       // TRUE iff called from a system command

{
////LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    LPWCHAR       lpwName,                  // Ptr to name portion of array
                  lpwNameEnd = NULL,        // Ptr to end of name
                  lpwData;                  // Ptr to data ...
    WCHAR         wch;                      // Temporary character
////LPWCHAR       lpwszFormat;              // Ptr to format area
    UBOOL         bRet = FALSE;             // TRUE iff result is valid
    STFLAGS       stFlags;                  // ST flags for name lookup
    LPSYMENTRY    lpSymEntry;               // Ptr to SYMENTRY for name lookup
    APLRANK       aplRankRes;               // Result rank
    SF_FCNS       SF_Fcns = {0};            // Common struc for SaveFunctionCom
    TF1_PARAMS    TF1_Params = {0};         // Local struc for  ...

    Assert (lptkFunc NE NULL);

////// Get ptr to PerTabData global memory
////lpMemPTD = GetMemPTD ();
////
////// Get ptr to temporary format save area
////lpwszFormat = lpMemPTD->lpwszFormat;

    // Point to the name, skipping over the type char
    lpwName = &lpwszTemp[1];

    // Search for the blank which marks the end of the name
    lpwData = SkipToCharW (lpwName, L' ');
    if (!lpwData)
    {
////////if (!lptkFunc)
////////{
////////    // Format the error message
////////    wsprintfW (lpwszFormat,
////////               L"Missing blank in .atf file, lines %u-%u (origin-1), record starts with %.20s.",
////////               uOldRecNo,
////////               uRecNo,
////////               lpwszTemp);
////////    // Display the error message
////////    AppendLine (lpwszFormat, FALSE, TRUE);
////////} // End IF

        goto ERROR_EXIT;
    } // End IF

    // Copy the ptr to the name end
    //   and skip over it
    lpwNameEnd = lpwData++;

    // Ensure the name is properly terminated
    wch = *lpwNameEnd;
    *lpwNameEnd = WC_EOS;

    // Set the flags for what we're looking up/appending
    ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;

    // If it's a system name
    if (IsSysName (lpwName))
    {
        // Convert the name to lowercase
        CharLowerBuffW (lpwName, (UINT) (lpwData - lpwName));

        // Tell 'em we're looking for system names
        stFlags.ObjName = OBJNAME_SYS;
    } else
        // Tell 'em we're looking for user names
        stFlags.ObjName = OBJNAME_USR;

    // Look up the name
    lpSymEntry = SymTabLookupName (lpwName, &stFlags);
    if (!lpSymEntry)
    {
        // If it's a system name and it's not found, then we don't support it
        if (IsSysName (lpwName))
            goto INVALIDSYSNAME_EXIT;
        lpSymEntry = SymTabAppendNewName_EM (lpwName, &stFlags);
        if (!lpSymEntry)
            goto STFULL_EXIT;

        // As this is a system command, we change the global values only
        while (lpSymEntry->stPrvEntry)
            lpSymEntry = lpSymEntry->stPrvEntry;

        // Set the object name and name type value for this new entry
        lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
        lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
    } else
    // If this is a system command, we change the global values only
    if (bSysCmd)
    while (lpSymEntry->stPrvEntry)
        lpSymEntry = lpSymEntry->stPrvEntry;

    // Get the rank
    sscanfW (lpwData, L"%I64u", &aplRankRes);

    Assert (aplRankRes EQ 2);

    // Skip past the rank
    lpwData = SkipPastCharW (lpwData, L' ');

    // Get the next shape value
    sscanfW (lpwData, L"%I64u", &TF1_Params.aplRowsRht);

    // Skip past the # rows
    lpwData = SkipPastCharW (lpwData, L' ');

    // Get the next shape value
    sscanfW (lpwData, L"%I64u", &TF1_Params.aplColsRht);

    // Skip past the # cols
    lpwData = SkipPastCharW (lpwData, L' ');

    // Save ptr to the function text
    TF1_Params.lpMemRht = lpwData;

    // Fill in common values
    SF_Fcns.bDisplayErr = FALSE;            // DO NOT Display Errors
////SF_Fcns.bRet        =                   // Filled in by SaveFunctionCom
////SF_Fcns.uErrLine    =                   // ...
////SF_Fcns.lpSymName   =                   // ...
    SF_Fcns.lptkFunc    = lptkFunc;         // Ptr to function token

    // Fill in common values
    SF_Fcns.SF_LineLen      = SF_LineLenTF1;        // Ptr to line length function
    SF_Fcns.SF_ReadLine     = SF_ReadLineTF1;       // Ptr to read line function
    SF_Fcns.SF_NumLines     = SF_NumLinesTF1;       // Ptr to # lines function
    SF_Fcns.SF_CreationTime = SF_CreationTimeTF1;   // Ptr to get function creation time
    SF_Fcns.SF_LastModTime  = SF_LastModTimeTF1;    // Ptr to get function last modification time
    SF_Fcns.SF_UndoBuffer   = SF_UndoBufferTF1;     // Ptr to get function Undo Buffer global memory handle

    // Save ptr to local parameters
    SF_Fcns.LclParams = &TF1_Params;

    // Call common routine
    bRet = SaveFunctionCom (NULL, &SF_Fcns);

    if (bRet)
        goto NORMAL_EXIT;

INVALIDSYSNAME_EXIT:
////if (!lptkFunc)
////{
////    // Format the error message
////    wsprintfW (lpwszFormat,
////               L"INVALID " WS_UTF16_QUAD L"NAME in .atf file, lines %u-%u (origin-1), record starts with %.20s.",
////               uOldRecNo,
////               uRecNo,
////               lpwszTemp);
////    // Display the error message
////    AppendLine (lpwszFormat, FALSE, TRUE);
////} // End IF

    goto ERROR_EXIT;

STFULL_EXIT:
////if (!lptkFunc)
////    AppendLine (ERRMSG_SYMBOL_TABLE_FULL APPEND_NAME, FALSE, TRUE);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // Restore the char at the name end
    if (lpwNameEnd)
        *lpwNameEnd = wch;

    return bRet;
} // End TransferInverseFcn1_EM


//***************************************************************************
//  End of File: qf_tf.c
//***************************************************************************
