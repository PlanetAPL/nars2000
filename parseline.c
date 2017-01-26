//***************************************************************************
//  NARS2000 -- Parse A Line
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

//***************************************************************************
//  This parser comes from "APL two by two-syntax analysis by pairwise reduction",
//    Bunda, J.D. and Gerth, J.A.
//    APL '84 Proceedings of the International Conference on APL.
//***************************************************************************

//***************************************************************************
//  TODO
//
//  Error Handling
//  Ctrl-Break
//
//***************************************************************************

//***************************************************************************
// Aliases
//***************************************************************************
#define plRedSA_F       plRedA_F
#define plRedNAM_F      plRedA_F
#define plRedIO_F       plRedA_F
#define plRedSA_FR      plRedA_F
#define plRedNAM_FR     plRedA_F
#define plRedIO_FR      plRedA_F
#define plRedA_FR       plRedA_F

#define plRedA_IO       plRedA_A
#define plRedNAM_IO     plRedA_A
#define plRedA_NAM      plRedA_A
#define plRedNAM_NAM    plRedA_A
#define plRedNAM_A      plRedA_A
#define plRedIO_A       plRedA_A
#define plRedIO_IO      plRedA_A
#define plRedIO_NAM     plRedA_A
#define plRedSA_IO      plRedSA_A
#define plRedSA_NAM     plRedSA_A

#define plRedMF_SA      plRedMF_A
#define plRedF_A        plRedMF_A
#define plRedF_SA       plRedMF_A
#define plRedF_IO       plRedMF_A
#define plRedMF_IO      plRedMF_A
#define plRedMF_NF      plRedMF_A

#define plRedNF_IO      plRedNF_A
#define plRedNF_NAM     plRedNF_A

#define plRedF_NF       plRedMF_NF

#define plRedNAM_NF     plRedA_NF
#define plRedIO_NF      plRedA_NF

#define plRedSYSN_MF    plRedSYSN_F

#define plRedGO_SA      plRedGO_A
#define plRedGO_IO      plRedGO_A

#define plRedSA_HR      plRedA_HR
#define plRedNAM_HR     plRedA_HR
#define plRedIO_HR      plRedA_HR

#define plRedSA_HY      plRedA_HY
#define plRedIO_HY      plRedA_HY
#define plRedNAM_HY     plRedA_HY

#define plRedJD_HY      plRedDOP_HY
#define plRedDOPN_HY    plRedDOP_HY

#define plRedDOP_IDX    plRedF_IDX
#define plRedDOPN_IDX   plRedF_IDX


#define plRedDOP_SA     plRedDOP_RhtOper
#define plRedDOP_IO     plRedDOP_RhtOper
#define plRedDOPN_A     plRedDOP_RhtOper
#define plRedDOPN_SA    plRedDOP_RhtOper
#define plRedDOPN_IO    plRedDOP_RhtOper
#define plRedDOPN_F     plRedDOP_RhtOper
#define plRedDOPN_MF    plRedDOP_RhtOper
#define plRedDOPN_J     plRedDOP_RhtOper
#define plRedDOP_A      plRedDOP_RhtOper
#define plRedDOP_F      plRedDOP_RhtOper
#define plRedDOP_MF     plRedDOP_RhtOper
#define plRedDOPN_NF    plRedDOP_RhtOper
#define plRedDOP_NF     plRedDOP_RhtOper
#define plRedDOP_J      plRedDOP_RhtOper
#define plRedJ_A        plRedDOP_RhtOper
#define plRedJ_SA       plRedDOP_RhtOper
#define plRedJ_IO       plRedDOP_RhtOper
#define plRedJ_F        plRedDOP_RhtOper
#define plRedJ_NF       plRedDOP_RhtOper
#define plRedJD_F       plRedDOP_RhtOper
#define plRedJD_MF      plRedDOP_RhtOper
#define plRedJD_A       plRedDOP_RhtOper
#define plRedJD_SA      plRedDOP_RhtOper
#define plRedJD_IO      plRedDOP_RhtOper

#define plRedJ_MOP      plRedLftOper_MOP
#define plRedJ_MOPN     plRedLftOper_MOP
#define plRedSA_MR      plRedLftOper_MOP
#define plRedNAM_MR     plRedLftOper_MOP
#define plRedIO_MR      plRedLftOper_MOP
#define plRedSA_MOP     plRedLftOper_MOP
#define plRedNAM_MOP    plRedLftOper_MOP
#define plRedIO_MOP     plRedLftOper_MOP
#define plRedA_MOP      plRedLftOper_MOP
#define plRedNF_MOP     plRedLftOper_MOP
#define plRedSA_MOPN    plRedLftOper_MOP
#define plRedNAM_MOPN   plRedLftOper_MOP
#define plRedIO_MOPN    plRedLftOper_MOP
#define plRedA_MOPN     plRedLftOper_MOP
#define plRedA_MR       plRedLftOper_MOP
#define plRedF_MOP      plRedLftOper_MOP
#define plRedF_MOPN     plRedLftOper_MOP
#define plRedMF_MOP     plRedLftOper_MOP
#define plRedMF_MOPN    plRedLftOper_MOP
#define plRedF_MR       plRedLftOper_MOP
#define plRedF_HR       plRedLftOper_MOP

#define plRedHY_MOPN    plRedHY_MOP
#define plRedNF_MOPN    plRedNF_MOP

#define plRedSPM_MOP    plRedLftOper_MOP
#define plRedSPMN_MOP   plRedLftOper_MOP
#define plRedSPM_MOPN   plRedLftOper_MOP
#define plRedSPMN_MOPN  plRedLftOper_MOP

#define plRedF_RP       plRedCom_RP
#define plRedHY_RP      plRedCom_RP
#define plRedDOP_RP     plRedCom_RP

#define plRedSP_SA      plRedSP_A
#define plRedSP_ARBK    plRedSP_A
#define plRedMF_HY      plRedF_HY

#define plRedMOPN_IDX   plRedMOP_IDX

#define plRedSA_IDX     plRedA_IDX
#define plRedNAM_IDX    plRedA_IDX
#define plRedIO_IDX     plRedA_IDX

#define plRedMF_IDX     plRedF_IDX
#define plRedHY_IDX     plRedF_IDX

#define plRedSP_F       plRedSP_Com
#define plRedSP_FR      plRedSP_Com
#define plRedSP_MR      plRedSP_Com
#define plRedSP_DR      plRedSP_Com
#define plRedSP_HR      plRedSP_Com
#define plRedSP_HY      plRedSP_Com
#define plRedSP_J       plRedSP_Com
#define plRedSP_MOP     plRedSP_Com
#define plRedSP_MOPN    plRedSP_Com
#define plRedSP_DOP     plRedSP_Com
#define plRedSP_DOPN    plRedSP_Com
#define plRedSP_IO      plRedSP_Com

#define plRedNAM_SPA    plRedNAM_SPCom
#define plRedNAM_SPAK   plRedNAM_SPCom
#define plRedNAM_SPF    plRedNAM_SPCom
#define plRedNAM_SPM    plRedNAM_SPCom
#define plRedNAM_SPMN   plRedNAM_SPCom
#define plRedNAM_SPMR   plRedNAM_SPCom
#define plRedNAM_SPD    plRedNAM_SPCom
#define plRedNAM_SPDN   plRedNAM_SPCom
#define plRedNAM_SPDR   plRedNAM_SPCom
#define plRedNAM_SPHY   plRedNAM_SPCom
#define plRedNAM_SPHR   plRedNAM_SPCom
#define plRedNAM_SPFR   plRedNAM_SPCom

#define plRedA_ISPA     plRedNAM_ISPA

#define plRedNAM_NNR    plRedNAM_NR

#define plRedLNR_SPNF   plRedLNR_SPA
#define plRedLNR_ISPA   plRedLNR_SPA

#define plRedLP_FR      plRedLP_Com
#define plRedLP_MR      plRedLP_Com
#define plRedLP_DR      plRedLP_Com
#define plRedLP_HR      plRedLP_Com

#define plRedLP_AFR     plRedLP_FFR

#define plRedAFOG_SA    plRedAFOG_A
#define plRedAFOG_SPA   plRedAFOG_A
#define plRedAFOG_IO    plRedAFOG_A
#define plRedAFOR_SA    plRedAFOR_A
#define plRedAFOR_SPA   plRedAFOR_A
#define plRedAFOR_IO    plRedAFOR_A

#define plRedSA_RBK     plRedA_RBK
#define plRedIO_RBK     plRedA_RBK
#define plRedSA_SRBK    plRedA_SRBK
#define plRedIO_SRBK    plRedA_SRBK

#define plRedF_ARBK     plRedMF_ARBK

#define plRedCS1_SA     plRedCS1_A
#define plRedCS1_SPA    plRedCS1_A

#define plRedCSI_SA     plRedCSI_A
#define plRedCS1_IO     plRedCS1_A
#define plRedCSI_IO     plRedCSI_A
#define plRedSA_FFR     plRedA_FFR
#define plRedNAM_FFR    plRedA_FFR
#define plRedIO_FFR     plRedA_FFR

#define plRedRBC_MOP    plRedPseudo
#define plRedRBC_MOPN   plRedPseudo
#define plRedRBC_HY     plRedPseudo

#define plRedRBK_MOP    plRedPseudo
#define plRedRBK_MOPN   plRedPseudo
#define plRedRBK_HY     plRedPseudo

#define plRedSP_RP      plRedSYNR
#define plRedSP_RBK     plRedSYNR
#define plRedSP_RBC     plRedSYNR
#define plRedSP_EOS     plRedSYNR

#define plRedNDX_SPA    plRedIDX_SPA

#define plRedF_ISPA     plRedF_SPA

#define STRICT
#include <windows.h>
#include "headers.h"
#include "parseline.h"
#define DEFINE_VALUES
#define EXTERN
#include "tokenso.h"
#undef  EXTERN
#undef  DEFINE_VALUES


#define IsValidSO(a)                (soUNK < (a) && (a) <= soLAST)

#define POPLEFT                     ((lpplOrgLftStk < &lpMemPTD->lpplLftStk[-1]) ? *--lpMemPTD->lpplLftStk : pl_yylex (&plLocalVars))
#define POPRIGHT                    *--lpMemPTD->lpplRhtStk
#define PUSHLEFT(a)                 *lpMemPTD->lpplLftStk++ = (a)
#define PUSHRIGHT(a)                *lpMemPTD->lpplRhtStk++ = (a)
#define LBIND(lftSynObj,curSynObj)  _BIND (lftSynObj, curSynObj)    // ((IsValidSO (lftSynObj && IsValidSO (curSynObj)) ? plBndStr[lftSynObj][curSynObj] : (DbgBrk (), 0xCCC))
#define RBIND(curSynObj,rhtSynObj)  _BIND (curSynObj, rhtSynObj)    // ((IsValidSO (curSynObj && IsValidSO (rhtSynObj)) ? plBndStr[curSynObj][rhtSynObj] : (DbgBrk (), 0xFFF))
#define LFTTOKEN                   ((lpplOrgLftStk < &lpMemPTD->lpplLftStk[-1]) ? lpMemPTD->lpplLftStk[-1]->tkToken : GetLftToken  (&plLocalVars))
#define LFTSYNOBJ                   (lpplOrgLftStk < &lpMemPTD->lpplLftStk[-1]) ? lpMemPTD->lpplLftStk[-1]->tkToken.tkSynObj : GetLftSynObj (&plLocalVars)
#define CURSYNOBJ                    lpplYYCurObj->tkToken.tkSynObj
#define RHTSYNOBJ                    lpMemPTD->lpplRhtStk[-1]->tkToken.tkSynObj
#define RH2SYNOBJ                   ((RSTACKLEN > 1) ? lpMemPTD->lpplRhtStk[-2]->tkToken.tkSynObj : soNONE)
#define RH3SYNOBJ                   ((RSTACKLEN > 2) ? lpMemPTD->lpplRhtStk[-3]->tkToken.tkSynObj : soNONE)
#define LSTSYNOBJ                    lpplYYLstRht->tkToken.tkSynObj
#define LSTACKLEN                   (lpMemPTD->lpplLftStk - lpplOrgLftStk)
#define LSTACKLEN2                  (lpplLocalVars->lpMemPTD->lpplLftStk - lpplLocalVars->lpMemPTD->lpplOrgLftStk)
#define RSTACKLEN                   (lpMemPTD->lpplRhtStk - lpplOrgRhtStk)
#define RSTACKLEN2                  (lpplLocalVars->lpMemPTD->lpplRhtStk - lpplLocalVars->lpMemPTD->lpplOrgRhtStk)
#define IsTknTypeNamedVar(a)        ((a) EQ TKT_VARNAMED)

#ifdef DEBUG
//#define DEBUG_TRACE
//#define DEBUG_START
#endif

#ifdef DEBUG_TRACE
  #define TRACE(a,EVENT,soType,rhtSynObj)                         \
              dprintfWL0 (L"%s %s %3d %-4s %3d %s %s",            \
                          (a),                                    \
                          LSTACK (&plLocalVars, lpplOrgLftStk),   \
                          LBIND (LFTSYNOBJ, CURSYNOBJ),           \
                          soNames[soType],                        \
                          RBIND (CURSYNOBJ, rhtSynObj),           \
                          RSTACK (&plLocalVars, lpplOrgRhtStk),   \
                          EVENT)
  #define TRACE2(a,EVENT,soType,rhtSynObj)                        \
              dprintfWL0 (L"%s %s %3d %-4s %3d %s %s",            \
                          (a),                                    \
                          LSTACK (&plLocalVars, lpplOrgLftStk),   \
                          LBIND (LFTSYNOBJ, soType),              \
                          soNames[soType],                        \
                          RBIND (soType, rhtSynObj),              \
                          RSTACK (&plLocalVars, lpplOrgRhtStk),   \
                          EVENT)
  #define PushVarStrand_YY(a)               DbgPushVarStrand_YY    (a, FNLN)
  #define MakeVarStrand_EM_YY(a)            DbgMakeVarStrand_EM_YY (a, FNLN)
#else
  #define TRACE(a,EVENT,soType,rhtSynObj)
  #define TRACE2(a,EVENT,soType,rhtSynObj)
//#define PushVarStrand_YY(a)               PushVarStrand_YY    (a)
//#define MakeVarStrand_EM_YY(a)            MakeVarStrand_EM_YY (a)
#endif


//***************************************************************************
//  $BIND
//***************************************************************************

#ifdef DEBUG
int _BIND
    (SO_ENUM lftSynObj,
     SO_ENUM rhtSynObj)

{
    if (IsValidSO (lftSynObj) && IsValidSO (rhtSynObj))
        return plBndStr[lftSynObj][rhtSynObj];

    DbgStop ();         // ***FIXME*** -- error in reduction/binding tables

    return -1;
} // End _BIND
#else
  #define   _BIND(lftSynObj,rhtSynObj)      plBndStr[lftSynObj][rhtSynObj]
#endif


//***************************************************************************
//  $plRedPseudo
//
//  Reduce various pseudo-reductions
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedPseudo"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedPseudo
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
#ifdef DEBUG
    // I know about these cases, so don't bother me with them
    if (!(soType EQ soUNK
      && (lpplYYCurObj->tkToken.tkSynObj EQ soMOP || lpplYYCurObj->tkToken.tkSynObj EQ soMOPN || lpplYYCurObj->tkToken.tkSynObj EQ soHY)
      && (lpplYYLstRht->tkToken.tkSynObj EQ soA   || lpplYYLstRht->tkToken.tkSynObj EQ soMOP  || lpplYYLstRht->tkToken.tkSynObj EQ soHY))
       )
        DbgStop ();
#endif
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpplYYLstRht->tkToken);
    return NULL;
} // End plRedPseudo
#undef  APPEND_NAME


//***************************************************************************
//  $plRedSYNR
//
//  Reduce to a SYNTAX ERROR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedSYNR"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedSYNR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpplYYCurObj->tkToken);
    return NULL;
} // End plRedSYNR
#undef  APPEND_NAME


//***************************************************************************
//  $plRedNAM_RP
//
//  Reduce "NAM RP"
//***************************************************************************

LPPL_YYSTYPE plRedNAM_RP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Set the token type
    lpplYYCurObj->tkToken.tkFlags.TknType = TKT_VARNAMED;

    // Initialize a name strand
    InitNameStrand (lpplYYCurObj);

    // Append this name to the strand
    lpYYRes =
      PushNameStrand_YY (lpplYYCurObj);

    // Check for error
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedNAM_RP


//***************************************************************************
//  $plRedNAM_NR
//
//  Reduce "NAM NR"
//***************************************************************************

LPPL_YYSTYPE plRedNAM_NR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Set the token type
    lpplYYCurObj->tkToken.tkFlags.TknType = TKT_VARNAMED;

    // Append this name to the strand
    lpYYRes =
      PushNameStrand_YY (lpplYYCurObj);

    // Check for error
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedNAM_NR


//***************************************************************************
//  $plRedLP_NR
//
//  Reduce "LP NR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_NR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
} // End plRedLP_NR


//***************************************************************************
//  $plRedLP_NNR
//
//  Reduce "LP NNR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_NNR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    lpYYRes =
      MakeNameStrand_EM_YY (lpplYYLstRht);

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Check for error
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedLP_NNR


//***************************************************************************
//  $plRedLNR_SPA
//
//  Reduce "LNR SPA" and "LNR ISPA"
//***************************************************************************

LPPL_YYSTYPE plRedLNR_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    UBOOL bRet;                         // TRUE iff the result is valid

    // If the last right object is a niladic function, ...
    if (lpplYYLstRht->tkToken.tkSynObj EQ soSPNF)
    {
        // Execute the niladic function returning an array
        lpplYYLstRht =
          plExecuteFn0 (lpplYYLstRht);

        // Check for error
        if (lpplYYLstRht EQ NULL)
            goto ERROR_EXIT;
    } // End IF

    // If the left fcn is present (select spec as in NAM F {is} A), ...
    if (lpplYYLstRht->lpplYYFcnCurry NE NULL)
    {
        Assert (!lpplYYCurObj->YYStranding && !lpplYYLstRht->lpplYYFcnCurry->YYStranding);
        Assert ( lpplYYCurObj->tkToken.tkSynObj EQ soLNR);

        // Modify assign the names
        bRet =
          ModifyAssignNamedVars_EM (lpplYYCurObj,
                                    lpplYYLstRht->lpplYYFcnCurry,
                                   &lpplYYLstRht->tkToken);
    } else
        // Assign the names
        bRet =
          AssignNamedVars_EM (lpplYYCurObj,
                              lpplYYLstRht);
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Check for error
    if (!bRet)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
ERROR_EXIT:
    // If there is a curried fcn, ...
    if (lpplYYLstRht->lpplYYFcnCurry NE NULL)
    {
        // If the curried function is not an AFO, ...
        if (!IsTknAFO (&lpplYYLstRht->lpplYYFcnCurry->tkToken))
            // Free it recursively
            FreeResult (lpplYYLstRht->lpplYYFcnCurry);

        // YYFree it
        YYFree (lpplYYLstRht->lpplYYFcnCurry); lpplYYLstRht->lpplYYFcnCurry = NULL;
    } // End IF

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedLNR_SPA


//***************************************************************************
//  $plRedLBK_RBK
//
//  Reduce "LBK RBK"
//***************************************************************************

LPPL_YYSTYPE plRedLBK_RBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // ...    temp

    // Initialize an empty list
    lpYYVar =
      InitList0_YY (lpplYYLstRht);
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    if (lpYYVar EQ NULL)                // If not defined, free args and YYERROR
        goto ERROR_EXIT;

    lpYYRes =
      MakeList_EM_YY (lpYYVar, TRUE);
    YYFree (lpYYVar); lpYYVar = NULL;

    if (lpYYRes EQ NULL)                // If not defined, free args and YYERROR
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedLBK_RBK


//***************************************************************************
//  $plRedNF_RBK
//
//  Reduce "NF RBK"
//***************************************************************************

LPPL_YYSTYPE plRedNF_RBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (lpplYYCurObj EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedA_RBK (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_RBK


//***************************************************************************
//  $plRedNF_SRBK
//
//  Reduce "NF SRBK"
//***************************************************************************

LPPL_YYSTYPE plRedNF_SRBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (lpplYYCurObj EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedA_SRBK (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_SRBK


//***************************************************************************
//  $plRedA_RBK
//
//  Reduce "A RBK"
//***************************************************************************

LPPL_YYSTYPE plRedA_RBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the current object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnVarStrand (lpplYYCurObj);

    // Initialize a list with the arg
    lpYYRes =
      InitList1_YY (lpplYYCurObj, lpplYYCurObj);
////FreeResult (lpplYYCurObj);              // Copied w/o IncrRefCnt in PushList_YY

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedA_RBK


//***************************************************************************
//  $plRedA_SRBK
//
//  Reduce "A SRBK"
//***************************************************************************

LPPL_YYSTYPE plRedA_SRBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the current object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj);
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnVarStrand (lpplYYCurObj);

    // Push an item onto the list
    lpYYRes =
      PushList_YY (lpplYYLstRht, lpplYYCurObj, lpplYYLstRht);
////FreeResult (lpplYYCurObj);              // Copied w/o IncrRefCnt in PushList_YY

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedA_SRBK


//***************************************************************************
//  $plRedLBK_ARBK
//
//  Reduce "LBK ARBK"
//***************************************************************************

LPPL_YYSTYPE plRedLBK_ARBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    lpYYRes =
      MakeList_EM_YY (lpplYYLstRht, TRUE);

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    // Split cases based upon the token type
    switch (lpYYRes->tkToken.tkFlags.TknType)
    {
        case TKT_LSTIMMED:
            lpYYRes->tkToken.tkFlags.TknType = TKT_VARIMMED;

            break;

        case TKT_LSTARRAY:
            lpYYRes->tkToken.tkFlags.TknType = TKT_VARARRAY;

            break;

        case TKT_LSTMULT:
            break;

        defstop
            break;
    } // End SWITCH
ERROR_EXIT:
    return lpYYRes;
} // End plRedLBK_ARBK


//***************************************************************************
//  $plRedLBK_SRBK
//
//  Reduce "LBK SRBK"
//***************************************************************************

LPPL_YYSTYPE plRedLBK_SRBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Push an empty item onto the list
    lpYYRes =
      PushList_YY (lpplYYLstRht, NULL, lpplYYLstRht);

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // Call common code
    return plRedLBK_ARBK (lpplLocalVars, lpplYYCurObj, lpYYRes, soType);
} // End plRedLBK_SRBK


//***************************************************************************
//  $plRedSEMI_RBK
//
//  Reduce "SEMI RBK"
//***************************************************************************

LPPL_YYSTYPE plRedSEMI_RBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Initialize a list with an empty item
    lpYYRes =
      InitList1_YY (NULL, lpplYYCurObj);
////FreeResult (lpplYYCurObj);              // Copied w/o IncrRefCnt in PushList_YY

    // ***FIXME*** -- Do we need PushList (NULL) ???

    // If it succeeded, ...
    if (lpYYRes NE NULL)
        // Change the tkSynObj
        lpYYRes->tkToken.tkSynObj = soType;

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedSEMI_RBK


//***************************************************************************
//  $plRedSEMI_ARBK
//
//  Reduce "SEMI ARBK"
//***************************************************************************

LPPL_YYSTYPE plRedSEMI_ARBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedSEMI_ARBK


//***************************************************************************
//  $plRedSEMI_SRBK
//
//  Reduce "SEMI SRBK"
//***************************************************************************

LPPL_YYSTYPE plRedSEMI_SRBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Push an empty item onto the list
    lpYYRes =
      PushList_YY (lpplYYLstRht, NULL, lpplYYLstRht);

    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedSEMI_SRBK


//***************************************************************************
//  $plRedA_A
//
//  Reduce "A A"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedA_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedA_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Ensure the current object has a value
    if (IsTknNamed (&lpplYYCurObj->tkToken)
     && !lpplYYCurObj->tkToken.tkData.tkSym->stFlags.Value)
        goto VALUE_CUR_EXIT;

    // Ensure the last right object has a value
    if (IsTknNamed (&lpplYYLstRht->tkToken)
     && !lpplYYLstRht->tkToken.tkData.tkSym->stFlags.Value)
        goto VALUE_LST_EXIT;

    Assert (!lpplYYCurObj->YYStranding);
    Assert (!lpplYYLstRht->YYStranding);

    // Prepare for stranding
    InitVarStrand (lpplYYCurObj);

    // Append the current object to the strand
    lpYYRes =
      PushVarStrand_YY (lpplYYCurObj);

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If not defined, ...
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Append the last right object to the strand
    return plRedSA_A (lpplLocalVars, lpYYRes, lpplYYLstRht, soType);

VALUE_CUR_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpplYYCurObj->tkToken);
    goto ERROR_EXIT;

VALUE_LST_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpplYYLstRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // YYFree the current & last right objects
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedA_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedSA_A
//
//  Reduce "SA A"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedSA_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedSA_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Ensure the last right object has a value
    if (IsTknNamed (&lpplYYLstRht->tkToken)
     && !lpplYYLstRht->tkToken.tkData.tkSym->stFlags.Value)
        goto VALUE_LST_EXIT;

    Assert ( lpplYYCurObj->YYStranding);
    Assert (!lpplYYLstRht->YYStranding);

    // Append the last right object to the strand
    lpYYRes =
      PushVarStrand_YY (lpplYYLstRht);

    // If not defined, ...
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

VALUE_LST_EXIT:
    ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                              &lpplYYLstRht->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpYYRes = NULL;
NORMAL_EXIT:
    // YYFree the last right & current objects
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedSA_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedA_F
//
//  Reduce "A F"  &  "SA F"
//***************************************************************************

LPPL_YYSTYPE plRedA_F
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // If the current object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYVar; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the current object if necessary
        UnVarStrand (lpplYYCurObj);

    Assert (lpplYYLstRht->lpplYYArgCurry EQ NULL);

    // Copy to the left curry object
    lpplYYLstRht->lpplYYArgCurry = lpplYYCurObj;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

////YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;  // Do *NOT* free as it is still curried

    return lpplYYLstRht;
ERROR_EXIT:
    return NULL;
} // End plRedA_F


//***************************************************************************
//  $ConvertHY2PFO
//
//  Convert an HY to a primitive function or operator
//***************************************************************************

void ConvertHY2PFO
    (LPPL_YYSTYPE lpYYArg,              // Ptr to current PL_YYSTYPE
     TOKEN_TYPES  tknType,              // New token type
     IMM_TYPES    immType,              // New immediate type
     SO_ENUM      soEnum)               // New Syntax Object type

{
    HGLOBAL      hGlbFcn;
    LPPL_YYSTYPE lpMemFcnStr;

    // Split cases based upon the token type
    switch (lpYYArg->tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
        case TKT_OP3IMMED:
            // Convert the token properties to an immediate function/operator
            lpYYArg->tkToken.tkFlags.TknType = tknType;
            lpYYArg->tkToken.tkFlags.ImmType = immType;
            lpYYArg->tkToken.tkSynObj        = soEnum;

            break;

        case TKT_FCNARRAY:
            // Get the global memory handle
            hGlbFcn = GetGlbHandle (&lpYYArg->tkToken);

            // Lock the memory to get a ptr to it
            lpMemFcnStr = MyGlobalLockFcn (hGlbFcn);

            Assert (((LPFCNARRAY_HEADER) lpMemFcnStr)->RefCnt EQ 1);

            // Skip over the header to the YYSTYPEs
            lpMemFcnStr = FcnArrayBaseToData (lpMemFcnStr);

            Assert (lpMemFcnStr->tkToken.tkFlags.TknType EQ TKT_OP3IMMED);

            // Convert the token properties to an immediate function/operator
            lpMemFcnStr->tkToken.tkFlags.TknType = tknType;
            lpMemFcnStr->tkToken.tkFlags.ImmType = immType;
            lpMemFcnStr->tkToken.tkSynObj        = soEnum;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbFcn); lpMemFcnStr = NULL;

            // Change the tkSynObj in the outer token
            lpYYArg->tkToken.tkSynObj = soEnum;

            break;

        case TKT_OP3NAMED:          // These cases can't happen as pl_yylex converts
        case TKT_FCNNAMED:          //   them to unnamed objects.
        defstop
            break;
    } // End SWITCH
} // ConvertHY2PFO


//***************************************************************************
//  $plRedA_HY
//
//  Reduce "A HY"
//***************************************************************************

LPPL_YYSTYPE plRedA_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive function
    ConvertHY2PFO (lpplYYLstRht,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soF);
    // Call common code
    return plRedA_F (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedA_HY


//***************************************************************************
//  $plRedA_NF
//
//  Reduce "A NF"
//***************************************************************************

LPPL_YYSTYPE plRedA_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (!lpplYYCurObj->YYStranding);

    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (lpplYYLstRht EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedA_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedA_NF


//***************************************************************************
//  $plRedSA_NF
//
//  Reduce "SA NF"
//***************************************************************************

LPPL_YYSTYPE plRedSA_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYCurObj->YYStranding);

    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (lpplYYLstRht EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedSA_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedSA_NF


//***************************************************************************
//  $plRedSYSN_F
//
//  Reduce "SYSN F"
//***************************************************************************

LPPL_YYSTYPE plRedSYSN_F
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Copy the system namespace level
    lpplYYLstRht->tkToken.tkFlags.SysNSLvl = lpplYYCurObj->tkToken.tkFlags.SysNSLvl;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedSYSN_F


//***************************************************************************
//  $plRedDOP_RhtOper
//
//  Reduce "DOP RhtOper"
//***************************************************************************

LPPL_YYSTYPE plRedDOP_RhtOper
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If the last right object is a var, ...
    if (IsTknTypeVar (lpplYYLstRht->tkToken.tkFlags.TknType))
    {
        // If the last right object is in the process of stranding, ...
        if (lpplYYLstRht->YYStranding)
        {
            // Turn this strand into a var
            lpYYRes =
              MakeVarStrand_EM_YY (lpplYYLstRht);
            // YYFree the last right object
            YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            // If not defined, ...
            if (lpYYRes EQ NULL)
                goto ERROR_EXIT;

            // Copy to the last right object
            lpplYYLstRht = lpYYRes; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
            lpYYRes = NULL;
        } else
            // Unstrand the last right object if necessary
            UnVarStrand (lpplYYLstRht);
    } // End IF

    // The result is always the root of the function tree
    lpYYRes = lpplYYCurObj;

    // Append the right operand to the function strand
    Assert (lpYYRes->lpplYYOpRCurry EQ NULL);
    lpYYRes->lpplYYOpRCurry = lpplYYLstRht;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedDOP_RhtOper


//***************************************************************************
//  $plRedDOP_HY
//
//  Reduce "DOP HY"
//***************************************************************************

LPPL_YYSTYPE plRedDOP_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive function
    ConvertHY2PFO (lpplYYLstRht,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soF);
    // Call common code
    return plRedDOP_RhtOper (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedDOP_HY


//***************************************************************************
//  $plRedJ_HY
//
//  Reduce "J HY"
//***************************************************************************

LPPL_YYSTYPE plRedJ_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive function
    ConvertHY2PFO (lpplYYLstRht,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soF);
    // Call common code
    return plRedLftOper_MOP (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedJ_HY


//***************************************************************************
//  $plRedGO_A
//
//  Reduce "GO A"  &  "GO SA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedGO_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedGO_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // Ptr to a temp
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

    // Check for Ctrl-Break
    if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
        goto ERROR_EXIT;
    else
    {
        lpplLocalVars->ExitType = GotoLine_EM (&lpplYYLstRht->tkToken, &lpplYYCurObj->tkToken);

        // Get ptr to PerTabData global memory
        lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

        // If we're tracing this line, ...
        if (lpplLocalVars->bTraceLine)
        {
            // Save the last right object (GOTO target) for tracing
            CopyAll (&lpMemPTD->YYResExec, lpplYYLstRht);

            // Tell the caller to free the var after tracing
            lpplLocalVars->bTraceFree = TRUE;
        } else
        {
            // Make a PL_YYSTYPE NoValue entry
            lpYYVar =
              MakeNoValue_YY (&lpplYYLstRht->tkToken);
            CopyAll (&lpMemPTD->YYResExec, lpYYVar);
            YYFree (lpYYVar); lpYYVar = NULL;
        } // End IF/ELSE

        // Split cases based upon the Exit Type
        switch (lpplLocalVars->ExitType)
        {
            case EXITTYPE_GOTO_ZILDE:  // {zilde}
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&lpplYYCurObj->tkToken);

                break;

            case EXITTYPE_GOTO_LINE:   // Valid line #
                // If we're not at a LABELSEP, EOS or EOL, YYERROR
                if (lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_LABELSEP
                 && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOS
                 && lpplLocalVars->lptkNext[-1].tkFlags.TknType NE TKT_EOL)
                {
                    PrimFnSyntaxError_EM (&lpplYYCurObj->tkToken APPEND_NAME_ARG);

                    goto ERROR_EXIT;
                } else
                {
                    // Tell the caller to stop executing this line
                    lpplLocalVars->bStopExec = TRUE;

                    break;
                } //End IF

            case EXITTYPE_ERROR:       // Error
            case EXITTYPE_STACK_FULL:  // ...
                goto ERROR_EXIT;

            defstop
                break;
        } // End SWITCH
    } // End IF/ELSE

/// // Change the tkSynObj
/// lpYYRes->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpplLocalVars->ExitType = EXITTYPE_ERROR;
NORMAL_EXIT:
    // If we're NOT tracing this line, ...
    if (!lpplLocalVars->bTraceLine)
        // Free (unnamed) last right object
        FreeTempResult (lpplYYLstRht);

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedGO_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedGO_NF
//
//  Reduce "GO NF"
//***************************************************************************

LPPL_YYSTYPE plRedGO_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (lpplYYLstRht EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedGO_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedGO_NF


//***************************************************************************
//  $plRedA_FFR
//
//  Reduce "A FFR" & "SA FFR"
//***************************************************************************

LPPL_YYSTYPE plRedA_FFR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If the curent object is in the process of stranding, ...
    if (lpplYYCurObj->YYStranding)
    {
        // Turn this strand into a var
        lpYYRes =
          MakeVarStrand_EM_YY (lpplYYCurObj);
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;

        // Copy to the current object
        lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
        lpYYRes = NULL;
    } else
        // Unstrand the current object if necessary
        UnVarStrand (lpplYYCurObj);

    // Start with the root
    lpYYRes = lpplYYLstRht;

    // While the FcnCurry slot is in use, ...
    while (lpYYRes->lpplYYFcnCurry NE NULL)
        // Recurse through the root to the next available lpplYYFcnCurry
        lpYYRes = lpYYRes->lpplYYFcnCurry;

    // Link in the new function
    lpYYRes->lpplYYFcnCurry = lpplYYCurObj;

    // The result is the root
    lpYYRes = lpplYYLstRht;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    return lpYYRes;

ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // curLstRht = soNONE;

    return lpplYYLstRht;
} // End plRedA_FFR


//***************************************************************************
//  $plRedCSI_A
//
//  Reduce "CSI A"  &  "CSI SA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedCSI_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedCSI_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

    // Check for Ctrl Strucs in AFO
    if (lpplLocalVars->bAfoCtrlStruc)  // FOR
        goto SYNTAX_EXIT;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                              &lpplYYCurObj->tkToken);
    goto ERROR_EXIT;

ERROR_EXIT:
    // If the last right object is defined, ...
    if (lpplYYLstRht NE NULL)
    {
        // Free (unnamed) and YYFree the last right object
        FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // curSynObj = soNONE;
    } // End IF
NORMAL_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedCSI_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedCSF_NAM
//
//  Reduce "CSF NAM"
//***************************************************************************

LPPL_YYSTYPE plRedCSF_NAM
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Change the tkSynObj
    lpplYYCurObj->tkToken.tkSynObj = soType;

    Assert (lpMemPTD->ForToken.tkToken.tkFlags.TknType EQ TKT_UNUSED);
    Assert (lpMemPTD->ForName .tkToken.tkFlags.TknType EQ TKT_UNUSED);

    // Save the :FOR token and the NAM token
    CopyAll (&lpMemPTD->ForToken, lpplYYCurObj);
    CopyAll (&lpMemPTD->ForName , lpplYYLstRht);

    // Free the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return lpplYYCurObj;
} // End plRedCSF_NAM


//***************************************************************************
//  $plRedCSFN_CSIA
//
//  Reduce "CSFN CSIA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedCSFN_CSIA"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedCSFN_CSIA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPPL_YYSTYPE lpYYRes = NULL;        // Ptr to the result

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

    // Check for Ctrl Strucs in AFO
    if (lpplLocalVars->bAfoCtrlStruc)
        goto SYNTAX_EXIT;

    // Handle FOR statement
    if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
        lpplLocalVars->bRet = FALSE;
    else
    {
        lpplLocalVars->bRet =
          CS_FOR_Stmt_EM (lpplLocalVars, &lpMemPTD->ForToken, &lpMemPTD->ForName, lpplYYLstRht);

        if (lpplLocalVars->bRet)
            // No return value needed
            lpYYRes = MakeNoValue_YY (&lpplYYCurObj->tkToken);
    } // End IF/ELSE

    //                    YYFree the current object
                                   YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    // Free (unnamed) and YYFree the last right object
    FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    ZeroMemory (&lpMemPTD->ForToken, sizeof (lpMemPTD->ForToken));
    ZeroMemory (&lpMemPTD->ForName , sizeof (lpMemPTD->ForName ));

    if (!lpplLocalVars->bRet)
        // Mark as in error
        lpplLocalVars->ExitType = EXITTYPE_ERROR;

    return lpYYRes;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    return NULL;
} // End plRedCSFN_CSIA
#undef  APPEND_NAME


//***************************************************************************
//  $plRedA_HR
//
//  Reduce "A HR"
//***************************************************************************

LPPL_YYSTYPE plRedA_HR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive function
    ConvertHY2PFO (lpplYYLstRht,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soF);
    // Call common code
    return plRedA_FR (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedA_HR


//***************************************************************************
//  $plRedA_IDX
//
//  Reduce "A IDX"  &  "SA IDX"
//***************************************************************************

LPPL_YYSTYPE plRedA_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYVar,               // Ptr to a temp
                 lpYYRes2,              // ...
                 lpYYVar2 = NULL;       // ...

    // Copy to temp vars
    lpYYRes = lpplYYCurObj;
    lpYYVar = lpplYYLstRht;

    // Loop until no more curried indices
    while (lpYYVar NE NULL)
    {
        // If the current object is in the process of stranding, ...
        if (lpYYRes->YYStranding)
        {
            // Turn this strand into a var
            lpYYVar2 =
              MakeVarStrand_EM_YY (lpYYRes);
            // YYFree the result object
            YYFree (lpYYRes); lpYYRes = NULL;

            // If not defined, ...
            if (lpYYVar2 EQ NULL)
                goto ERROR_EXIT;

            // Copy to the current object
            lpYYRes = lpYYVar2;
            lpYYVar2 = NULL;
        } else
            // Unstrand the result object if necessary
            UnVarStrand (lpYYRes);

        if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
            lpYYRes2 = NULL;
        else
            lpYYRes2 = ArrayIndexRef_EM_YY (&lpYYRes->tkToken, &lpYYVar->tkToken);

        // Save the next curried index (if any)
        lpYYVar2 = lpYYVar->lpplYYIdxCurry;

        // Free (unnamed) and YYFree the objects
        FreeTempResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
        FreeTempResult (lpYYVar); YYFree (lpYYVar); lpYYVar = NULL;

        // Copy back and repeat
        lpYYRes = lpYYRes2;
        lpYYVar = lpYYVar2;

        // Check for errors
        if (lpYYRes EQ NULL)
            break;
    } // End WHILE

    // While the curried index is valid, ...
    while (lpYYVar NE NULL)
    {
        // Save the next curried index (if any)
        lpYYVar2 = lpYYVar->lpplYYIdxCurry;

        // Free (unnamed) and YYFree the curried index
        FreeTempResult (lpYYVar); YYFree (lpYYVar); lpYYVar = NULL;

        // Copy back and repeat
        lpYYVar = lpYYVar2;
    } // End WHILE

    // If it succeeded, ...
    if (lpYYRes NE NULL)
        // Change the tkSynObj
        lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedA_IDX


//***************************************************************************
//  $plRedMF_A
//
//  Reduce "MF A"  &  "MF SA"  &  "MF NF"
//         " F A"  &  " F SA"  &  " F NF"
//***************************************************************************

LPPL_YYSTYPE plRedMF_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    TOKEN        tkLftArg = {0};        // Copy of left arg token
    LPTOKEN      lptkLftArg;            // Ptr to copy of left arg token
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                *lplpYYArgCurry = NULL; // Ptr to ptr to Arg curry (if any)

    // Ensure that the current object is a function
    Assert (IsTknFcnOpr (&lpplYYCurObj->tkToken));

    // If this function has a curried arg, ...
    if (lpplYYCurObj->lpplYYArgCurry NE NULL)
        // Point to the curried arg
        lplpYYArgCurry = &lpplYYCurObj->lpplYYArgCurry;
    else
    // If there is a curried function with a curried arg, ...
    if (lpplYYCurObj->lpplYYFcnCurry NE NULL
     && lpplYYCurObj->lpplYYFcnCurry->lpplYYArgCurry)
        // Point to the curried arg
        lplpYYArgCurry = &lpplYYCurObj->lpplYYFcnCurry->lpplYYArgCurry;

    // If a left arg is present, ...
    if (lplpYYArgCurry NE NULL)
    {
        // Copy the left arg token ptr
        CopyAll (&tkLftArg, &(*lplpYYArgCurry)->tkToken);
        lptkLftArg = &tkLftArg;

        // YYFree the curried arg
        YYFree (*lplpYYArgCurry); *lplpYYArgCurry = NULL;

        // If the left arg is a named var, ...
        if (IsTknNamedVar (lptkLftArg))
        {
            // This DEBUG stmt probably never is triggered because
            //    pl_yylex converts all unassigned named vars to temps
#ifdef DEBUG
            DbgStop ();         // ***Probably never executed***
#endif
            // Increment the refcnt
            DbgIncrRefCntTkn (lptkLftArg);      // EXAMPLE:  ***Probably never executed***
        } // End IF
    } else
        // Set the left arg token ptr
        lptkLftArg = NULL;

    // Unstrand the function if appropriate
    UnFcnStrand_EM (&lpplYYCurObj, NAMETYPE_FN12, FALSE);

    // Ensure that the current object is a function
    Assert (IsTknFcnOpr (&lpplYYCurObj->tkToken));

    // If the last right object is invalid, ...
    if (lpplYYLstRht EQ NULL)
        // This can occur if called by (say) plRedMF_ARBK without validating lpplYYLstRht
        //   so we can call UnFcnStrand_EM on the current object before signalling an error.
        goto ERROR_EXIT;

    // If the last right object is a niladic function, ...
    if (lpplYYLstRht->tkToken.tkSynObj EQ soNF)
    {
        // Execute the niladic function returning an array
        lpplYYLstRht =
          plExecuteFn0 (lpplYYLstRht);

        // Check for error
        if (lpplYYLstRht EQ NULL)
            goto ERROR_EXIT;
    } else
    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYRes =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYRes = NULL;
    } else
    {
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

        // If this is a named var, ...
        if (IsTknNamedVar (&lpplYYLstRht->tkToken))
        {
            // This DEBUG stmt probably never is triggered because
            //    pl_yylex converts all unassigned named vars to temps
#ifdef DEBUG
            DbgStop ();             // ***Probably never executed***
#endif
            // Increment the refcnt
            DbgIncrRefCntTkn (&lpplYYLstRht->tkToken);  // EXAMPLE:  ***Probably never executed***
        } // End IF
    } // End IF/ELSE

    Assert (!lpplYYCurObj->YYStranding && !lpplYYLstRht->YYStranding);

    // Save the caret position of this function
    lpplLocalVars->lpMemPTD->lpSISCur->iCharIndex = lpplYYCurObj->tkToken.tkCharIndex;

    // Execute the function between the curried left arg and the last right arg
    lpYYRes =
      ExecFunc_EM_YY (lptkLftArg, lpplYYCurObj, &lpplYYLstRht->tkToken);

    // If the left arg is present, ...
    if (lptkLftArg NE NULL)
    {
        // Free the left arg
        FreeResultTkn (lptkLftArg);
    } // End IF

    // Free and YYFree the function
    FreeResult (lpplYYCurObj); YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Free and YYFree the last right arg
    FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // If not defined, ...
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    return lpYYRes;

ERROR_EXIT:
    if (lpplYYCurObj NE NULL)
    {
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    } // End IF

    return NULL;
} // End plRedMF_A


//***************************************************************************
//  $plRedMF_ARBK
//
//  Reduce "MF ARBK"
//***************************************************************************

LPPL_YYSTYPE plRedMF_ARBK
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYVar1,              // Ptr to temp
                 lpYYVar2;              // ...

    // Pop the most recent item from the list
    //   returning the item and shortening the list in <lpplYYLstRht>
    lpYYVar1 =
      PopList_YY (lpplYYLstRht);

    // Check for error in plRedMF_A after calling UnFcnStrand_EM on the current object
////// Check for error
////if (lpYYVar1 EQ NULL)
////    goto ERROR_EXIT;

    // Execute the function on the var
    lpYYVar2 =
      plRedMF_A (lpplLocalVars, lpplYYCurObj, lpYYVar1, soA);

    // Note these vars have been freed by <plRedMF_A>
    lpplYYCurObj = lpYYVar1 = NULL;

    // Check for error
    if (lpYYVar2 EQ NULL)
        goto ERROR_EXIT;

    // Push the result onto the list
    lpYYRes =
      PushList_YY (lpplYYLstRht, lpYYVar2, lpplYYLstRht);

    // YYFree the temp
    YYFree (lpYYVar2); lpYYVar2 = NULL;

    // Check for error
    if (lpYYRes EQ NULL)
        goto ERROR_EXIT;

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    return lpYYRes;
ERROR_EXIT:
    if (lpplYYCurObj NE NULL)
    {
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    } // End IF

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedMF_ARBK


//***************************************************************************
//  $plRedCS1_A
//
//  Reduce "CS1 A"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedCS1_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedCS1_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp
    TOKEN        tkToken;               // The incoming token

    // Save the token type
    CopyAll (&tkToken, &lpplYYCurObj->tkToken);

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

    // Split cases based upon the token type
    switch (tkToken.tkFlags.TknType)
    {
        case TKT_CS_ANDIF:
        case TKT_CS_ELSEIF:
        case TKT_CS_IF:
        case TKT_CS_ORIF:
        case TKT_CS_UNTIL:
        case TKT_CS_WHILE:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)
                goto SYNTAX_EXIT;

            // Handle ANDIF/ELSEIF/IF/ORIF/UNTIL/WHILE statements
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_IF_Stmt_EM (lpplLocalVars, &tkToken, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_IF_Stmt_EM already set the error msg

        case TKT_CS_ASSERT:
            // Handle ASSERT statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ASSERT_Stmt_EM (lpplLocalVars, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_ASSERT_Stmt_EM already set the error msg

        case TKT_CS_CASE:
        case TKT_CS_CASELIST:
            // Handle CASE/CASELIST statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_CASE_Stmt (lpplLocalVars, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_CONTINUEIF:
            // Handle CONTINUEIF statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_CONTINUEIF_Stmt_EM (lpplLocalVars, &tkToken, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_CONTINUEIF_Stmt_EM already set the error msg

        case TKT_CS_GOTO:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)
                goto SYNTAX_EXIT;

            // Call common code
            return plRedGO_A (lpplLocalVars, NULL, lpplYYLstRht, soType);

        case TKT_CS_LEAVEIF:
            // Handle LEAVEIF statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_LEAVEIF_Stmt_EM (lpplLocalVars, &tkToken, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_LEAVEIF_Stmt_EM already set the error msg

        case TKT_CS_SELECT:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)
                goto SYNTAX_EXIT;

            // Handle SELECT statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_SELECT_Stmt_EM (lpplLocalVars, &tkToken, lpplYYLstRht);
            // Free (unnamed) and YYFree the last right object
            FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            if (lpplLocalVars->bRet)
                // No return value needed
                return MakeNoValue_YY (&tkToken);

            // Mark as in error
            lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed
            goto ERROR_EXIT;    // Not SYNTAX ERROR as CS_SELECT_Stmt_EM already set the error msg

        defstop
            break;
    } // End SWITCH

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End plRedCS1_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedF_IDX
//
//  Reduce "F IDX"
//***************************************************************************

LPPL_YYSTYPE plRedF_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes,               // Ptr to the result
                 lpYYVar;               // ...    temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYRes =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYRes = NULL;
    } else
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

    // The result is the current object
    lpYYRes = lpplYYCurObj;

    // Make it into an axis operand
    lpYYVar = MakeAxis_YY (lpplYYLstRht);

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // Link it into the result
    Assert (lpYYRes->lpplYYIdxCurry EQ NULL);
    lpYYRes->lpplYYIdxCurry = lpYYVar;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedF_IDX


//***************************************************************************
//  $plRedLBC_RBC
//
//  Reduce "LBC RBC"
//***************************************************************************

LPPL_YYSTYPE plRedLBC_RBC
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    Assert (lpplYYLstRht->lptkLftBrace[1].tkFlags.TknType EQ TKT_GLBDFN );

    // Get the global memory handle
    lpYYRes->tkToken.tkData.tkGlbData = lpplYYLstRht->lptkLftBrace[1].tkData.tkGlbData;

    // Set the caret index to the left brace
    lpYYRes->tkToken.tkCharIndex      = lpplYYCurObj->tkToken.tkCharIndex;

    // YYFree the current and last right objects
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // Set the UDFO/AFO properties including the proper <tkSynObj>
    //   based upon the AFO's DFNTYPE_xxx as the caller can't predict
    //   the DFNTYPE_xxx is based only on the two tokens {} it can see.
    plSetDfn (&lpYYRes->tkToken, GetGlbDataToken (&lpYYRes->tkToken));

    // Increment the RefCnt
    DbgIncrRefCntTkn (&lpYYRes->tkToken);   // EXAMPLE:  {omega}23

////// Change the tkSynObj (already set by <plSetDfn>, possibly to soNF)
////lpYYRes->tkToken.tkSynObj = soType;

    return lpYYRes;
} // End plRedLBC_RBC


//***************************************************************************
//  $plRedF_HY
//
//  Reduce "F HY"
//***************************************************************************

LPPL_YYSTYPE plRedF_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive operator
    ConvertHY2PFO (lpplYYLstRht,
                   TKT_OP1IMMED,
                   IMMTYPE_PRIMOP1,
                   soMOP);
    // Call common code
    return plRedLftOper_MOP (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedF_HY


//***************************************************************************
//  $plRedCom_RP
//
//  Reduce "Com RP"
//***************************************************************************

LPPL_YYSTYPE plRedCom_RP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstRhtObj = soNONE;

    // Change the tkSynObj
    lpplYYCurObj->tkToken.tkSynObj = soType;

    return lpplYYCurObj;
} // End plRedCom_RP


//***************************************************************************
//  $plRedMOP_RP
//
//  Reduce "MOP RP"
//***************************************************************************

LPPL_YYSTYPE plRedMOP_RP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstRhtObj = soNONE;

    // Change the tkSynObj
    lpplYYCurObj->tkToken.tkSynObj = soType;

    return lpplYYCurObj;
} // End plRedMOP_RP


//***************************************************************************
//  $plRedHY_MR
//
//  Reduce "HY MR"
//***************************************************************************

LPPL_YYSTYPE plRedHY_MR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive function
    ConvertHY2PFO (lpplYYCurObj,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soF);
    // Call common code
    return plRedLftOper_MOP (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedHY_MR


//***************************************************************************
//  $plRedHY_MOP
//
//  Reduce "HY MOP"
//***************************************************************************

LPPL_YYSTYPE plRedHY_MOP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive function
    ConvertHY2PFO (lpplYYCurObj,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soF);
    // Call common code
    return plRedLftOper_MOP (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedHY_MOP


//***************************************************************************
//  $plRedLftOper_MOP
//
//  Reduce "LftOper MOP"
//***************************************************************************

LPPL_YYSTYPE plRedLftOper_MOP
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If the current object is a var, ...
    if (IsTknTypeVar (lpplYYCurObj->tkToken.tkFlags.TknType))
    {
        // If the current object is in the process of stranding, ...
        if (lpplYYCurObj->YYStranding)
        {
            // Turn this strand into a var
            lpYYRes =
              MakeVarStrand_EM_YY (lpplYYCurObj);
            // YYFree the current object
            YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

            // If not defined, ...
            if (lpYYRes EQ NULL)
                goto ERROR_EXIT;

            // Copy to the current object
            lpplYYCurObj = lpYYRes; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
            lpYYRes = NULL;
        } else
            // Unstrand the current object if necessary
            UnVarStrand (lpplYYCurObj);
    } // End IF

    // The result is always the root of the function tree
    lpYYRes = lpplYYLstRht;

    // If the current object is an axis operand, ...
    if (IsTknTypeAxis (lpplYYCurObj->tkToken.tkFlags.TknType))
    {
        Assert (lpYYRes->lpplYYIdxCurry EQ NULL);

        // Append the axis operand to the function strand
        lpYYRes->lpplYYIdxCurry = lpplYYCurObj;
    } else
    {
        Assert (lpYYRes->lpplYYFcnCurry EQ NULL);

        // Append the left operand to the function strand
        lpYYRes->lpplYYFcnCurry = lpplYYCurObj;
    } // End IF/ELSE

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;
ERROR_EXIT:
    return lpYYRes;
} // End plRedLftOper_MOP


//***************************************************************************
//  $plRedHY_FR
//
//  Reduce "HY FR"
//***************************************************************************

LPPL_YYSTYPE plRedHY_FR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive function
    ConvertHY2PFO (lpplYYCurObj,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soF);
    // Call common code
    return plRedF_FR (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedHY_FR


//***************************************************************************
//  $plRedF_FR
//
//  Reduce "F FR"
//***************************************************************************

LPPL_YYSTYPE plRedF_FR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Unstrand the function if appropriate
    UnFcnStrand_EM (&lpplYYCurObj, TranslateSOTypeToNameType (lpplYYCurObj->tkToken.tkSynObj), FALSE);

    // Unstrand the function if appropriate
    UnFcnStrand_EM (&lpplYYLstRht, TranslateSOTypeToNameType (lpplYYLstRht->tkToken.tkSynObj), FALSE);

    // Make a pseudo train operator as the result
    lpYYRes =
      MakeTrainOp_YY (lpplYYCurObj);

    Assert (lpplYYLstRht->lpplYYArgCurry EQ NULL);
    Assert (lpplYYLstRht->lpplYYIdxCurry EQ NULL);
    Assert (lpplYYLstRht->lpplYYOpRCurry EQ NULL);

    Assert (lpplYYCurObj->lpplYYArgCurry EQ NULL);
    Assert (lpplYYCurObj->lpplYYIdxCurry EQ NULL);
    Assert (lpplYYCurObj->lpplYYOpRCurry EQ NULL);

    // Link in the functions
    Assert (lpYYRes     ->lpplYYFcnCurry EQ NULL);
    lpYYRes     ->lpplYYFcnCurry = lpplYYLstRht;

    Assert (lpplYYLstRht->lpplYYFcnCurry EQ NULL);
    lpplYYLstRht->lpplYYFcnCurry = lpplYYCurObj;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    // Return the root
    return lpYYRes;
} // End plRedF_FR


//***************************************************************************
//  $plRedHY_FFR
//
//  Reduce "HY FFR"
//***************************************************************************

LPPL_YYSTYPE plRedHY_FFR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Convert the HY to a primitive function
    ConvertHY2PFO (lpplYYCurObj,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soF);
    // Call common code
    return plRedF_FFR (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedHY_FFR


//***************************************************************************
//  $plRedF_FFR
//
//  Reduce "F FFR"
//***************************************************************************

LPPL_YYSTYPE plRedF_FFR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // Unstrand the function if appropriate
    UnFcnStrand_EM (&lpplYYCurObj, TranslateSOTypeToNameType (lpplYYCurObj->tkToken.tkSynObj), FALSE);

    // Start with the root
    lpYYRes = lpplYYLstRht;

    // While the FcnCurry slot is in use, ...
    while (lpYYRes->lpplYYFcnCurry NE NULL)
        // Recurse through the root to the next available lpplYYFcnCurry
        lpYYRes = lpYYRes->lpplYYFcnCurry;

    // Link in the new function
    lpYYRes->lpplYYFcnCurry = lpplYYCurObj;

    // The result is the root
    lpYYRes = lpplYYLstRht;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    // Return the root
    return lpYYRes;
} // End plRedF_FFR


//***************************************************************************
//  $plRedLP_FFR
//
//  Reduce "LP FFR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_FFR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Initialize the function strand (Train) base
    lpplYYLstRht->lpYYStrandBase = lpplYYLstRht->lpYYFcnBase;

    // Unstrand the function if appropriate
    UnFcnStrand_EM (&lpplYYLstRht, NAMETYPE_TRN, TRUE);

    if (lpplYYLstRht NE NULL)       // If valid, ...
        // Change the tkSynObj
        lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
} // End plRedLP_FFR


//***************************************************************************
//  $plRedLP_HFR
//
//  Reduce "LP HFR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_HFR
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_OP1IMMED
         && lpplYYLstRht->tkToken.tkData.tkChar   EQ INDEX_OPTRAIN
         && lpplYYLstRht->tkToken.tkSynObj        EQ soHFR);

    // Convert the OP1 to a primitive function
    ConvertHY2PFO (lpplYYLstRht,
                   TKT_FCNIMMED,
                   IMMTYPE_PRIMFCN,
                   soFFR);
    // Call common code
    return plRedLP_FFR (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
} // End plRedLP_HFR


//***************************************************************************
//  $plRedNF_A
//
//  Reduce "NF A"
//***************************************************************************

LPPL_YYSTYPE plRedNF_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (lpplYYCurObj EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedA_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_A


//***************************************************************************
//  $plRedNF_F
//
//  Reduce "NF F"
//***************************************************************************

LPPL_YYSTYPE plRedNF_F
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Unstrand the function if appropriate
    UnFcnStrand_EM (&lpplYYLstRht, NAMETYPE_FN12, FALSE);

    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (lpplYYCurObj EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedA_F (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // FreeResult (lpplYYLstRht); // ***FIXME***

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_F


//***************************************************************************
//  $plRedNF_HY
//
//  Reduce "NF HY"
//***************************************************************************

LPPL_YYSTYPE plRedNF_HY
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (lpplYYCurObj EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedA_HY (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_HY


//***************************************************************************
//  $plRedNF_IDX
//
//  Reduce "NF IDX"
//***************************************************************************

LPPL_YYSTYPE plRedNF_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (lpplYYCurObj EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedA_IDX (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return NULL;
} // End plRedNF_IDX


//***************************************************************************
//  $plRedNF_NF
//
//  Reduce "NF NF"
//***************************************************************************

LPPL_YYSTYPE plRedNF_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYCurObj =
      plExecuteFn0 (lpplYYCurObj);

    // Check for error
    if (lpplYYCurObj EQ NULL)
        goto ERROR_EXIT;

#ifdef DEBUG
    // Decrement the SI level of lpplYYCurObj
    //   so YYResIsEmpty won't complain
    lpplYYCurObj->SILevel--;
#endif
    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);
#ifdef DEBUG
    // Restore the SI level of lpplYYCurObj
    lpplYYCurObj->SILevel++;
#endif
    // Check for error
    if (lpplYYLstRht EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedA_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    if (lpplYYCurObj NE NULL)
    {
        // YYFree the current object
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    } // End IF

    if (lpplYYLstRht NE NULL)
    {
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;
    } // End IF

    return NULL;
} // End plRedNF_NF


//***************************************************************************
//  $plRedMOP_IDX
//
//  Reduce "MOP IDX"
//***************************************************************************

LPPL_YYSTYPE plRedMOP_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

    // Make it into an axis operand
    lpYYVar = MakeAxis_YY (lpplYYLstRht);

    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    // Call common code
    return plRedLftOper_MOP (lpplLocalVars, lpYYVar, lpplYYCurObj, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedMOP_IDX


//***************************************************************************
//  $plRedSP_A
//
//  Reduce "SP A"  &  "SP SA"
//***************************************************************************

LPPL_YYSTYPE plRedSP_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // curSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    // Set the NoDisplay flag
    lpplYYLstRht->tkToken.tkFlags.NoDisplay = TRUE;

    return lpplYYLstRht;
ERROR_EXIT:
    // YYFree the last right object
    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedSP_A


//***************************************************************************
//  $plRedSP_NF
//
//  Reduce "SP NF"
//***************************************************************************

LPPL_YYSTYPE plRedSP_NF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // If the last right object is a system function, ...
    if (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_FCNNAMED
     && lpplYYLstRht->tkToken.tkData.tkSym->stFlags.FcnDir)
    {
        Assert (lpplYYLstRht->tkToken.tkData.tkSym->stFlags.stNameType EQ NAMETYPE_FN0);

        // Execute the niladic function returning an array
        lpplYYLstRht =
          plExecuteFn0 (lpplYYLstRht);

        // Check for error
        if (lpplYYLstRht EQ NULL)
            goto ERROR_EXIT;

        // Call common code
        return plRedSP_A (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soSPA);
    } // End IF

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;
} // End plRedSP_NF


//***************************************************************************
//  $plRedSP_Com
//
//  Reduce "SP Com"
//***************************************************************************

LPPL_YYSTYPE plRedSP_Com
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
} // End plRedSP_Com


//***************************************************************************
//  $plRedIO_SPA
//
//  Reduce "IO SPA"
//***************************************************************************

LPPL_YYSTYPE plRedIO_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    UBOOL bEndingCR;                    // TRUE iff last line has CR

    // Check for ending CR
    bEndingCR = lpplYYCurObj->tkToken.tkData.tkChar EQ UTF16_QUAD;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Mark as NOT already displayed
    lpplYYLstRht->tkToken.tkFlags.NoDisplay = FALSE;
    if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
        lpplLocalVars->bRet = FALSE;
    else
        lpplLocalVars->bRet =
          ArrayDisplay_EM (&lpplYYLstRht->tkToken, bEndingCR, &lpplLocalVars->bCtrlBreak);

    if (!lpplLocalVars->bRet)
    {
        FreeResult (lpplYYLstRht);

        goto ERROR_EXIT;
    } // End IF

    // Mark as already displayed
    lpplYYLstRht->tkToken.tkFlags.NoDisplay = TRUE;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;

ERROR_EXIT:
    return NULL;
} // End plRedIO_SPA


//***************************************************************************
//  $plRedNAM_SPNF
//
//  Reduce "NAM SPNF"
//***************************************************************************

LPPL_YYSTYPE plRedNAM_SPNF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // If the token is an AFO, ...
    if (IsTknAFO (&lpplYYLstRht->tkToken))
    {
        Assert (soType EQ soA);

        // Call common code
        return plRedNAM_SPF (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soNF);
    } else
    {
        // Execute the niladic function returning an array
        lpplYYLstRht =
          plExecuteFn0 (lpplYYLstRht);

        // Check for error
        if (lpplYYLstRht EQ NULL)
            goto ERROR_EXIT;

        // Call common code
        return plRedNAM_SPA (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
    } // End IF
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedNAM_SPNF


//***************************************************************************
//  $plRedIO_SPNF
//
//  Reduce "IO SPNF"
//***************************************************************************

LPPL_YYSTYPE plRedIO_SPNF
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // Execute the niladic function returning an array
    lpplYYLstRht =
      plExecuteFn0 (lpplYYLstRht);

    // Check for error
    if (lpplYYLstRht EQ NULL)
        goto ERROR_EXIT;

    // Call common code
    return plRedIO_SPA (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
ERROR_EXIT:
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedIO_SPNF


//***************************************************************************
//  $plRedF_SPA
//
//  Reduce "F SPA" and "F ISPA"
//***************************************************************************

LPPL_YYSTYPE plRedF_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    Assert (lpplYYLstRht->lpplYYFcnCurry EQ NULL);

    // Unstrand the function if appropriate
    UnFcnStrand_EM (&lpplYYCurObj, NAMETYPE_FN12, FALSE);

    // The result is the SPA
    lpYYRes = lpplYYLstRht;

    // Copy to the function curry object
    lpYYRes->lpplYYFcnCurry = lpplYYCurObj;

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    return lpYYRes;
} // End plRedF_SPA


//***************************************************************************
//  $plRedIDX_IDX
//
//  Reduce "IDX IDX"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedIDX_IDX"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedIDX_IDX
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpplYYTmp = lpplYYCurObj;

    // Find an open lpplYYIdxCurry
    while (lpplYYTmp->lpplYYIdxCurry NE NULL)
        // Point to the next IdxCurry
        lpplYYTmp = lpplYYTmp->lpplYYIdxCurry;

    // Extend the chain of curried objects
    lpplYYTmp->lpplYYIdxCurry = lpplYYLstRht;

    // Change the tkSynObj
    lpplYYCurObj->tkToken.tkSynObj = soType;

    // Do *NOT* free as it is still curried
////YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    return lpplYYCurObj;
} // End plRedIDX_IDX
#undef  APPEND_NAME


//***************************************************************************
//  $plRedIDX_SPA
//
//  Reduce "IDX SPA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedIDX_SPA"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedIDX_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    Assert (lpplYYLstRht->lpplYYIdxCurry EQ NULL);

    // Copy to the left curry object
    lpplYYLstRht->lpplYYIdxCurry = lpplYYCurObj;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

////YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;  // Do *NOT* free as it is still curried

    return lpplYYLstRht;
} // End plRedIDX_SPA
#undef  APPEND_NAME


//***************************************************************************
//  $plRedA_SPA
//
//  Reduce "A SPA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedA_SPA"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedA_SPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // If it's a named var, ...
    if (lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_VARNAMED)
    {
        // Change the tkSynObj
        lpplYYCurObj->tkToken.tkSynObj = soNAM;

        // Call common code
        return plRedNAM_SPCom (lpplLocalVars, lpplYYCurObj, lpplYYLstRht, soType);
    } // End IF

    // YYFree the current object
                               YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    // Free and YYFree the last right object
    FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    return NULL;
} // End plRedA_SPA
#undef  APPEND_NAME


//***************************************************************************
//  $plRedNAM_ISPA
//
//  Reduce "NAM ISPA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedNAM_ISPA"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedNAM_ISPA
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    UBOOL bRet;                         // TRUE iff the result is valid

    Assert (lpplYYLstRht->lpplYYIdxCurry NE NULL);
    Assert (IsTknNamed (&lpplYYCurObj->tkToken));

    // If the IDX is repeated, ...
    if (lpplYYLstRht->lpplYYIdxCurry->lpplYYIdxCurry NE NULL)
        goto NONCE_EXIT;

    if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
        bRet = FALSE;
    else
    // If the left fcn is present (select spec as in NAM[A] F {is} A), ...
    if (lpplYYLstRht->lpplYYFcnCurry NE NULL)
        // Assign the value to the indexed name via the modify function
        bRet =
          ArrayIndexFcnSet_EM (&lpplYYCurObj->tkToken,
                               &lpplYYLstRht->lpplYYIdxCurry->tkToken,
                                lpplYYLstRht->lpplYYFcnCurry,
                               &lpplYYLstRht->tkToken);
    else
        // Assign the value to the indexed name
        bRet =
          ArrayIndexSet_EM    (&lpplYYCurObj->tkToken,
                               &lpplYYLstRht->lpplYYIdxCurry->tkToken,
                               &lpplYYLstRht->tkToken);
    // YYFree the current & curried objects
                                               YYFree (lpplYYCurObj);                 lpplYYCurObj = NULL; // curSynObj = soNONE;
    FreeResult (lpplYYLstRht->lpplYYIdxCurry); YYFree (lpplYYLstRht->lpplYYIdxCurry); lpplYYLstRht->lpplYYIdxCurry = NULL;

    if (!bRet)
    {
        FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;
    } else
    {
        // Change the tkSynObj
        lpplYYLstRht->tkToken.tkSynObj = soType;

        // Mark as already displayed
        lpplYYLstRht->tkToken.tkFlags.NoDisplay = TRUE;
    } // End IF

    return lpplYYLstRht;

NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpplYYCurObj->tkToken);
    // We'll free this object because of the NULL return,
    //   so we increment the RefCnt as this is a named object
    DbgIncrRefCntTkn (&lpplYYCurObj->tkToken);

    return NULL;
} // End plRedNAM_ISPA
#undef  APPEND_NAME


//***************************************************************************
//  $plRedNAM_SPCom
//
//  Reduce "NAM SPA"  & "NAM SPF", etc.
//***************************************************************************

LPPL_YYSTYPE plRedNAM_SPCom
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    // If the last right object is a var, ...
    if (IsTknTypeVar (lpplYYLstRht->tkToken.tkFlags.TknType))
    {
        // If the last right object is in the process of stranding, ...
        if (lpplYYLstRht->YYStranding)
        {
            // Turn this strand into a var
            lpYYRes =
              MakeVarStrand_EM_YY (lpplYYLstRht);
            // YYFree the last right object
            YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

            // If not defined, ...
            if (lpYYRes EQ NULL)
                goto ERROR_EXIT;

            // Copy to the last right object
            lpplYYLstRht = lpYYRes; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
            lpYYRes = NULL;
        } else
            // Unstrand the last right object if necessary
            UnVarStrand (lpplYYLstRht);
    } else
    // If the last right object is a function/operator, ...
    if (IsTknTypeFcnOpr (lpplYYLstRht->tkToken.tkFlags.TknType))
        // Unstrand the function if appropriate
        UnFcnStrand_EM (&lpplYYLstRht, TranslateSOTypeToNameType (lpplYYLstRht->tkToken.tkSynObj), TRUE);

    // If the left fcn is present (select spec as in NAM F {is} A), ...
    if (lpplYYLstRht->lpplYYFcnCurry NE NULL)
    {
        Assert (!lpplYYCurObj->YYStranding && !lpplYYLstRht->lpplYYFcnCurry->YYStranding);

        if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
            lpYYRes = NULL;
        else
            lpYYRes =
              ExecFunc_EM_YY (&lpplYYCurObj->tkToken, lpplYYLstRht->lpplYYFcnCurry, &lpplYYLstRht->tkToken);
        // Free the function (including YYFree)
        FreeResult (lpplYYLstRht->lpplYYFcnCurry); YYFree (lpplYYLstRht->lpplYYFcnCurry); lpplYYLstRht->lpplYYFcnCurry = NULL;

        // Free (unnamed) and YYFree the last right object
        FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

        // If not defined, ...
        if (lpYYRes EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYRes; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYRes = NULL;
    } // End IF

    // If the <lpplYYLstRht> is an intermediate list, ...
    if (lpplYYLstRht->tkToken.tkFlags.TknType EQ TKT_LISTINT)
    {
        LPPL_YYSTYPE lpYYVar1,
                     lpYYVar2;

        // This case occurs with embedded assignment within an bracket
        //    list such as z[F z{is}...].

        // Pop the most recent item from the list
        //   returning the item and shortening the list in <lpplYYLstRht>
        lpYYVar1 =
          PopList_YY (lpplYYLstRht);

        // Assign this token to this name
        // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
        if (!AssignName_EM (&lpplYYCurObj->tkToken, &lpYYVar1->tkToken))
            goto ERROR_EXIT;

        // Push the var back onto the list
        lpYYVar2 =
          PushList_YY (lpplYYLstRht, lpYYVar1, lpplYYLstRht);

        YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

        // Save back into the last right object
        lpplYYLstRht = lpYYVar2;

        // YYFree the temp
        YYFree (lpYYVar1); lpYYVar1 = NULL;
    } else
    // Assign this token to this name
    // Note that <AssignName_EM> sets the <NoDisplay> flag in the source token
    if (!AssignName_EM (&lpplYYCurObj->tkToken, &lpplYYLstRht->tkToken))
        goto ERROR_EXIT;
#ifdef DEBUG_TRACE
    dprintfWL0 (L"AssignName_EM %s = %p",
                lpplYYCurObj->tkToken.tkData.tkSym->stHshEntry->lpwCharName,
                lpplYYCurObj->tkToken.tkData.tkSym->stData.stGlbData);
#endif
    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    // Mark as assigned
    lpplYYLstRht->tkToken.tkFlags.NoDisplay = TRUE;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpplYYLstRht;

ERROR_EXIT:
    // YYFree the current & last right objects
    if (lpplYYCurObj NE NULL)
    {
        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;
    } // End IF

    if (lpplYYLstRht NE NULL)
    {
        // Free (unnamed) and YYFree the last right object
        FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;  // lstSynObj = soNONE;
    } // End IF

    return NULL;
} // End plRedNAM_SPCom


//***************************************************************************
//  $plRedLP_Com
//
//  Common code for "LP FR" & "LP MR" & "LP DR" & "LP HR" & "LP FFR" & "LP AFR"
//***************************************************************************

LPPL_YYSTYPE plRedLP_Com
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Change the tkSynObj
    lpplYYLstRht->tkToken.tkSynObj = soType;

    return lpplYYLstRht;
} // End plRedLP_Com


//***************************************************************************
//  $plRedAFOG_A
//
//  Reduce "AFOG A"  &  "AFOG SA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedAFOG_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedAFOG_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar;               // Ptr to a temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

    // Check the guard value
    if (!AfoGuard (lpplLocalVars, &lpplYYLstRht->tkToken))
        goto ERROR_EXIT;

    // Make a NoValue entry as the result
    lpYYRes = MakeNoValue_YY (&lpplYYCurObj->tkToken);

    // Change the tkSynObj
    lpYYRes->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpplLocalVars->ExitType = EXITTYPE_ERROR;
NORMAL_EXIT:
    // Free and YYFree the last right object
    FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return lpYYRes;
} // End plRedAFOG_A
#undef  APPEND_NAME


//***************************************************************************
//  $plRedAFOR_A
//
//  Reduce "AFOR A"  &  "AFOR SA"
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- plRedAFOR_A"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE plRedAFOR_A
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj,        // Ptr to current PL_YYSTYPE
     LPPL_YYSTYPE  lpplYYLstRht,        // ...    last right ...
     SO_ENUM       soType)              // Next SO_ENUM value

{
    LPPL_YYSTYPE lpYYVar;               // Ptr to a temp

    // If the last right object is in the process of stranding, ...
    if (lpplYYLstRht->YYStranding)
    {
        // Turn this strand into a var
        lpYYVar =
          MakeVarStrand_EM_YY (lpplYYLstRht);
        // YYFree the last right object
        YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

        // If not defined, ...
        if (lpYYVar EQ NULL)
            goto ERROR_EXIT;

        // Copy to the last right object
        lpplYYLstRht = lpYYVar; // lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
        lpYYVar = NULL;
    } else
        // Unstrand the last right object if necessary
        UnVarStrand (lpplYYLstRht);

    // Check the return value
    AfoReturn (lpplLocalVars, lpplYYLstRht);

    // Tell the caller to stop executing this line
    lpplLocalVars->bStopExec = TRUE;

////// Change the tkSynObj
////lpYYRes->tkToken.tkSynObj = soType;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as in error
    lpplLocalVars->ExitType = EXITTYPE_ERROR;
NORMAL_EXIT:
    // Free (unnamed) and YYFree the last right object
    FreeTempResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL;

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    return NULL;
} // End plRedAFOR_A
#undef  APPEND_NAME


//***************************************************************************
//  $ParseLine
//
//  Parse a line
//  The result of parsing the line is in the return value (plLocalVars.ExitType)
//    as well as in lpMemPTD->YYResExec if there is a value (EXITTYPE_DISPLAY
//    or EXITTYPE_NODISPLAY).
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseLine"
#else
#define APPEND_NAME
#endif

EXIT_TYPES ParseLine
    (HWND           hWndSM,                 // Session Manager window handle
     LPTOKEN_HEADER lpMemTknHdr,            // Ptr to tokenized line global memory header
     HGLOBAL        hGlbTxtLine,            // Text of tokenized line global memory handle
     LPWCHAR        lpwszLine,              // Ptr to the line text (may be NULL)
     LPPERTABDATA   lpMemPTD,               // Ptr to PerTabData global memory
     UINT           uLineNum,               // Function line #
     UINT           uTknNum,                // Starting token # in the above function line
     UBOOL          bTraceLine,             // TRUE iff we're tracing this line
     HGLOBAL        hGlbDfnHdr,             // User-defined function/operator global memory handle (NULL = execute/immexec)
     LPTOKEN        lptkFunc,               // Ptr to function token used for AFO function name
     UBOOL          bActOnErrors,           // TRUE iff errors are acted upon
     UBOOL          bExec1Stmt,             // TRUE iff executing only one stmt
     UBOOL          bNoDepthCheck)          // TRUE iff we're to skip the depth check

{
    PLLOCALVARS   plLocalVars = {0};        // SyntaxAnalyze local vars
    LPPLLOCALVARS oldTlsPlLocalVars;        // Ptr to previous value of dwTlsPlLocalVars
    UINT          oldTlsType,               // Previous value of dwTlsType
                  uRet;                     // The result from pl_yyparse
    UBOOL         bPLBracket = FALSE,       // TRUE iff we came from PARSELINE_MP_BRACKET
                  bBreakMessage = FALSE,    // TRUE iff we called BreakMessage
                  bRP_BRK;                  // TRUE iff we should recurse into () or []
    ERROR_CODES   uError = ERRORCODE_NONE;  // Error code
    UBOOL         bOldExecuting,            // Old value of bExecuting
                  bAssignName,              // TRUE iff the result is from an assigned name
                  bSink;                    // TRUE iff the result is from sink
    HWND          hWndEC;                   // Edit Ctrl window handle
    LPSIS_HEADER  lpSISCur,                 // Ptr to current SI Stack Header
                  lpSISPrv;                 // Ptr to previous ...
    LPDFN_HEADER  lpMemDfnHdr;              // Ptr to user-defined function/operator header ...
    LPPL_YYSTYPE *lpplOrgLftStk,            // Original ptr to left stack
                 *lpplOrgRhtStk,            // ...             right ...
                  lpYYRes = NULL;           // Ptr to the result
    int           lftBndStr,                // Left binding strength
                  rhtBndStr;                // Right ...
    SO_ENUM       lftSynObj,                // Left stack's current element Syntax Object value
                  curSynObj,                // Current ...
                  rhtSynObj,                // Right ...
                  lstSynObj,                // Last right ...
                  oldLstSynObj = soUNK;     // Old ...
    LPPL_REDSTR   lpplCurStr;               // Ptr to current plRedStr
    LPPL_YYSTYPE  lpplYYCurObj,             // Ptr to the current token object
                  lpplYYLstRht;             // The last POPRIGHT token in REDUCE

    static PL_YYSTYPE plYYEOS = {                   // tkToken
                                    {{TKT_EOS},     //   tkFlags
                                     soEOS,         //   tkSynObj
                                     {NULL},        //   tkData
                                     0},            //   tkCharIndex
                                    1,              // TknCount
                                    FALSE,          // YYInuse
                                    FALSE,          // YYIndirect
                                    TRUE ,          // YYPerm
                                };  // EOS PL_YYSTYPE

    static PL_YYSTYPE plYYSOS = {                   // tkToken
                                    {{TKT_SOS},     //   tkFlags
                                     soSOS,         //   tkSynObj
                                     {NULL},        //   tkData
                                     0},            //   tkCharIndex
                                    1,              // TknCount
                                    FALSE,          // YYInuse
                                    FALSE,          // YYIndirect
                                    TRUE ,          // YYPerm
                                };  // SOS PL_YYSTYPE
#ifdef DEBUG
    // Simplify certain names
    soNames[soSP] = WS_UTF16_LEFTARROW;
    soNames[soGO] = WS_UTF16_RIGHTARROW;

////gDbgLvl = 3;
#endif
    // Save the previous value of dwTlsType
    oldTlsType = PtrToUlong (TlsGetValue (dwTlsType));

    // Save the thread type ('PL')
    TlsSetValue (dwTlsType, TLSTYPE_PL);

    // Save the previous value of dwTlsPlLocalVars
    oldTlsPlLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Save ptr to SyntaxAnalyze local vars
    TlsSetValue (dwTlsPlLocalVars, (LPVOID) &plLocalVars);

    // Save ptr to PerTabData global memory
    TlsSetValue (dwTlsPerTabData, (LPVOID) lpMemPTD);

    // Get the Edit Ctrl window handle
    (HANDLE_PTR) hWndEC = GetWindowLongPtrW (hWndSM, GWLSF_HWNDEC);

    // Indicate that we're executing
    bOldExecuting = lpMemPTD->bExecuting; SetExecuting (lpMemPTD, TRUE);

    EnterCriticalSection (&CSOPL);

    // Increment the depth counter
    lpMemPTD->uExecDepth++;

    // Save the original left & right stack ptrs
    lpplOrgLftStk = lpMemPTD->lpplLftStk;
    lpplOrgRhtStk = lpMemPTD->lpplRhtStk;

    // Link this plLocalVars into the chain of such objects
    plLocalVars.lpPLPrev = lpMemPTD->lpPLCur;

    // If it's valid, ...
    if (lpMemPTD->lpSISCur NE NULL)
        // Save a ptr to this struc in the SIS header
        lpMemPTD->lpSISCur->lpplLocalVars = &plLocalVars;

    // If there's a previous ptr, transfer its Ctrl-Break flag
    if (lpMemPTD->lpPLCur NE NULL)
        plLocalVars.bCtrlBreak = lpMemPTD->lpPLCur->bCtrlBreak;

    // Save as new current ptr
    lpMemPTD->lpPLCur = &plLocalVars;

    LeaveCriticalSection (&CSOPL);

    // Initialize the error & return codes
    uRet   = 0;
    uError = ERRORCODE_NONE;

    // If we don't have a valid ptr, ...
    if (lpMemTknHdr EQ NULL)
    {
        plLocalVars.ExitType = EXITTYPE_ERROR;

        goto NORMAL_EXIT;
    } // End IF
#ifdef DEBUG
    // Display the tokens so far
    DisplayTokens (lpMemTknHdr);
#endif

    // If there's a UDFO global memory handle, ...
    if (hGlbDfnHdr NE NULL)
    {
        // Lock the memory to get a ptr to it
        lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

        // Save value in LocalVars
        plLocalVars.bAFO          = lpMemDfnHdr->bAFO;
        plLocalVars.bAfoCtrlStruc = lpMemDfnHdr->bAfoCtrlStruc;
        plLocalVars.bMFO          = lpMemDfnHdr->bMFO;

        // We no longer need this ptr
        MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
    } // End IF

    // Save values in the LocalVars
    plLocalVars.lpMemPTD       = lpMemPTD;
    plLocalVars.hWndSM         = hWndSM;
    plLocalVars.hGlbTxtLine    = hGlbTxtLine;
    plLocalVars.lpMemTknHdr    = lpMemTknHdr;
    plLocalVars.lpwszLine      = lpwszLine;
    plLocalVars.ExitType       = EXITTYPE_NONE;
    plLocalVars.uLineNum       = uLineNum;
    plLocalVars.hGlbDfnHdr     = hGlbDfnHdr;
    plLocalVars.bExec1Stmt     = bExec1Stmt;
    plLocalVars.bTraceLine     = bTraceLine;
////plLocalVars.bTraceFree     = FALSE;         // Already zero from = {0}
////plLocalVars.uStmtNum       = 0;             // ...

    // Get # tokens in the line
    plLocalVars.uTokenCnt = plLocalVars.lpMemTknHdr->TokenCnt;

    // If the starting token # is outside the token count, ...
    if (uTknNum >= plLocalVars.uTokenCnt)
    {
        // Set the exit type to exit normally
        plLocalVars.ExitType = EXITTYPE_GOTO_ZILDE;

        goto NORMAL_EXIT;
    } // End IF

    // Skip over TOKEN_HEADER
    plLocalVars.lptkStart = TokenBaseToStart (plLocalVars.lpMemTknHdr);
    plLocalVars.lptkEnd   = &plLocalVars.lptkStart[plLocalVars.uTokenCnt];

    // Save a ptr to the EOS/EOL token
    plLocalVars.lptkEOS = plLocalVars.lptkStart;

    Assert (plLocalVars.lptkStart->tkFlags.TknType EQ TKT_EOL
         || plLocalVars.lptkStart->tkFlags.TknType EQ TKT_EOS);

    // If we're not starting at the first token, ...
    if (uTknNum NE 0)
    {
        // Skip to the starting token
        plLocalVars.lptkNext  = &plLocalVars.lptkStart[uTknNum];

        // If this token is an EOS/EOL, skip to the end of the stmt
        //   and start executing there
        if (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL)
            plLocalVars.lptkNext = &plLocalVars.lptkNext[plLocalVars.lptkNext->tkData.tkIndex - 1];
    } else
        // Skip to end of 1st stmt
        plLocalVars.lptkNext  = &plLocalVars.lptkStart[plLocalVars.lptkStart->tkData.tkIndex];

    // Start off with no error
    plLocalVars.tkErrorCharIndex = NEG1U;

    // Initialize the strand starts
    if (oldTlsPlLocalVars NE NULL)
    {
        plLocalVars.lpYYStrArrStart[STRAND_VAR] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_VAR];
        plLocalVars.lpYYStrArrStart[STRAND_FCN] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_FCN];
        plLocalVars.lpYYStrArrStart[STRAND_LST] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_LST];
        plLocalVars.lpYYStrArrStart[STRAND_NAM] = oldTlsPlLocalVars->lpYYStrArrNext[STRAND_NAM];
    } else
    {
        plLocalVars.lpYYStrArrStart[STRAND_VAR] = lpMemPTD->lpStrand[STRAND_VAR];
        plLocalVars.lpYYStrArrStart[STRAND_FCN] = lpMemPTD->lpStrand[STRAND_FCN];
        plLocalVars.lpYYStrArrStart[STRAND_LST] = lpMemPTD->lpStrand[STRAND_LST];
        plLocalVars.lpYYStrArrStart[STRAND_NAM] = lpMemPTD->lpStrand[STRAND_NAM];
    } // End IF/ELSE

    // Initialize the base & next strand ptrs
    plLocalVars.lpYYStrArrStart[STRAND_VAR]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_VAR]                 =
    plLocalVars.lpYYStrArrNext [STRAND_VAR]                 = plLocalVars.lpYYStrArrStart[STRAND_VAR];
    plLocalVars.lpYYStrArrStart[STRAND_FCN]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_FCN]                 =
    plLocalVars.lpYYStrArrNext [STRAND_FCN]                 = plLocalVars.lpYYStrArrStart[STRAND_FCN];
    plLocalVars.lpYYStrArrStart[STRAND_LST]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_LST]                 =
    plLocalVars.lpYYStrArrNext [STRAND_LST]                 = plLocalVars.lpYYStrArrStart[STRAND_LST];
    plLocalVars.lpYYStrArrStart[STRAND_NAM]->lpYYStrandBase =
    plLocalVars.lpYYStrArrBase [STRAND_NAM]                 =
    plLocalVars.lpYYStrArrNext [STRAND_NAM]                 = plLocalVars.lpYYStrArrStart[STRAND_NAM];

    __try
    {
        __try
        {
            //***************************************************************************
            //  Start of 2by2 code
            //***************************************************************************

            Assert (plLocalVars.lptkStart[plLocalVars.uTokenCnt - 1].tkFlags.TknType EQ TKT_SOS);

            // Initialize the left & right stacks with an EOS & SOS
            *lpMemPTD->lpplLftStk++ = &plYYSOS;
            *lpMemPTD->lpplRhtStk++ = &plYYEOS;

            //***************************************************************************
            //  lpplLftStk and lpplRhtStk always point to the next available slot
            //***************************************************************************

            // Initialize the lpplYYLstRht var
            lpplYYLstRht = &plYYEOS; lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));

            // Parse the line, check for errors
            //   0 = success
            //   1 = YYABORT or APL error
            //   2 = memory exhausted
            // uRet = pl_yyparse (&plLocalVars);
PARSELINE_START:
            // Get the current object
            lpplYYCurObj = POPLEFT; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
#ifdef DEBUG_START
            {
                LPMEMTXT_UNION lpMemTxtLine;    // Ptr to header/line text global memory

                if (hGlbTxtLine NE NULL)
                    lpMemTxtLine = MyGlobalLockTxt (hGlbTxtLine);
                else
                    // Make it look like LPMEMTXT_UNION as all we need to do
                    //   is reference ->C which is two WCHARs in
                    lpMemTxtLine = (LPMEMTXT_UNION) (lpwszLine - 2);
                dprintfWL0 (L"Starting line(%d/%d):  %s",
                            uLineNum,
                            uTknNum,
                           &lpMemTxtLine->C);
                TRACE (L"Starting:", L"", CURSYNOBJ, RHTSYNOBJ);

                if (hGlbTxtLine NE NULL)
                {
                    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
                } // End IF
            }
#endif
            // If this is an EOS, ...
            if (CURSYNOBJ EQ soEOS)
            {
                // If the current token is a label separator, ...
                if (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_LABELSEP)
                {
                    // YYFree the current object
                    YYFree (lpplYYCurObj); // lpplYYCurObj = NULL; curSynObj = soNONE;

                    // Get the current object
                    lpplYYCurObj = POPLEFT; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                } else
                {
                    // Make a NoValue entry as the result
                    lpYYRes = MakeNoValue_YY (&lpplYYCurObj->tkToken);

                    // Change the tkSynObj
                    lpYYRes->tkToken.tkSynObj = soEOS;

                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = soEOS; Assert (IsValidSO (curSynObj));
                    lpYYRes = NULL;
                } // End IF/ELSE

                goto PARSELINE_DONE;
            } else
            // If this is a SYNTAX ERROR, ...
            if (CURSYNOBJ EQ soSYNR)
                goto PARSELINE_SYNTERR;

            goto PARSELINE_SCAN;

PARSELINE_SHIFT:
            PUSHRIGHT (lpplYYCurObj); lpplYYCurObj = POPLEFT; // curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));

            TRACE (L"Shifted: ", L"", CURSYNOBJ, RHTSYNOBJ);
PARSELINE_SCAN:
            // Get the left, current, and right SynObj values
            lftSynObj = LFTSYNOBJ; Assert (IsValidSO (lftSynObj));
            curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
            rhtSynObj = RHTSYNOBJ; Assert (IsValidSO (rhtSynObj));

            // Convert "HY HY" to "HY MOP"
            if (lftSynObj EQ soHY
             && curSynObj EQ soHY)
            {
                // Convert the righthand HY to a primitive operator
                ConvertHY2PFO (lpplYYCurObj,
                               TKT_OP1IMMED,
                               IMMTYPE_PRIMOP1,
                               soMOP);
                // Reset the value as it has changed
                curSynObj = CURSYNOBJ;
            } // End IF

            TRACE (L"Scanning:", L"", CURSYNOBJ, RHTSYNOBJ);
PARSELINE_SCAN1:
            // Get the left & right binding strengths
            lftBndStr = LBIND (lftSynObj, curSynObj);   // Ensure that lftSynObj & curSynObj are set
            rhtBndStr = RBIND (curSynObj, rhtSynObj);   // ...         rhtSynObj & ...

            TRACE (L"Binding: ", L"", CURSYNOBJ, RHTSYNOBJ);

            // If lftSynObj is soNAM and
            //    curSynObj is soIDX and
            //    rhtSynObj is soF   and
            //    (rh2SynObj is soMOP and rh3SynObj is soSPA
            //     or rh2SynObj is soSPA), ...
            if (lftSynObj EQ soNAM
             && curSynObj EQ soIDX
             && rhtSynObj EQ soF
             && ((RH2SYNOBJ EQ soMOP && RH3SYNOBJ EQ soSPA)
              || RH2SYNOBJ EQ soSPA))
            {
                // This case handles
                //     NAM IDX F MOP ... MOP SPA
                //   by transforming it into
                //     NAM NDX F MOP ... MOP SPA
                //   and left shifting it so that
                //     NDX F MOP
                //   is handled next and it is reduced to
                //     NDX F  by  F MOP => F
                //   because (IDX F) > (F MOP)
                //   and     (NDX F) < (F MOP)
                //   so F MOP is the next pair to be reduced.
                // This case also handles the case of NAM IDX F SPA
                //   same as the above but with SPA in place of MOP.
                lpplYYCurObj->tkToken.tkSynObj = soNDX;

                goto LEFTSHIFT;
            } else
            // If lftSynObj is neither soIDX nor soSRBK, ...
            if (lftSynObj NE soIDX          // LFTSYNOBJ NE IDX
             && lftSynObj NE soSRBK         // LFTSYNOBJ NE SRBK
             && lftSynObj NE soSYNR         // LFTSYNOBJ NE SYNR
             && lftSynObj NE soVALR         // LFTSYNOBJ NE VALR
             && lftBndStr < rhtBndStr)      // LBIND < RBIND
                goto PARSELINE_REDUCE;
            else
            if (lftBndStr > rhtBndStr)      // LBIND > RBIND
                goto PARSELINE_SHIFT;
            else                            // LBIND EQ RBIND
            // If the left (or right) binding strength is non-zero, ...
            if (lftBndStr NE 0)
                goto PARSELINE_SHIFT;       // Not both zero

            // Fall through if both binding strengths are zero

            // MATCH_PAIR

            // Check for left & right parens
            if (lftSynObj EQ soLP && rhtSynObj EQ soRP)
                goto PARSELINE_MP_PAREN;

            // Check for left & right brackets
            if (lftSynObj EQ soLBK && rhtSynObj EQ soRBK)
                goto PARSELINE_MP_BRACKET;

            // Check for SYNTAX ERROR
            if (curSynObj EQ soSYNR)
                goto PARSELINE_SYNTERR;

            // Check for VALUE ERROR
            if (curSynObj EQ soVALR)
                goto PARSELINE_VALUEERR;

            // Check for left & right EOS
            if (lftSynObj EQ soEOS && rhtSynObj EQ soEOS)
                goto PARSELINE_MP_DONE;

            // Check for shift
            if (curSynObj NE soEOS)
                goto PARSELINE_SHIFT;

            goto PARSELINE_SYNTERR;

PARSELINE_MP_BRACKET:
            // Mark as coming from PARSELINE_MP_BRACKET
            bPLBracket = TRUE;
PARSELINE_MP_PAREN:
            // Remove the parens/brackets from the left & right stacks
            YYFree (POPRIGHT); YYFree (POPLEFT);

            TRACE (L"MatchPair", L"", CURSYNOBJ, RHTSYNOBJ);

            // If the current object is a Niladic Function, ...
            if (curSynObj EQ soNF)
            {
                // Execute the niladic function returning an array
                lpYYRes =
                  plExecuteFn0 (lpplYYCurObj);

                // Check for error
                if (lpYYRes EQ NULL)
                    goto PARSELINE_ERROR;

                // Copy to the current object
                lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                lpYYRes = NULL;
            } // End IF

            // If the current object is in the process of stranding, ...
            if (lpplYYCurObj->YYStranding)
            {
                // Turn this strand into a var
                lpYYRes =
                  MakeVarStrand_EM_YY (lpplYYCurObj);
                // YYFree the current object
                YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                // If not defined, ...
                if (lpYYRes EQ NULL)
                    goto PARSELINE_ERROR;

                // Copy to the current object
                lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                lpYYRes = NULL;
            } else
            // If the current object is a numeric/character strand/scalar, ...
            if (lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_NUMSTRAND
             || lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_CHRSTRAND
             || lpplYYCurObj->tkToken.tkFlags.TknType EQ TKT_NUMSCALAR)
                // Unstrand the current object
                UnVarStrand (lpplYYCurObj);

            // If we came from PARSELINE_MP_BRACKET, ...
            if (bPLBracket)
            {
                // Change the tkSynObj value to IDX
                lpplYYCurObj->tkToken.tkSynObj = soIDX;

                // Clear the flag
                bPLBracket = FALSE;
            } else
            {
                // Get the left, current, and right SynObj values
                lftSynObj = LFTSYNOBJ; Assert (IsValidSO (lftSynObj));
                curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                rhtSynObj = RHTSYNOBJ; Assert (IsValidSO (rhtSynObj));

                // If a shift left is required, ...
                while (TRUE)
                if (1 < RSTACKLEN
                 && curSynObj NE soDOP
                 && curSynObj NE soDOPN
                 && curSynObj NE soRP
                 && curSynObj NE soRBK
                 && curSynObj NE soARBK
                 && curSynObj NE soSRBK)
                {
                    SO_ENUM rh2SynObj;

                    rh2SynObj = RH2SYNOBJ; Assert (IsValidSO (rh2SynObj));

                    if (RBIND (curSynObj, rhtSynObj) >= RBIND (rhtSynObj, rh2SynObj))
                    {
                        if (rhtSynObj NE soRP
                         && rhtSynObj NE soRBK
                         && rhtSynObj NE soARBK
                         && rhtSynObj NE soSRBK)
                        {
                            if (LBIND (lftSynObj, curSynObj) <= RBIND (curSynObj, rhtSynObj))
                                break;
                        } else
                        {
                            if (LBIND (lftSynObj, curSynObj) <= RBIND (curSynObj, rhtSynObj)
                             && RBIND (curSynObj, rhtSynObj) <= RBIND (rhtSynObj, rh2SynObj))
                                break;
                        } // End IF/ELSE
                    } // End IF

                    PUSHLEFT (lpplYYCurObj); lftSynObj = LFTSYNOBJ; Assert (IsValidSO (lftSynObj));
                    lpplYYCurObj = POPRIGHT; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));

                    rhtSynObj = RHTSYNOBJ; Assert (IsValidSO (rhtSynObj));
                    rh2SynObj = RH2SYNOBJ;      // Assert (IsValidSO (rh2SynObj)); // This item might be invalid

                    TRACE (L"MatchPair", L"- LftShift", CURSYNOBJ, rhtSynObj);
                } else
                    break;
            } // End IF

            // Get the left, current, and right SynObj values
            lftSynObj = LFTSYNOBJ; Assert (IsValidSO (lftSynObj));
            curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
            rhtSynObj = RHTSYNOBJ; Assert (IsValidSO (rhtSynObj));

            goto PARSELINE_SCAN1;

PARSELINE_MP_DONE:
#ifdef DEBUG_TRACE
            dprintfWL0 (L"MatchPair Done:  curSynObj (%s)",
                        soNames[curSynObj]);
#endif
            // If the current object is a Niladic Function, ...
            if (curSynObj EQ soNF
             || curSynObj EQ soSPNF)
            {
                UBOOL NoDisplay;            // NoDisplay flag

                // Save the NoDisplay flag
                NoDisplay = lpplYYCurObj->tkToken.tkFlags.NoDisplay;

                // Unstrand the function if appropriate
                if (UnFcnStrand_EM (&lpplYYCurObj, NAMETYPE_FN0, FALSE))
                {
                    // Transfer the NoDisplay flag
                    lpplYYCurObj->tkToken.tkFlags.NoDisplay = NoDisplay;

                    // Set the tkSynObj
                    lpplYYCurObj->tkToken.tkSynObj = curSynObj;
                } // End IF

                // If the current object is not the result of an assignment, ...
                if (!NoDisplay)
                {
                    // Execute the niladic function returning an array
                    lpYYRes =
                      plExecuteFn0 (lpplYYCurObj);

                    // Check for error
                    if (lpYYRes EQ NULL)
                        goto PARSELINE_ERROR;

                    // If it's SPNF, ...
                    if (curSynObj EQ soSPNF)
                        lpYYRes->tkToken.tkFlags.NoDisplay = TRUE;

                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                    lpYYRes = NULL;
                } // End IF
            } else
            // If the current object is a CS0, ...
            if (curSynObj EQ soCS0)
            {
                // Execute the CS0 Function and replace the current object with the result
                lpYYRes =
                  ExecuteCS0 (&plLocalVars, lpplYYCurObj);

                // Check for error
                if (lpYYRes EQ NULL)
                {
                    // Check for stop execution of this line
                    if (plLocalVars.bStopExec)
                    {
                        // Set the curSynObj to something harmless
                        curSynObj = soEOS;

                        goto PARSELINE_DONE;
                    } else
                        goto PARSELINE_ERROR;
                } // End IF

                // If it's NoValue, ...

                // Copy to the current object
                lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                lpYYRes = NULL;
            } else
            // If the current object is a soGO, ...
            if (curSynObj EQ soGO)
                plLocalVars.ExitType = EXITTYPE_RESET_ONE_INIT;
            else
                // Unstrand the current object if necessary
                UnVarStrand (lpplYYCurObj);

            goto PARSELINE_DONE;

PARSELINE_REDUCE:
            // CUROBJ = REDUCE

            Assert (curSynObj EQ CURSYNOBJ); Assert (IsValidSO (curSynObj));

            // Recurse into () or [] ?
            bRP_BRK = (curSynObj EQ soRP
                    || curSynObj EQ soRBK
                    || curSynObj EQ soARBK
                    || curSynObj EQ soSRBK);
            // If we should recurse into () or [], ...
            if (bRP_BRK)
            {
                PUSHRIGHT (lpplYYCurObj); lpplYYCurObj = POPLEFT; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));

                TRACE (L"Recursing", L"", CURSYNOBJ, RHTSYNOBJ);
            } else
            {
                RESET_FLAGS  resetFlag;             // The current Reset Flag

                lpplYYLstRht = POPRIGHT; lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));

                // If we're recursing into "A HY", instead change the HY to F, push it back onto RSTACK, and shift left
                if (curSynObj EQ soA
                 && lstSynObj EQ soHY)
                {
                    // Convert the HY to a primitive function
                    ConvertHY2PFO (lpplYYLstRht,
                                   TKT_FCNIMMED,
                                   IMMTYPE_PRIMFCN,
                                   soF);
                    // Change the soType
                    lstSynObj = soF;

                    // Push it back onto RSTACK
                    PUSHRIGHT (lpplYYLstRht);

                    goto LEFTSHIFT;
                } // End IF

                // If the current object is INPOUT, and
                //   the last right object is neither SPA nor SPNF
                if (curSynObj EQ soIO
                 && lstSynObj NE soSPA
                 && lstSynObj NE soSPNF)
                {
                    lpYYRes =
                      WaitForInput (plLocalVars.hWndSM,
                                    lpplYYCurObj->tkToken.tkData.tkChar EQ UTF16_QUOTEQUAD,
                                   &lpplYYCurObj->tkToken);
                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

                    // Get the Reset Flag
                    resetFlag = plLocalVars.lpMemPTD->lpSISCur->ResetFlag;

                    // If we're resetting, ...
                    if (resetFlag NE RESETFLAG_NONE)
                    {
                        plLocalVars.ExitType = TranslateResetFlagToExitType (resetFlag);

                        goto PARSELINE_ERROR;
                    } // End IF

                    if (lpYYRes EQ NULL)            // If not defined, free args and YYERROR
                        goto PARSELINE_ERROR;

                    // Set the new tkSynObj
                    lpYYRes->tkToken.tkSynObj = soA;

                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                    lpYYRes = NULL;
                } // End IF

                // If the last right object is INPOUT, ...
                if (lstSynObj EQ soIO)
                {
                    lpYYRes =
                      WaitForInput (plLocalVars.hWndSM,
                                    lpplYYLstRht->tkToken.tkData.tkChar EQ UTF16_QUOTEQUAD,
                                   &lpplYYLstRht->tkToken);
                    // YYFree the last right object
                    YYFree (lpplYYLstRht); lpplYYLstRht = NULL; // lstSynObj = soNONE;

                    // Get the Reset Flag
                    resetFlag = plLocalVars.lpMemPTD->lpSISCur->ResetFlag;

                    // If we're resetting, ...
                    if (resetFlag NE RESETFLAG_NONE)
                    {
                        plLocalVars.ExitType = TranslateResetFlagToExitType (resetFlag);

                        goto PARSELINE_ERROR;
                    } // End IF

                    if (lpYYRes EQ NULL)            // If not defined, free args and YYERROR
                        goto PARSELINE_ERROR;

                    // Set the new tkSynObj
                    lpYYRes->tkToken.tkSynObj = soA;

                    // Copy to the current object
                    lpplYYLstRht = lpYYRes;  lstSynObj = LSTSYNOBJ; Assert (IsValidSO (lstSynObj));
                    lpYYRes = NULL;
                } // End IF

                // Point to the PL_REDSTR
                lpplCurStr = &plRedStr[curSynObj][lpplYYLstRht->tkToken.tkSynObj];
#ifdef DEBUG
                if (lpplCurStr->lpplRedFcn EQ NULL)
                {
                    WCHAR wszTemp[128];

                    MySprintfW (wszTemp,
                                sizeof (wszTemp),
                               L"ASSERTION ERROR in <ParseLine>#%d",
                                __LINE__);
                    MessageBoxW (NULL,
                                 wszTemp,
                                 lpwszAppName,
                                 MB_OK | MB_ICONWARNING | MB_APPLMODAL);
                    DbgBrk ();      // ***FINISHME*** -- Missing reduction in 2by2
                } // End IF

                {
                    WCHAR EVENT[64];

                    MySprintfW (EVENT,
                                sizeof (EVENT),
                               L"- \"%s %s\" " WS_UTF16_RIGHTARROW L" %s",
                                soNames[curSynObj],
                                soNames[lstSynObj],
                                soNames[lpplCurStr->soType]);
                    TRACE2 (L"Reducing:", EVENT, lpplCurStr->soType, lstSynObj);
                }

                // I know about these cases, so don't bother me with them
                if (!(lpplCurStr->lpplRedFcn EQ plRedPseudo
                  && (curSynObj EQ soMOP || curSynObj EQ soMOPN || curSynObj EQ soHY)
                  && (lstSynObj EQ soA   || lstSynObj EQ soMOP  || lstSynObj EQ soHY))
                   )
                    Assert (lpplCurStr->soType NE soUNK);
#endif
                // Save the value of lstSynObj so we may check it at PARSELINE_DONE
                oldLstSynObj = lstSynObj;

                // Reduce the stmt
                // *** N.B.  This call might change the values pointed to by
                //           lpplYYCurObj & lpplYYLstRht and it might invalidate
                //           the values in curSynObj & lstSynObj ***
                lpYYRes =
                  (*lpplCurStr->lpplRedFcn) (&plLocalVars,
                                              lpplYYCurObj,
                                              lpplYYLstRht,
                                              lpplCurStr->soType);
                // Check for YYFreed or returned as result current & last right objects
                if (!lpplYYCurObj->YYInuse || lpYYRes EQ lpplYYCurObj)
                {
                    // Zap it
                    lpplYYCurObj = NULL; curSynObj = soNONE;
                } // End IF

                if (!lpplYYLstRht->YYInuse || lpYYRes EQ lpplYYLstRht)
                {
                    // Zap it
                    lpplYYLstRht = NULL; lstSynObj = soNONE;
                } // End IF
#ifdef DEBUG
                // Ensure these get respecified
                curSynObj = lstSynObj = soNONE;
#endif
                // Check for stop execution of this line
                if (plLocalVars.bStopExec)
                {
                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes->tkToken))
                    {
                        // YYFree the result
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } // End IF

                    // Set the curSynObj to something harmless
                    curSynObj = soEOS;

                    goto PARSELINE_DONE;
                } // End IF

                // If we're restarting, ...
                if (plLocalVars.bRestart)
                {
                    plLocalVars.bRestart = FALSE;

                    // The current token is SOS

                    // Check for NoValue
                    if (IsTokenNoValue (&lpYYRes->tkToken))
                    {
                        // YYFree the result
                        YYFree (lpYYRes); lpYYRes = NULL;
                    } // End IF

                    goto PARSELINE_START;
                } // Ebd IF

                // Check for error
                if (lpYYRes EQ NULL)
                    goto PARSELINE_ERROR;

#ifdef DEBUG
                // In case the result is from a lower SI level (such as Execute)
                //   reset the SI level
                lpYYRes->SILevel = lpMemPTD->SILevel;
#endif

                // Check for ResetFlag
                if (lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE)
                    goto PARSELINE_ERROR;

                // Get the left, current, and right SynObj values
                lftSynObj = LFTSYNOBJ; Assert (IsValidSO (lftSynObj));
                if (lpplYYCurObj NE NULL)
                {
                    curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                } else
                    curSynObj = soNONE;
                rhtSynObj = RHTSYNOBJ; Assert (IsValidSO (rhtSynObj));

                // Check for NoValue
                if (IsTokenNoValue (&lpYYRes->tkToken)
                 && lftSynObj EQ soEOS)
                {
                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; lpplYYCurObj->tkToken.tkSynObj = curSynObj = soEOS; Assert (IsValidSO (curSynObj));
                    lpYYRes = NULL;
                } else
                {
                    Assert (IsValidSO (lpYYRes->tkToken.tkSynObj));

                    // Copy to the current object
                    lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                    lpYYRes = NULL;
                } // End IF/ELSE

                // If it's not a SYNTAX ERROR, ...
                if (IsValidSO (curSynObj))
                {
                    // If a shift left is required, ...
////////////////////if (2 < RSTACKLEN && LBIND (lftSynObj, curSynObj) >= RBIND (curSynObj, rhtSynObj))
                    while (TRUE)
                    if (1 < RSTACKLEN
                     && curSynObj NE soDOP
                     && curSynObj NE soDOPN
                     && curSynObj NE soRP
                     && curSynObj NE soRBK
                     && curSynObj NE soARBK
                     && curSynObj NE soSRBK)
                    {
                        SO_ENUM rh2SynObj;

                        rh2SynObj = RH2SYNOBJ; Assert (IsValidSO (rh2SynObj));

                        // Check for left shift
                        if (RBIND (curSynObj, rhtSynObj) >= RBIND (rhtSynObj, rh2SynObj))
                        {
                            if (rhtSynObj NE soRP
                             && rhtSynObj NE soRBK
                             && rhtSynObj NE soARBK
                             && rhtSynObj NE soSRBK)
                            {
                                if (LBIND (lftSynObj, curSynObj) <= RBIND (curSynObj, rhtSynObj))
                                    break;
                            } else
                            {
                                if (LBIND (lftSynObj, curSynObj) <= RBIND (curSynObj, rhtSynObj)
                                 && RBIND (curSynObj, rhtSynObj) <= RBIND (rhtSynObj, rh2SynObj))
                                    break;
                            } // End IF/ELSE
                        } // End IF
LEFTSHIFT:
                        PUSHLEFT (lpplYYCurObj); lftSynObj = LFTSYNOBJ; Assert (IsValidSO (lftSynObj));
                        lpplYYCurObj = POPRIGHT; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));

                        rhtSynObj = RHTSYNOBJ; Assert (IsValidSO (rhtSynObj));
                        rh2SynObj = RH2SYNOBJ;  // Assert (IsValidSO (rh2SynObj));  // This item might be invalid

                        TRACE (L"PostRed: ", L"- LftShift", CURSYNOBJ, rhtSynObj);
                    } else
                        break;
                } // End IF
            } // End IF/ELSE

            // Check for SYNTAX ERROR
            if (IsValidSO (curSynObj))
            {
                // Get the left, current, and right SynObj values
                lftSynObj = LFTSYNOBJ; Assert (IsValidSO (lftSynObj));
////////////////curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                rhtSynObj = RHTSYNOBJ; Assert (IsValidSO (rhtSynObj));

                goto PARSELINE_SCAN1;
            } else
                goto PARSELINE_SYNTERR;

PARSELINE_VALUEERR:
            ErrorMessageIndirectToken (ERRMSG_VALUE_ERROR APPEND_NAME,
                                      &lpplYYCurObj->tkToken);
            goto PARSELINE_ERROR;

PARSELINE_SYNTERR:
            ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                                      &lpplYYCurObj->tkToken);
            goto PARSELINE_ERROR;

PARSELINE_ERROR:
            // If lpYYRes is defined, ...
            if (lpYYRes NE NULL)
            {
                // YYFree the temp
                YYFree (lpYYRes); lpYYRes = NULL;
            } // End IF

            // Loop through the left stack freeing temps
            while (LSTACKLEN > 1)
            {
                LPPL_YYSTYPE lpYYRes;

                // Get the next item from the left stack
                lpYYRes = POPLEFT;

                // If it's a function/operator, ...
                if (IsTknFcnOpr (&lpYYRes->tkToken))
                {
                    // Unstrand the function if appropriate
                    UnFcnStrand_EM (&lpYYRes, NAMETYPE_FN12, FALSE);

                    // Free the function (including YYFree)
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
                } else
                // If it's a var, ...
                if (IsTknTypeVar (lpYYRes->tkToken.tkFlags.TknType))
                {
                    // Unstrand the temp if necessary
                    UnVarStrand (lpYYRes);

                    // Free & YYFree the temp
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
                } else
                // If it's not an EOS or SOS, ...
                if (lpYYRes->tkToken.tkSynObj NE soSOS
                 && lpYYRes->tkToken.tkSynObj NE soEOS)
                {
                    // YYFree the temp
                    YYFree (lpYYRes); lpYYRes = NULL;
                } // End IF/ELSE/...
            } // End WHILE

            // Loop through the right stack freeing temps
            while (RSTACKLEN > 1)
            {
                LPPL_YYSTYPE lpYYRes;

                // Get the next item from the right stack
                lpYYRes = POPRIGHT;

                // If it's a function/operator, ...
                if (IsTknFcnOpr (&lpYYRes->tkToken))
                {
                    // Unstrand the function if appropriate
                    UnFcnStrand_EM (&lpYYRes, NAMETYPE_FN12, FALSE);

                    // Free the function (including YYFree)
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
                } else
                // If it's a var, ...
                if (IsTknTypeVar (lpYYRes->tkToken.tkFlags.TknType))
                {
                    // Unstrand the temp if necessary
                    UnVarStrand (lpYYRes);

                    // Free & YYFree the temp
                    FreeResult (lpYYRes); YYFree (lpYYRes); lpYYRes = NULL;
                } else
                // If it's not an EOS or SOS, ...
                if (lpYYRes->tkToken.tkSynObj NE soSOS
                 && lpYYRes->tkToken.tkSynObj NE soEOS)
                {
                    // YYFree the temp
                    YYFree (lpYYRes); lpYYRes = NULL;
                } // End IF/ELSE/...
            } // End WHILE

            // If it's valid, ...
            if (lpplYYCurObj NE NULL)
            {
                // If it's inuse, ...
                if (lpplYYCurObj->YYInuse)
                {
                    // In rare cases, (e.g., "MOP A"), the curried right operand (soon to be freed)
                    //  is <lpplYYLstRht>.
                    if (lpplYYLstRht EQ lpplYYCurObj
                     || lpplYYLstRht EQ lpplYYCurObj->lpplYYOpRCurry)
                        // Zap it so we won't attempt to free it again just below
                        lpplYYLstRht = NULL;

                    // If the current object is a Fcn or Var, ...
                    if (IsTknTypeFcnOpr (lpplYYCurObj->tkToken.tkFlags.TknType)
                     || IsTknTypeVar    (lpplYYCurObj->tkToken.tkFlags.TknType))
                        // Free the object and its curries
                        FreeResult (lpplYYCurObj);
                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;
                } else
                {
                    // The following Assert is triggered on certain
                    //   SYNTAX ERROR conditions (e.g. {del}{is}2)
                    //   which can be ignored
////////////////////Assert (YYCheckInuse (lpplYYCurObj));
                } // End IF/ELSE
            } // End IF

            // If it's valid, ...
            if (lpplYYLstRht NE NULL)
            {
                // If it's inuse, ...
                if (lpplYYLstRht->YYInuse)
                {
                    // Free the object and its curries
                    FreeResult (lpplYYLstRht); YYFree (lpplYYLstRht); lpplYYLstRht = NULL; lstSynObj = soNONE;
                } else
                    Assert (YYCheckInuse (lpplYYLstRht));
            } // End IF

            // Check for ResetFlag
            if (lpMemPTD->lpSISCur NE NULL
             && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_NONE
             && lpMemPTD->lpSISCur->ResetFlag NE RESETFLAG_QUADERROR_EXEC)
            {
                // Set the exit type
                plLocalVars.ExitType = TranslateResetFlagToExitType (lpMemPTD->lpSISCur->ResetFlag);

                // Clear this flag so we don't execute []ELX
                bActOnErrors = FALSE;
            } else
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_ERROR;

            // Mark as in error
            uRet   = 1;
            uError = ERRORCODE_ELX;

            goto PARSELINE_END;

PARSELINE_DONE:
            // Increment the stmt #
            plLocalVars.uStmtNum++;

            // If the current object is defined, ...
            if (lpplYYCurObj NE NULL)
            // If the current object is NoValue and EOS, ...
            if (IsTokenNoValue (&lpplYYCurObj->tkToken)
             && curSynObj EQ soEOS)
                // YYFree the current object
                YYFree (lpplYYCurObj);

            Assert (lpplOrgLftStk EQ &lpMemPTD->lpplLftStk[-1]);
            Assert (lpplOrgRhtStk EQ &lpMemPTD->lpplRhtStk[-1]);

            // N.B.:  DO NOT RELOAD lstSynObj as we are relying on the old value
#ifdef DEBUG_TRACE
            dprintfWL0 (L"Stmt Done:  curSynObj (%s), oldLstSynObj (%s)",
                        soNames[curSynObj],
                        soNames[oldLstSynObj]);
#endif
            // If we're restarting, ...
            if (plLocalVars.bRestart)
            {
                plLocalVars.bRestart = FALSE;

                goto PARSELINE_START;
            } // End IF

            // Set flag for assigned name
            bAssignName = 0
             || (curSynObj EQ soA    && oldLstSynObj EQ soSPA )
             || (curSynObj EQ soSA   && oldLstSynObj EQ soSPA )
             || (curSynObj EQ soF    && oldLstSynObj EQ soSPF )
             || (curSynObj EQ soNF   && oldLstSynObj EQ soSPNF)
             || (curSynObj EQ soMOP  && oldLstSynObj EQ soSPM )
             || (curSynObj EQ soMOPN && oldLstSynObj EQ soSPMN)
             || (curSynObj EQ soDOP  && oldLstSynObj EQ soSPD )
             || (curSynObj EQ soDOPN && oldLstSynObj EQ soSPDN)
             || (curSynObj EQ soHY   && oldLstSynObj EQ soSPHY);

            // Set flag for sink
            bSink = !bAssignName
                 && (curSynObj EQ soSPA)
                 && (lpplYYCurObj->lpplYYFcnCurry EQ NULL);

            // Check for SYNTAX ERROR
            if ( curSynObj EQ soA
             ||  curSynObj EQ soSA
             ||  curSynObj EQ soNF
             ||  curSynObj EQ soEOS
             ||  curSynObj EQ soGO
             ||  curSynObj EQ soCS0
             ||  curSynObj EQ soCS1
             ||  curSynObj EQ soAFOG
             ||  curSynObj EQ soAFOR
/////////////|| (curSynObj EQ soA    && lstSynObj EQ soSPA )    // Already handled via curSynObj EQ soA
/////////////|| (curSynObj EQ soSA   && lstSynObj EQ soSPA )    // Already handled via curSynObj EQ soSA
             ||  bSink
             ||  bAssignName)
            {
                UBOOL bEOL;             // TRUE iff we're at the EOL (or equivalent)

                // If the current object is a var, ...
                if (lpplYYCurObj NE NULL
                 && IsTknTypeVar (lpplYYCurObj->tkToken.tkFlags.TknType))
                {
                    // If the var is in the process of stranding, ...
                    if (lpplYYCurObj->YYStranding)
                    {
                        // Turn this strand into a var
                        lpYYRes =
                          MakeVarStrand_EM_YY (lpplYYCurObj);
                        // YYFree the current object
                        YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                        // If not defined, ...
                        if (lpYYRes EQ NULL)
                            goto PARSELINE_ERROR;

                        // Change the tkSynObj
                        lpYYRes->tkToken.tkSynObj = soA;

                        // Copy to the current object
                        lpplYYCurObj = lpYYRes; curSynObj = CURSYNOBJ; Assert (IsValidSO (curSynObj));
                        lpYYRes = NULL;
                    } else
                        // Unstrand the current object if necessary
                        UnVarStrand (lpplYYCurObj);
                } // End IF

                // Get ptr to current SI stack
                lpSISCur = lpMemPTD->lpSISCur;

                // If the function is being traced, ...
                if (bTraceLine)
                    TraceLine (lpMemPTD,            // Ptr to PerTabData global memory
                              &plLocalVars,         // Ptr to ParseLine local vars
                               lptkFunc,            // Ptr to function token used for AFO function name
                               lpplYYCurObj);       // Ptr to the current token object (may be NULL)
                // If the current token is an array or from sink, ...
                if (curSynObj EQ soA || bSink)
                {
                    HGLOBAL hGlbDfnHdr = NULL;      // UDFO/AFO global memory handle

                    // Do not display if caller is Execute or Quad
                    //   and the current statement is the last one on the line
                    if (IsLastStmt (&plLocalVars, lpplYYCurObj->tkToken.tkCharIndex)
                     && lpSISCur
                     && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                      || (lpSISCur->DfnType EQ DFNTYPE_IMM
                       && lpSISCur->lpSISPrv NE NULL
                       && (lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_EXEC
                        || lpSISCur->lpSISPrv->DfnType EQ DFNTYPE_QUAD))))
                        // Handle ArrExpr if caller is Execute or quad
                        ArrExprCheckCaller (&plLocalVars, lpSISCur, lpplYYCurObj, FALSE);
                    else
                    // If we're parsing an AFO, ...
                    if (hGlbDfnHdr = SISAfo (lpMemPTD))
                    {
                        // AFO display it
                        plLocalVars.bRet =
                          AfoDisplay_EM (&lpplYYCurObj->tkToken, FALSE, &plLocalVars, hGlbDfnHdr);
                    } else
                        plLocalVars.bRet =
                          ArrayDisplay_EM (&lpplYYCurObj->tkToken, TRUE, &plLocalVars.bCtrlBreak);

                    // Check for NoValue
                    if (IsTokenNoValue (&lpplYYCurObj->tkToken))
                        // Set the exit type
                        plLocalVars.ExitType = EXITTYPE_NOVALUE;
                    else
                    // Check for NoDisplay
                    if (lpplYYCurObj->tkToken.tkFlags.NoDisplay)
                        // Set the exit type
                        plLocalVars.ExitType = EXITTYPE_NODISPLAY;
                    else
                        // Set the exit type
                        plLocalVars.ExitType = EXITTYPE_DISPLAY;

                    // Free the current object
                    FreeResult (lpplYYCurObj);

                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;
                } else
                // If the current object is a function (not naked GOTO), ...
                if (lpplYYCurObj NE NULL
                 && !(curSynObj EQ soGO
                   && rhtSynObj EQ soEOS)
                 && IsTknTypeFcnOpr (lpplYYCurObj->tkToken.tkFlags.TknType))
                {
                    // Free the function (including YYFree)
                    FreeResult (lpplYYCurObj); YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;

                    // Set the exit type
                    plLocalVars.ExitType = EXITTYPE_NOVALUE;
                } else
                // If from an assigned name,
                //   or naked goto, ...
                if (bAssignName
                 || curSynObj EQ soGO)
                {
                    // YYFree the current object
                    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; curSynObj = soNONE;
#ifdef DEBUG
                } else
                {
                    // If we didn't come from AFOR, EOS, {goto}, CS0, or CS1, ...
                    if (curSynObj NE soAFOR
                     && curSynObj NE soEOS
                     && curSynObj NE soGO
                     && curSynObj NE soCS0
                     && curSynObj NE soCS1)
                        DbgStop ();     // Can this ever occur??
#endif
                } // End IF/ELSE/...

                // If the SIS level is valid, ...
                if (lpSISCur NE NULL)
                    // Copy the AFO value flag
                    lpSISCur->bAfoValue = plLocalVars.bAfoValue;

                // If there are no preceding tokens, ...
                if (plLocalVars.lptkEOS EQ plLocalVars.lptkNext)
                    // Skip to one past the EOS
                    plLocalVars.lptkNext++;

                // If the previous token is a NOP, ...
                if (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_NOP)
                    // Skip to the previous token (NOP) before which is (EOS/EOL)
                    plLocalVars.lptkNext--;

                // While the previous token is a Line Continuation, ...
                while (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_LINECONT)
                    // Skip to the previous token
                    plLocalVars.lptkNext--;

                // If the current token is a NOP, ...
                // This can occur in an AFO such as f{is}{leftbrace} CR/CR/LF ...
                if (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_NOP)
                    // Skip to the previous token
                    plLocalVars.lptkNext--;

                Assert (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_EOS
                     || plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_EOL
                     || plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_LABELSEP);

                // If we're at the end of the line, ...
                bEOL = (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_EOL
                     || plLocalVars.bStopExec
                     || plLocalVars.bExec1Stmt
                     || ( plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_LABELSEP
                      && &plLocalVars.lptkNext[-3] >= plLocalVars.lptkStart
                      &&  plLocalVars.lptkNext[-3].tkFlags.TknType EQ TKT_EOL));

                // If we're not at the end of the line, ...
                if (!bEOL && !plLocalVars.bAfoValue)
                {
                    // Skip to the previous token (EOS/EOL)
                    plLocalVars.lptkNext--;

                    Assert (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
                         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL
                         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_LABELSEP);
                    // If we're at label separator, ...
                    if (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_LABELSEP)
                    {
                        Assert (plLocalVars.lptkNext[-1].tkFlags.TknType EQ TKT_VARNAMED);

                        // Skip to the EOS/EOL token
                        plLocalVars.lptkNext -= 2;

                        Assert (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
                             || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL);
                    } // End IF

                    // Skip to end of the current stmt
                    plLocalVars.lptkNext = &plLocalVars.lptkNext[plLocalVars.lptkNext->tkData.tkIndex];

                    Assert (plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOS
                         || plLocalVars.lptkNext->tkFlags.TknType EQ TKT_EOL);

                    // Save a ptr to the EOS/EOL token
                    plLocalVars.lptkEOS =   plLocalVars.lptkNext;

                    // And again to the end of the next stmt
                    plLocalVars.lptkNext = &plLocalVars.lptkNext[plLocalVars.lptkNext->tkData.tkIndex];

                    goto PARSELINE_START;
                } // End IF

                goto PARSELINE_END;
            } else
            // Check for VALUE ERROR
            if (curSynObj EQ soVALR)
                goto PARSELINE_VALUEERR;
            else
                goto PARSELINE_SYNTERR;

            //***************************************************************************
            //  End of 2by2 code
            //***************************************************************************
        } __except (CheckVirtAlloc (GetExceptionInformation (),
                                    L"ParseLine"))
        {} // End __try/__except
    } __except (CheckException (GetExceptionInformation (), L"ParseLine"))
    {
        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_STACK_OVERFLOW:
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_STACK_FULL;

                break;

            case EXCEPTION_WS_FULL:
                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_ERROR;

                // Mark as in error
                uRet   = 1;
                uError = ERRORCODE_ELX;
                ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                                           NULL);
                break;

            default:
                // Display message for unhandled exception
                DisplayException ();

                // Mark as in error
                uRet   = 1;
                uError = ERRORCODE_ELX;

                goto NORMAL_EXIT;
        } // End SWITCH
    } // End __try/__except
PARSELINE_END:
    // In case we exited through an error, ...
    bPLBracket = FALSE;

    Assert (YYResIsEmpty ());

    if (!plLocalVars.bCtrlBreak)
    // Split cases based upon the exit type
    switch (plLocalVars.ExitType)
    {
        case EXITTYPE_QUADERROR_INIT:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_QUADERROR_INIT;

            break;

        case EXITTYPE_RESET_ALL:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ALL;

            break;

        case EXITTYPE_RESET_ONE:
            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE;

            break;

        case EXITTYPE_RESET_ONE_INIT:
            // Get a ptr to the current SIS header
            lpSISCur = lpMemPTD->lpSISCur;

            // Peel back to the first non-Exec layer
            while (lpSISCur->DfnType EQ DFNTYPE_EXEC)
               lpSISCur = lpSISCur->lpSISPrv;

            if (!lpSISCur->bItsEC)
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ONE_INIT;

            break;

        case EXITTYPE_QUADERROR_EXEC:
            // Set the exit type
            plLocalVars.ExitType = EXITTYPE_ERROR;

            // Set the reset flag
            lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_NONE;

            // Fall through to common code

        case EXITTYPE_ERROR:        // Mark user-defined function/operator as suspended
        case EXITTYPE_STACK_FULL:   // ...
        case EXITTYPE_STOP:         // ...
            // If it's STACK FULL, ...
            if (plLocalVars.ExitType EQ EXITTYPE_STACK_FULL)
////         || (plLocalVars.ExitType EQ EXITTYPE_LIMIT))   // ***FINISHME***
            {
                // Mark as in error
                uRet   = 1;
                uError = ERRORCODE_NONE;
                ErrorMessageIndirectToken (ERRMSG_STACK_FULL APPEND_NAME,
                                           NULL);
                // Set the reset flag
                lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_STOP;
            } // End IF

            // Get a ptr to the current SIS header
            lpSISCur = lpMemPTD->lpSISCur;

            // If it's an MFO
            //   or an AFO,
            // but not STACK FULL, ...
            if (lpSISCur NE NULL
             && (lpSISCur->bMFO
              || lpSISCur->bAFO)
             && plLocalVars.ExitType NE EXITTYPE_STACK_FULL)
            {
                // Don't suspend at this level

                // Set the exit type
                plLocalVars.ExitType = EXITTYPE_QUADERROR_INIT;

                // Set the reset flag
                lpSISCur->ResetFlag  = RESETFLAG_QUADERROR_INIT;

                // Reset the error codes so we don't execute []ELX at this level
                uRet   = 0;
                uError = ERRORCODE_NONE;

                break;
            } // End IF

            // If this level or an adjacent preceding level is from
            //   Execute, immediate execution mode, or an AFO,
            //   peel back to the preceding level
            while (lpSISCur NE NULL
                && (lpSISCur->DfnType EQ DFNTYPE_EXEC
                 || lpSISCur->DfnType EQ DFNTYPE_IMM
                 || lpSISCur->bAFO))
                lpSISCur = lpSISCur->lpSISPrv;

            // If this level is a user-defined function/operator,
            //   mark it as suspended
            if (lpSISCur NE NULL
             && (lpSISCur->DfnType EQ DFNTYPE_OP1
              || lpSISCur->DfnType EQ DFNTYPE_OP2
              || lpSISCur->DfnType EQ DFNTYPE_FCN))
                lpSISCur->bSuspended = TRUE;
            break;

        case EXITTYPE_DISPLAY:      // Nothing more to do with these types
        case EXITTYPE_NODISPLAY:    // ...
        case EXITTYPE_NOVALUE:      // ...
        case EXITTYPE_GOTO_ZILDE:   // ...
        case EXITTYPE_GOTO_LINE:    // ...
        case EXITTYPE_NONE:         // ...
            // Check on user-defined function/operator exit error
            if (CheckDfnExitError_EM (lpMemPTD))
            {
                // Mark as an APL error
                uRet                 = 1;
                plLocalVars.ExitType = EXITTYPE_ERROR;
            } // End IF

            break;

        case EXITTYPE_RETURNxLX:
            uRet = 0;

            break;

        defstop
            break;
    } // End IF/SWITCH

    // If Ctrl-Break was pressed, ...
    if (plLocalVars.bCtrlBreak)
    {
        // Mark as in error
        uError = ERRORCODE_ALX;

        // Reset the flag
        plLocalVars.bCtrlBreak = FALSE;

        // Peel back to first user-defined function/operator
        for (lpSISCur = lpMemPTD->lpSISCur;
             lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt;
             lpSISCur = lpSISCur->lpSISPrv)
        if (!lpSISCur->bMFO
         && !lpSISCur->bAFO
         && (lpSISCur->DfnType EQ DFNTYPE_OP1
          || lpSISCur->DfnType EQ DFNTYPE_OP2
          || lpSISCur->DfnType EQ DFNTYPE_FCN))
            break;

        // Signal an error
        BreakMessage (hWndSM,
                      (lpSISCur && lpSISCur NE lpMemPTD->lpSISNxt) ? lpSISCur
                                                                   : NULL);
        // Mark as current
        bBreakMessage = TRUE;
    } // End IF

    if (uRet EQ 0 || uError EQ ERRORCODE_ALX)
        goto NORMAL_EXIT;

    // If we're not resetting, ...
    if (lpMemPTD->lpSISCur NE NULL
     && lpMemPTD->lpSISCur->ResetFlag EQ RESETFLAG_NONE)
    {
        // If called from Immediate Execution/Execute, ...
        if (lpwszLine NE NULL)
            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                bBreakMessage ? NULL
                              : lpwszLine,                      // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
        else
        {
            LPMEMTXT_UNION lpMemTxtLine;    // Ptr to header/line text global memory

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLockTxt (hGlbTxtLine);

            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                bBreakMessage ? NULL
                              :&lpMemTxtLine->C,                // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE

        // If not already set, ...
        if (uError EQ ERRORCODE_NONE)
            // Mark as in error
            uError = ERRORCODE_ELX;
    } // End IF
NORMAL_EXIT:
    EnterCriticalSection (&CSOPL);

    // Unlink this plLocalVars from the chain of such objects
    lpMemPTD->lpPLCur = plLocalVars.lpPLPrev;

    // Transfer the Ctrl-Break flag in case it hasn't been acted upon
    if (lpMemPTD->lpPLCur NE NULL)
        lpMemPTD->lpPLCur->bCtrlBreak = plLocalVars.bCtrlBreak;

    LeaveCriticalSection (&CSOPL);

    // Restore the previous value of dwTlsType
    TlsSetValue (dwTlsType, ULongToPtr (oldTlsType));

    // If there's an error to be signalled, ...
    if (uError NE ERRORCODE_NONE
     && bActOnErrors
     && lpMemPTD->lpSISCur->lpSISErrCtrl EQ NULL)
    {
        EXIT_TYPES exitType;        // Return code from PrimFnMonUpTackJotCSPLParse

#ifdef DEBUG
        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                DbgMsgW2 (L"~~Start []ALX on");

                break;

            case ERRORCODE_ELX:
                DbgMsgW2 (L"~~Start []ELX on");

                break;

            case ERRORCODE_DM:
                DbgMsgW2 (L"~~Display []DM on");

                break;

            defstop
                break;
        } // End SWITCH
#endif
        // If there's an immediate text line, ...
        if (lpwszLine NE NULL)
        {
            DbgMsgW2 (lpwszLine);

            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                bBreakMessage ? NULL
                              : lpwszLine,                      // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
        } else
        {
            LPMEMTXT_UNION lpMemTxtLine;    // Ptr to header/line text global memory

            // Lock the memory to get a ptr to it
            lpMemTxtLine = MyGlobalLockTxt (hGlbTxtLine);

            // Display the function line
            DbgMsgW2 (&lpMemTxtLine->C);

            // Create []DM & []EM
            ErrorMessageDirect (lpMemPTD->lpwszErrorMessage,    // Ptr to error message text
                bBreakMessage ? NULL
                              :&lpMemTxtLine->C,                // Ptr to the line which generated the error
                                plLocalVars.tkErrorCharIndex);  // Position of caret (origin-0)
            // We no longer need this ptr
            MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
        } // End IF/ELSE

        // Set the text for the line
        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                lpwszLine = WS_UTF16_UPTACKJOT $QUAD_ALX;
                bNoDepthCheck = FALSE;

                break;

            case ERRORCODE_ELX:
                lpwszLine = WS_UTF16_UPTACKJOT $QUAD_ELX;
                bNoDepthCheck = FALSE;

                break;

            case ERRORCODE_DM:
                lpwszLine = $QUAD_DM;
                bNoDepthCheck = TRUE;

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
            case ERRORCODE_ELX:
                // Execute the statement
                exitType =
                  PrimFnMonUpTackJotCSPLParse (hWndEC,          // Edit Ctrl window handle
                                               lpMemPTD,        // Ptr to PerTabData global memory
                                               lpwszLine,       // Ptr to text of line to execute
                                               lstrlenW (lpwszLine), // Length of the line to execute
                                               TRUE,            // TRUE iff we should act on errors
                                               bNoDepthCheck,   // TRUE iff we're to skip the depth check
                                               lpMemPTD->lpSISCur->DfnType, // DfnType for FillSISNxt
                                               NULL);           // Ptr to function token
                break;

            case ERRORCODE_DM:
                // Display []DM
                DisplayGlbArr_EM (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DM]->stData.stGlbData,   // Global memory handle to display
                                  TRUE,                                                         // TRUE iff last line has CR
                                 &plLocalVars.bCtrlBreak,                                       // Ptr to Ctrl-Break flag
                                  NULL);                                                        // Ptr to function token
                // Return code as already displayed
                exitType = EXITTYPE_NODISPLAY;

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the error code
        switch (uError)
        {
            case ERRORCODE_ALX:
                // If we exited normally, ...
                if (exitType EQ EXITTYPE_DISPLAY
                 || exitType EQ EXITTYPE_NODISPLAY
                 || exitType EQ EXITTYPE_NOVALUE)
                {
                    // Stop execution so we can display the break message
                    plLocalVars.ExitType =
                    exitType             = EXITTYPE_STOP;

                    // Set the reset flag
                    lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_STOP;

                    // If the Execute/Quad result is present, clear it
                    if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType NE TKT_UNUSED)
                    {
                        // Free the result
                        FreeResult (&lpMemPTD->YYResExec);

                        // We no longer need these values
                        ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
                    } // End IF
                } // End IF

                break;

            case ERRORCODE_ELX:
            case ERRORCODE_DM:
                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the exit type
        switch (exitType)
        {
            case EXITTYPE_GOTO_LINE:
            case EXITTYPE_RESET_ALL:
            case EXITTYPE_RESET_ONE:
            case EXITTYPE_RESET_ONE_INIT:
            case EXITTYPE_QUADERROR_INIT:
                // Pass on to caller
                plLocalVars.ExitType = exitType;

                break;

            case EXITTYPE_NODISPLAY:    // Display the result (if any)
            case EXITTYPE_DISPLAY:      // ...
                // If the Execute/Quad result is present, display it
                if (lpMemPTD->YYResExec.tkToken.tkFlags.TknType NE TKT_UNUSED)
                {
                    // Display the array
                    ArrayDisplay_EM (&lpMemPTD->YYResExec.tkToken, TRUE, &plLocalVars.bCtrlBreak);

                    // Free the result
                    FreeResult (&lpMemPTD->YYResExec);

                    // We no longer need these values
                    ZeroMemory (&lpMemPTD->YYResExec, sizeof (lpMemPTD->YYResExec));
                } // End IF

                // Fall through to common code

            case EXITTYPE_ERROR:        // Display the prompt unless called by Quad or User fcn/opr
            case EXITTYPE_STACK_FULL:   // ...
            case EXITTYPE_STOP:         // ...
            case EXITTYPE_NOVALUE:      // ...
                // Get a ptr to the current SIS header
                lpSISPrv =
                lpSISCur = lpMemPTD->lpSISCur;

                // If this level is an AFO, ...
                while (lpSISPrv NE NULL
                    && lpSISPrv->bAFO)
                    // Peel back
                    lpSISPrv = lpSISPrv->lpSISPrv;

                // If this level is Immediate Execution Mode, ...
                while (lpSISPrv NE NULL
                    && lpSISPrv->DfnType EQ DFNTYPE_IMM)
                    // Peel back
                    lpSISPrv = lpSISPrv->lpSISPrv;

                // If this level is Execute, skip the prompt
                //   as Execute will handle that
                if (lpSISPrv NE NULL
                 && lpSISPrv->DfnType EQ DFNTYPE_EXEC)
                {
                    // If the result of {execute}[]xLX is a normal
                    //   result (EXITTYPE_NODISPLAY),
                    //   return  EXITTYPE_RETURNxLX so the caller
                    //   can avoid displaying a prompt until the
                    //   result of executing []xLX is handled
                    if (exitType EQ EXITTYPE_NODISPLAY)
                        exitType =  EXITTYPE_RETURNxLX;

                    // Pass on this exit type to the caller
                    plLocalVars.ExitType = exitType;
                } // End IF

                break;

            case EXITTYPE_GOTO_ZILDE:   // Nothing more to do with these
            case EXITTYPE_NONE:         // ...
                break;

            defstop
                break;
        } // End SWITCH
    } // End IF

    EnterCriticalSection (&CSOPL);

    // Restore the previous left & right stack ptrs
    lpMemPTD->lpplLftStk = lpplOrgLftStk;
    lpMemPTD->lpplRhtStk = lpplOrgRhtStk;

    // Restore the previous value of dwTlsPlLocalVars
    TlsSetValue (dwTlsPlLocalVars, oldTlsPlLocalVars);

    // Restore the previous executing state
    SetExecuting (lpMemPTD, bOldExecuting);

    // Decrement the depth counter
    lpMemPTD->uExecDepth--;

    LeaveCriticalSection (&CSOPL);

    return plLocalVars.ExitType;
} // End ParseLine
#undef  APPEND_NAME


//***************************************************************************
//  $pl_yylex
//
//  Lexical analyzer for Bison
//***************************************************************************

LPPL_YYSTYPE pl_yylex
    (LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    LPPL_YYSTYPE lpplYYLval;        // Ptr to YYRes

    Assert (!lpplLocalVars->bRestart);

    // Allocate a new YYRes
    lpplYYLval = YYAlloc ();

    // Call the lexical analyser
    pl_yylexCOM (lpplLocalVars, lpplYYLval, FALSE);

    return lpplYYLval;
} // End pl_yylex


//***************************************************************************
//  $GetLftSynObj
//
//  Return the left stack SynObj value without popping the token stack
//***************************************************************************

SO_ENUM GetLftSynObj
    (LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    SO_ENUM tkSynObj = soEOS;       // The result

    if (lpplLocalVars->lptkStart < lpplLocalVars->lptkNext)
    {
        PL_YYSTYPE plYYLval = {0};  // Temporary YYRes

        // Get the PL_YYSTYPE result
        pl_yylexCOM (lpplLocalVars, &plYYLval, TRUE);

        if (plYYLval.tkToken.tkSynObj NE soNONE)
        {
            // If the token is named,
            //   and not assigned into, ...
            if (IsTknNamed (&plYYLval.tkToken)
             && !plYYLval.tkToken.tkFlags.bAssignName
             &&  plYYLval.tkToken.tkSynObj NE soNAM
             &&  plYYLval.tkToken.tkSynObj NE soVALR
             &&  plYYLval.tkToken.tkSynObj NE soSYNR)
                // Get the matching tkSynObj
                tkSynObj = TranslateNameTypeToSOType (plYYLval.tkToken.tkData.tkSym->stFlags.stNameType);
            else
                // Get the result
                tkSynObj = plYYLval.tkToken.tkSynObj;
        } // End IF
    } // End IF

    return tkSynObj;
} // End GetLftSynObj


#if FALSE
//***************************************************************************
//  $GetLftToken
//
//  Return the left stack token without popping the token stack
//***************************************************************************

TOKEN GetLftToken
    (LPPLLOCALVARS lpplLocalVars)   // Ptr to local plLocalVars

{
    TOKEN tkLftStk = {0};           // The result

    if (lpplLocalVars->lptkStart < lpplLocalVars->lptkNext)
    {
        PL_YYSTYPE plYYLval = {0};  // Temporary YYRes

        // Get the PL_YYSTYPE result
        pl_yylexCOM (lpplLocalVars, &plYYLval, TRUE);

        CopyAll (&tkLftStk, &plYYLval.tkToken);
    } // End IF

    return tkLftStk;
} // End GetLftToken
#endif


//***************************************************************************
//  $pl_yylexCOM
//
//  Lexical analyzer for Bison
//  Common routine to <pl_yylex> and <GetLftSynObj>.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- pl_yylexCOM"
#else
#define APPEND_NAME
#endif

void pl_yylexCOM
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     LPPL_YYSTYPE  lpplYYLval,      // Ptr to YYRes
     UBOOL         bRestoreStk)     // TRUE iff we restore the token stack upon exit

{
    LPPERTABDATA      lpMemPTD;     // Ptr to PerTabData global memory
    LPSIS_HEADER      lpSISCur;     // Ptr to current SIS layer
    LPTOKEN           lptkOrigNext; // Ptr to original token stack
    LPDFN_HEADER      lpMemDfnHdr;  // Ptr to UDFO/AFO header
    LPFCNARRAY_HEADER lpMemHdrFcn;  // Ptr to FCNARRAY header
    HGLOBAL           hGlbFcn;      // Global Fcn/Opr handle

    // Save the original token stack ptr
    lptkOrigNext = lpplLocalVars->lptkNext;

    // Get ptr to PerTabData global memory
    lpMemPTD = lpplLocalVars->lpMemPTD; Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));
PL_YYLEX_START:
    Assert (IsValidPtr (lpplLocalVars->lptkNext, sizeof (lpplLocalVars->lptkNext[0])));
    Assert ((*(LPUINT) lpplLocalVars->lptkNext) NE 0xfeeefeee);

    // Because we're parsing the stmt from right to left
    lpplLocalVars->lptkNext--;

#if (defined (DEBUG)) && (defined (YYLEX_DEBUG))
    dprintfWL0 (L"==pl_yylex:  TknType = %S, CharIndex = %d",
                 GetTokenTypeName (lpplLocalVars->lptkNext->tkFlags.TknType),
                 lpplLocalVars->lptkNext->tkCharIndex);
#endif

    // Return the current token
    CopyAll (&lpplYYLval->tkToken, lpplLocalVars->lptkNext);

    // Initialize the rest of the fields
////lpplYYLval->TknCount        =               // Already zero from ZeroMemory
////lpplYYLval->YYInuse         =               // ...
////lpplYYLval->YYIndirect      = 0;            // ...
////lpplYYLval->YYStranding     = FALSE;        // ...
////lpplYYLval->lpYYFcnBase     = NULL;         // ...
////lpplYYLval->lpYYStrandBase  = NULL;         // ...

#ifdef DEBUG
    // If we should not restore the token stack ptr, ...
    if (!bRestoreStk)
        nop ();
#endif
    // Split cases based upon the token type
    switch (lpplLocalVars->lptkNext->tkFlags.TknType)
    {
        case TKT_VARIMMED:
        case TKT_VARARRAY:
            break;

        case TKT_INPOUT:
            break;

        case TKT_FCNNAMED:
PL_YYLEX_FCNNAMED:
            Assert (GetPtrTypeDir (lpplYYLval->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.FcnDir)  // Not a direct fcn/opr
            {
                // If we're not assigning into this name,
                if (!lpplYYLval->tkToken.tkFlags.bAssignName)
                {
                    // If we should not restore the token stack ptr,
                    if (!bRestoreStk)
                    {
                        // If it's an immediate, ...
                        if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // If the function is a hybrid, ...
                            if (IsTknHybrid (&lpplYYLval->tkToken))
                                // Mark is as a hybrid
                                lpplYYLval->tkToken.tkSynObj = soHY;
                            else
                                // Mark it as a Fcn so it can't be re-assigned.
                                lpplYYLval->tkToken.tkSynObj = soF;

                            // Convert this to an unnamed immediate fcn
                            lpplYYLval->tkToken.tkFlags.TknType   = TKT_FCNIMMED;
                            lpplYYLval->tkToken.tkFlags.ImmType   = lpplYYLval->tkToken.tkData.tkSym->stFlags.ImmType;
                            lpplYYLval->tkToken.tkData .tkChar    = lpplYYLval->tkToken.tkData.tkSym->stData.stChar;
                        } else
                        // If it's not a (named) direct function, ...
                        if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.FcnDir)
                        {
                            // Get the global memory handle
                            hGlbFcn = GetGlbHandle (&lpplYYLval->tkToken);

                            // Split cases based upon the array signature
                            switch (GetSignatureGlb_PTB (hGlbFcn))
                            {
                                case FCNARRAY_HEADER_SIGNATURE:
                                    // Lock the memory to get a ptr to it
                                    lpMemHdrFcn = MyGlobalLockFcn (hGlbFcn);

                                    // Convert this to an unnamed global fcn array
                                    lpplYYLval->tkToken.tkFlags.TknType   = TKT_FCNARRAY;
                                    lpplYYLval->tkToken.tkData .tkGlbData = hGlbFcn;
                                    lpplYYLval->tkToken.tkSynObj          = soF;        // ***FIXME*** -- can this ever be soNF ???

                                    // We no longer need this ptr
                                    MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL;

                                    // Increment the RefCnt of each item
                                    DbgIncrRefCntFcnArray (hGlbFcn);

                                    break;

                                case DFN_HEADER_SIGNATURE:
                                    // Lock the memory to get a ptr to it
                                    lpMemDfnHdr = MyGlobalLockDfn (hGlbFcn);

#if FALSE       // Enable this code when we have a TKT_FCNDFN (unnamed FCN DFN)
                                    // Convert this to an unnamed global fcn array
                                    lpplYYLval->tkToken.tkFlags.TknType   = lpMemDfnHdr->bAFO ? TKT_FCNAFO : TKT_FCNDFN;
                                    lpplYYLval->tkToken.tkData .tkGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;
////////////////////////////////////lpplYYLval->tkToken.tkSynObj          = ????        // See below
#endif
                                    // Set the tkSynObj value from the UDFO header
                                    lpplYYLval->tkToken.tkSynObj = TranslateDfnTypeToSOType (lpMemDfnHdr);

                                    // We no longer need this ptr
                                    MyGlobalUnlock (hGlbFcn); lpMemDfnHdr = NULL;

                                    // Increment the refcnt
                                    DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  any unassigned named non-UDFO

                                    break;

                                defstop
                                    break;
                            } // End SWITCH
                        } // End IF/ELSE/...

                        // If the SynObj is still the default, ...
                        if (lpplYYLval->tkToken.tkSynObj EQ soA)
                            // Mark it as a Fcn so it can't be re-assigned.
                            lpplYYLval->tkToken.tkSynObj = soF;
                    } // End IF
                } else
                    lpplYYLval->tkToken.tkSynObj = soNAM;
            } // End IF

            break;

        case TKT_VARNAMED:
        {
            STFLAGS stFlags;        // STE flags

            // Get the STE flags
            stFlags = lpplLocalVars->lptkNext->tkData.tkSym->stFlags;

            // Split cases based upon the NAMETYPE_xxx
            switch (stFlags.stNameType)
            {
                case NAMETYPE_UNK:
                    if (stFlags.DfnSysLabel)
                        // Mark it as an Array so it can't be re-assigned.
                        lpplYYLval->tkToken.tkSynObj = soA;
                    else
                    if (lpplLocalVars->lptkNext->tkFlags.bAssignName)
                        // Mark it as named as it'll be re-assigned shortly
                        lpplYYLval->tkToken.tkSynObj = soNAM;
                    else
                        // Mark is as a VALUE ERROR
                        lpplYYLval->tkToken.tkSynObj = soVALR;
                    break;

                case NAMETYPE_VAR:
                {
                    // If we should not restore the token stack ptr, ...
                    if (!bRestoreStk)
                    {
                        LPPL_YYSTYPE lpYYRht1 = NULL,
                                     lpYYRht2 = NULL,
                                     lpYYRht3 = NULL,
                                     lpYYRht4 = NULL;
                        LPTOKEN      lptkRht1 = NULL,
                                     lptkRht2 = NULL,
                                     lptkRht3 = NULL,
                                     lptkRht4 = NULL;
                        UBOOL        bAssignName;

                        // If it's a System Var,
                        //   but not a System Label, ...
                        if (lpplYYLval->tkToken.tkData.tkSym->stFlags.ObjName EQ OBJNAME_SYS
                         && !lpplYYLval->tkToken.tkData.tkSym->stFlags.DfnSysLabel)
                            // Copy the current value from the most local HTS
                            lpplYYLval->tkToken.tkData.tkSym =
                              lpMemPTD->lphtsPTD->lpSymQuad[lpplYYLval->tkToken.tkData.tkSym->stFlags.SysVarValid];

                        // Start with the AssignName value from Tokenize
                        bAssignName = lpplYYLval->tkToken.tkFlags.bAssignName;

                        // If not already marked so, ...
                        if (!bAssignName)
                        {
                            if (RSTACKLEN2 > 1)
                            {
                                lpYYRht1 = lpplLocalVars->lpMemPTD->lpplRhtStk[-1];
                                lptkRht1 = &lpYYRht1->tkToken;
                            } // End IF

                            if (RSTACKLEN2 > 2)
                            {
                                lpYYRht2 = lpplLocalVars->lpMemPTD->lpplRhtStk[-2];
                                lptkRht2 = &lpYYRht2->tkToken;
                            } // End IF

                            if (RSTACKLEN2 > 3)
                            {
                                lpYYRht3 = lpplLocalVars->lpMemPTD->lpplRhtStk[-3];
                                lptkRht3 = &lpYYRht3->tkToken;
                            } // End IF

                            if (RSTACKLEN2 > 4)
                            {
                                lpYYRht4 = lpplLocalVars->lpMemPTD->lpplRhtStk[-4];
                                lptkRht4 = &lpYYRht4->tkToken;
                            } // End IF

                            // Late catch of                                  RhtStk1 RhtStk2 RhtStk3 RhtStk4
                            //   #1          NAM          F        {is} A       SPA
                            //   #2          NAM [A]      F        {is} A      ISPA
                            //   #3          NAM [A]      F MOP    {is} A       IDX       F      MOP     SPA
                            //   #4a        (NAM ... NAM) F        {is} A       RP        F      SPA
                            //   #4b        (NAM ... NAM) F        {is} A       NR       SPA
                            //   #4c        (NAM ... NAM) F        {is} A      NNR       SPA
                            //   #5a        (NAM ... NAM) F MOP    {is} A       RP        F      MOP     SPA
                            //   #5b        (NAM ... NAM) F MOP    {is} A       NR        F      SPA
                            //   #5c        (NAM ... NAM) F MOP    {is} A      NNR        F      SPA
                            //   #6a        (NAM ... NAM) F MOP[A] {is} A       RP        F      MOP     ISPA
                            //   #6b        (NAM ... NAM) F MOP[A] {is} A       NR        F      ISPA
                            //   #6c        (NAM ... NAM) F MOP[A] {is} A      NNR        F      ISPA

                            // Check for #1 & #2
                            bAssignName |= ((lptkRht1 NE NULL)
                                         && ((lptkRht1->tkSynObj EQ soSPA) || (lptkRht1->tkSynObj EQ soISPA))
                                         && (lpYYRht1->lpplYYFcnCurry NE NULL))
                                          ;
                            // Check for #3
                            bAssignName |= ((lptkRht1 NE NULL)
                                         && (lptkRht2 NE NULL)
                                         && (lptkRht3 NE NULL)
                                         && (lptkRht4 NE NULL)
                                         && (lptkRht1->tkSynObj EQ soIDX)
                                         && (lptkRht2->tkSynObj EQ soF)
                                         && (lptkRht3->tkSynObj EQ soMOP)
                                         && (lptkRht4->tkSynObj EQ soSPA))
                                          ;
                            // Check for #4a
                            bAssignName |= ((lptkRht1 NE NULL)
                                         && (lptkRht2 NE NULL)
                                         && (lptkRht3 NE NULL)
                                         && (lptkRht1->tkSynObj EQ soRP)
                                         && (lptkRht2->tkSynObj EQ soF)
                                         && (lptkRht3->tkSynObj EQ soSPA))
                                          ;
                            // Check for #4b & #4c
                            bAssignName |= ((lptkRht1 NE NULL)
                                         && (lptkRht2 NE NULL)
                                         && ((lptkRht1->tkSynObj EQ soNR) || (lptkRht1->tkSynObj EQ soNNR))
                                         && (lptkRht2->tkSynObj EQ soSPA))
                                          ;
                            // Check for #5a & #6a
                            bAssignName |= ((lptkRht1 NE NULL)
                                         && (lptkRht2 NE NULL)
                                         && (lptkRht3 NE NULL)
                                         && (lptkRht4 NE NULL)
                                         && (lptkRht1->tkSynObj EQ soRP)
                                         && (lptkRht2->tkSynObj EQ soF)
                                         && (lptkRht3->tkSynObj EQ soMOP)
                                         && ((lptkRht4->tkSynObj EQ soSPA) || (lptkRht4->tkSynObj EQ soISPA)))
                                          ;
                            // Check for #5b & #5c & #6b & #6c
                            bAssignName |= ((lptkRht1 NE NULL)
                                         && (lptkRht2 NE NULL)
                                         && (lptkRht3 NE NULL)
                                         && ((lptkRht1->tkSynObj EQ soNR) || (lptkRht1->tkSynObj EQ soNNR))
                                         && (lptkRht2->tkSynObj EQ soF)
                                         && ((lptkRht3->tkSynObj EQ soSPA) || (lptkRht3->tkSynObj EQ soISPA)))
                                          ;
                        } // End IF

                        if (bAssignName)
                        {
                            // Mark as being assigned into
                            lpplYYLval->tkToken.tkFlags.bAssignName = TRUE;
                            lpplYYLval->tkToken.tkSynObj = soNAM;
                        } else
                        {
                            int I;

                            for (I = 1; I < RSTACKLEN2; I++)
                            {
                                lpYYRht1 = lpplLocalVars->lpMemPTD->lpplRhtStk[-I];
                                lptkRht1 = &lpYYRht1->tkToken;

                                if ((lptkRht1 EQ NULL)
                                 || ((lptkRht1->tkSynObj NE soA)
                                  && (lptkRht1->tkSynObj NE soNAM)))
                                    break;
                            } // End FOR

                            if (RSTACKLEN2 > (I + 1))
                            {
                                lpYYRht2 = lpplLocalVars->lpMemPTD->lpplRhtStk[-(I + 1)];
                                lptkRht2 = &lpYYRht2->tkToken;

                                if ((lptkRht1 NE NULL)
                                  && (lptkRht2 NE NULL)
                                  && ((lptkRht1->tkSynObj EQ soRP)
                                   || (lptkRht1->tkSynObj EQ soNR)
                                   || (lptkRht1->tkSynObj EQ soNNR))
                                  && (lptkRht2->tkSynObj EQ soSPA)
                                  && (lpYYRht2->lpplYYFcnCurry NE NULL))
                                {
                                    // Mark as being assigned into
                                    lpplYYLval->tkToken.tkFlags.bAssignName = bAssignName = TRUE;
                                    lpplYYLval->tkToken.tkSynObj = soNAM;
                                } // End IF
                            } // End IF
                        } // End IF/ELSE

                        // If we're not assigning into this name,
                        //   and we're not in the middle of "for I :in", ...
                        if (!lpplYYLval->tkToken.tkFlags.bAssignName
                         &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        {
                            // If it's an immediate, ...
                            if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
                            {
                                // Convert this to an unnamed immediate var
                                lpplYYLval->tkToken.tkFlags.TknType   = TKT_VARIMMED;
                                lpplYYLval->tkToken.tkFlags.ImmType   = lpplYYLval->tkToken.tkData.tkSym->stFlags.ImmType;
                                lpplYYLval->tkToken.tkData .tkLongest = lpplYYLval->tkToken.tkData.tkSym->stData.stLongest;
                            } else
                            {
                                // Convert this to an unnamed global var
                                lpplYYLval->tkToken.tkFlags.TknType   = TKT_VARARRAY;
                                lpplYYLval->tkToken.tkData .tkGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;

                                // Increment the refcnt
                                DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  any unassigned named var
                            } // End IF/ELSE

                            // Mark it as an Array so it can't be re-assigned.
                            lpplYYLval->tkToken.tkSynObj = soA;
                        } // End IF
                    } else
                        lpplYYLval->tkToken.tkSynObj = soNAM;

                    break;
                } // case NAMETYPE_VAR

                case NAMETYPE_FN0:
                    // Call this one TKT_FCNNAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;
                    lpplYYLval->tkToken.tkSynObj        = soNF;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_FCNNAMED;
                    break;

                case NAMETYPE_FN12:
                    // Call this one TKT_FCNNAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;

                    // If it's not an immediate,
                    //    and the function is not direct, ...
                    if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm
                     && !lpplYYLval->tkToken.tkData.tkSym->stFlags.FcnDir)
                    {
                        HGLOBAL hGlbUDFO;

                        // Get the global handle
                        hGlbUDFO = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;

                        // Allow a NULL hGlbUDFO if it's also being assigned
                        if (hGlbUDFO NE NULL
                         || !lpplYYLval->tkToken.tkFlags.bAssignName)
                        {
                            // Set the UDFO/AFO properties
                            plSetDfn (&lpplYYLval->tkToken, hGlbUDFO);

                            Assert (lpplYYLval->tkToken.tkSynObj EQ soF);
                        } // End IF
                    } else
                        lpplYYLval->tkToken.tkSynObj = soF;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_FCNNAMED;
                    break;

                case NAMETYPE_OP1:
                    // Call this one TKT_OP1NAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_OP1NAMED;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_OP1NAMED;
                    break;

                case NAMETYPE_OP2:
                    // Call this one TKT_OP2NAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_OP2NAMED;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_OP2NAMED;
                    break;

                case NAMETYPE_OP3:
                    // Call this one TKT_OP3NAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_OP3NAMED;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_OP3NAMED;
                    break;

                case NAMETYPE_TRN:
                    // Call this one TKT_FCNNAMED
                    lpplYYLval->tkToken.tkFlags.TknType = TKT_FCNNAMED;
                    lpplYYLval->tkToken.tkSynObj        = soF;

                    // If we're not assigning into this name,
                    //   and we're not in the middle of "for I :in", ...
                    if (!lpplYYLval->tkToken.tkFlags.bAssignName
                     &&  lpplLocalVars->lptkNext[1].tkFlags.TknType NE TKT_CS_IN)
                        goto PL_YYLEX_FCNNAMED;
                    break;

                case NAMETYPE_LST:
                defstop
                    break;
            } // End SWITCH

            // If the name is assigned into, or
            //   the next token is TKT_CS_IN, ...
            if (lpplLocalVars->lptkNext->tkFlags.bAssignName
             || lpplLocalVars->lptkNext[1].tkFlags.TknType EQ TKT_CS_IN)
                // Make it a NAME
                lpplYYLval->tkToken.tkSynObj = soNAM;

            break;
        } // End TKT_VARNAMED

        case TKT_ASSIGN:
            // If the next token is a right paren, we're starting Selective Specification
            if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_RIGHTPAREN)
            {
                // If we're already doing Selective Specification, signal an error
                if (lpplLocalVars->bSelSpec)
                {
                    break;          // ***FINISHME*** -- Selective Specification
                } // End IF

                // Mark as doing Selective Specification
                lpplLocalVars->bSelSpec =
                lpplLocalVars->bIniSpec = TRUE;
            } // End IF

            Assert (lpplYYLval->tkToken.tkSynObj EQ soSP);

            break;

        case TKT_LISTSEP:           // List separator
            Assert (lpplYYLval->tkToken.tkSynObj EQ soSEMI);

            break;

        case TKT_FCNAFO:            // Anonymous function
        case TKT_DELAFO:            // Del Anon -- either a function, or a monadic or dyadic operator, bound to its operands
            Assert (lpplYYLval->tkToken.tkSynObj EQ soF
                 || lpplYYLval->tkToken.tkSynObj EQ soNF);

            // If we should not restore the token stack ptr, ...
            if (!bRestoreStk)
                // Increment the refcnt
                DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  {del}{each}
            break;

        case TKT_OP1AFO:            // Anonymous monadic operator
            Assert (lpplYYLval->tkToken.tkSynObj EQ soMOP
                 || lpplYYLval->tkToken.tkSynObj EQ soMOPN);

            // If we should not restore the token stack ptr, ...
            if (!bRestoreStk)
                // Increment the refcnt twice
                DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  {del}{each}

            break;

        case TKT_OP2AFO:            // Anonymous dyadic operator
            Assert (lpplYYLval->tkToken.tkSynObj EQ soDOP
                 || lpplYYLval->tkToken.tkSynObj EQ soDOPN);

            // If we should not restore the token stack ptr, ...
            if (!bRestoreStk)
                // Increment the refcnt twice
                DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  {del}{each}

            break;

        case TKT_AFOGUARD:          // AFO guard
        case TKT_AFORETURN:         // AFO return
            break;

        case TKT_DEL:               // Del -- always a function
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur NE NULL)
            {
                // Fill in the ptr to the function header
                //   in both the return value and the token stream
                     lpplYYLval->tkToken.tkData.tkVoid   =
                lpplLocalVars->lptkNext->tkData.tkVoid   = MakePtrTypeGlb (lpSISCur->hGlbDfnHdr);
                     lpplYYLval->tkToken.tkFlags.TknType =
                lpplLocalVars->lptkNext->tkFlags.TknType = TKT_DELAFO;
                     lpplYYLval->tkToken.tkSynObj        =
                lpplLocalVars->lptkNext->tkSynObj        = soF;

                // If we should not restore the token stack ptr, ...
                if (!bRestoreStk)
                    // Increment the refcnt
                    DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  {del}{each}
            } else
            {
                // Mark it as a SYNTAX ERROR
                // Change the token type
                lpplYYLval->tkToken.tkSynObj = soSYNR;

                break;
            } // End IF/ELSE

            break;

        case TKT_DELDEL:            // Del Del -- either a monadic or dyadic operator
            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur NE NULL)
            {
                // Fill in the ptr to the function header
                //   in both the return value and the token stream
                     lpplYYLval->tkToken.tkData.tkVoid =
                lpplLocalVars->lptkNext->tkData.tkVoid = MakePtrTypeGlb (lpSISCur->hGlbDfnHdr);

                // If we should not restore the token stack ptr, ...
                if (!bRestoreStk)
                    // Increment the refcnt twice
                    DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  {del}{del}{each}

                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
////////////////////case DFNTYPE_FCN:
////////////////////    // Change it into an anonymous function
////////////////////    //   in both the return value and the token stream
////////////////////         lpplYYLval->tkToken.tkFlags.TknType =
////////////////////    lpplLocalVars->lptkNext->tkFlags.TknType = TKT_FCNAFO;
////////////////////    lpplLocalVars->lptkNext->tkFlags.TknType = TKT_FCNAFO;
////////////////////         lpplYYLval->tkToken.tkSynObj        =
////////////////////    lpplLocalVars->lptkNext->tkSynObj        = soF;
////////////////////
////////////////////    break
////////////////////
                    case DFNTYPE_OP1:
                        // Change it into an anonymous monadic operator
                        //   in both the return value and the token stream
                             lpplYYLval->tkToken.tkFlags.TknType =
                        lpplLocalVars->lptkNext->tkFlags.TknType = TKT_OP1AFO;
                             lpplYYLval->tkToken.tkSynObj        =
                        lpplLocalVars->lptkNext->tkSynObj        = soMOP;

                        break;

                    case DFNTYPE_OP2:
                        // Change it into an anonymous dyadic operator
                        //   in both the return value and the token stream
                             lpplYYLval->tkToken.tkFlags.TknType =
                        lpplLocalVars->lptkNext->tkFlags.TknType = TKT_OP2AFO;
                             lpplYYLval->tkToken.tkSynObj        =
                        lpplLocalVars->lptkNext->tkSynObj        = soDOP;

                        break;

                    default:
                        // Mark it as a SYNTAX ERROR
                        // Change the token type
                        lpplYYLval->tkToken.tkSynObj = soSYNR;

                        break;
                } // End SWITCH
            } else
            {
                // Mark it as a SYNTAX ERROR
                // Change the token type
                lpplYYLval->tkToken.tkSynObj = soSYNR;

                break;
            } // End IF/ELSE

            break;

        case TKT_SETALPHA:          // Set {alpha}
            Assert (lpplLocalVars->lptkNext[1].tkFlags.TknType EQ TKT_SOS);

            // If {alpha} has a value, ...
            if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Value)
                // Back off to the token to the right of the EOS/EOL
                //   so as to skip this stmt entirely
                lpplLocalVars->lptkNext -= (lpplLocalVars->lptkNext[1].tkData.tkIndex - 2);

            goto PL_YYLEX_START;    // Go around again

        case TKT_FCNIMMED:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soF
                 || lpplYYLval->tkToken.tkSynObj EQ soGO);
            break;

        case TKT_SOS:
        case TKT_LINECONT:
        case TKT_NOP:               // NOP
            goto PL_YYLEX_START;    // Ignore these tokens

        case TKT_CHRSTRAND:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soA);

            break;

        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soA);

            break;

        case TKT_OP1NAMED:
PL_YYLEX_OP1NAMED:
            // If we're not assigning into this name,
            if (!lpplYYLval->tkToken.tkFlags.bAssignName)
            {
                // If we should not restore the token stack ptr,
                if (!bRestoreStk)
                {
                    // If it's an immediate, ...
                    if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
                    {
////////////////////////// If the function is a hybrid, ...
////////////////////////if (lpplYYLval->tkToken.tkData.tkSym->stFlags.ImmType EQ IMMTYPE_PRIMOP3)
////////////////////////    // Mark is as a hybrid
////////////////////////    lpplYYLval->tkToken.tkSynObj = soHY;
////////////////////////else
                            // Mark it as a MOP so it can't be re-assigned.
                            lpplYYLval->tkToken.tkSynObj = soMOP;   // N.B.:  No immediate MOPNs

                        // Convert this to an unnamed immediate op1
                        lpplYYLval->tkToken.tkFlags.TknType   = TKT_OP1IMMED;
                        lpplYYLval->tkToken.tkFlags.ImmType   = lpplYYLval->tkToken.tkData.tkSym->stFlags.ImmType;
                        lpplYYLval->tkToken.tkData .tkChar    = lpplYYLval->tkToken.tkData.tkSym->stData.stChar;
                    } else
                    // If it's not a (named) direct fcn/opr ...
                    if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.FcnDir)
                    {
                        // Get the global memory handle
                        hGlbFcn = GetGlbHandle (&lpplYYLval->tkToken);

                        // Split cases based upon the array signature
                        switch (GetSignatureGlb_PTB (hGlbFcn))
                        {
                            case FCNARRAY_HEADER_SIGNATURE:
                                DbgBrk ();      // How do we ever get here?

                                // Lock the memory to get a ptr to it
                                lpMemHdrFcn = MyGlobalLockFcn (hGlbFcn);

                                // Convert this to an unnamed global fcn array
                                lpplYYLval->tkToken.tkFlags.TknType   = TKT_FCNARRAY;
                                lpplYYLval->tkToken.tkData .tkGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;
                                lpplYYLval->tkToken.tkSynObj          = soMOP;      // ***FIXME*** -- can this ever be soMOPN ???

                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL;

                                // Increment the RefCnt of each item
                                DbgIncrRefCntFcnArray (hGlbFcn);

                                break;

                            case DFN_HEADER_SIGNATURE:
                                // Lock the memory to get a ptr to it
                                lpMemDfnHdr = MyGlobalLockDfn (hGlbFcn);

#if FALSE       // Enable this code when we have a TKT_OP1DFN (unnamed OP1 DFN)
                                // Convert this to an unnamed global fcn array
                                lpplYYLval->tkToken.tkFlags.TknType   = lpMemDfnHdr->bAFO ? TKT_OP1AFO : TKT_OP1DFN;
                                lpplYYLval->tkToken.tkData .tkGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;
////////////////////////////////lpplYYLval->tkToken.tkSynObj          = ????        // See below
#endif
                                // Set the tkSynObj value from the UDFO header
                                lpplYYLval->tkToken.tkSynObj = TranslateDfnTypeToSOType (lpMemDfnHdr);

                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbFcn); lpMemDfnHdr = NULL;

                                // Increment the refcnt
                                DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  Any named OP2

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End IF/ELSE/...

                    // If the SynObj is still the default, ...
                    if (lpplYYLval->tkToken.tkSynObj EQ soA)
                        // Mark it as a MOP so it can't be re-assigned.
                        lpplYYLval->tkToken.tkSynObj = soMOP;
                } // End IF
            } else
                lpplYYLval->tkToken.tkSynObj = soNAM;

            break;

        case TKT_OP2NAMED:
PL_YYLEX_OP2NAMED:
            // If we're not assigning into this name,
            if (!lpplYYLval->tkToken.tkFlags.bAssignName)
            {
                // If we should not restore the token stack ptr,
                if (!bRestoreStk)
                {
                    // If it's an immediate, ...
                    if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
                    {
////////////////////////// If the function is a hybrid, ...
////////////////////////if (lpplYYLval->tkToken.tkData.tkSym->stFlags.ImmType EQ IMMTYPE_PRIMOP3)
////////////////////////    // Mark is as a hybrid
////////////////////////    lpplYYLval->tkToken.tkSynObj = soHY;
////////////////////////else
                            // Mark it as a DOP so it can't be re-assigned.
                            lpplYYLval->tkToken.tkSynObj = soDOP;   // N.B.:  No immediate DOPNs

                        // Convert this to an unnamed immediate op2
                        lpplYYLval->tkToken.tkFlags.TknType   = TKT_OP2IMMED;
                        lpplYYLval->tkToken.tkFlags.ImmType   = lpplYYLval->tkToken.tkData.tkSym->stFlags.ImmType;
                        lpplYYLval->tkToken.tkData .tkChar    = lpplYYLval->tkToken.tkData.tkSym->stData.stChar;
                    } else
                    // If it's not a (named) direct fcn/opr, ...
                    if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.FcnDir)
                    {
                        // Get the global memory handle
                        hGlbFcn = GetGlbHandle (&lpplYYLval->tkToken);

                        // Split cases based upon the array signature
                        switch (GetSignatureGlb_PTB (hGlbFcn))
                        {
                            case FCNARRAY_HEADER_SIGNATURE:
                                DbgBrk ();      // How do we ever get here?

                                // Lock the memory to get a ptr to it
                                lpMemHdrFcn = MyGlobalLockFcn (hGlbFcn);

                                // Convert this to an unnamed global fcn array
                                lpplYYLval->tkToken.tkFlags.TknType   = TKT_FCNARRAY;
                                lpplYYLval->tkToken.tkData .tkGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;
                                lpplYYLval->tkToken.tkSynObj          = soDOP;      // ***FIXME*** -- can this ever be soDOPN ???

                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbFcn); lpMemHdrFcn = NULL;

                                // Increment the RefCnt of each item
                                DbgIncrRefCntFcnArray (hGlbFcn);

                                break;

                            case DFN_HEADER_SIGNATURE:
                                // Lock the memory to get a ptr to it
                                lpMemDfnHdr = MyGlobalLockDfn (hGlbFcn);

#if FALSE       // Enable this code when we have a TKT_OP2DFN (unnamed OP2 DFN)
                                // Convert this to an unnamed global fcn array
                                lpplYYLval->tkToken.tkFlags.TknType   = lpMemDfnHdr->bAFO ? TKT_OP2AFO : TKT_OP2DFN;
                                lpplYYLval->tkToken.tkData .tkGlbData = lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData;
////////////////////////////////lpplYYLval->tkToken.tkSynObj          = ????        // See below
#endif
                                // Set the tkSynObj value from the UDFO header
                                lpplYYLval->tkToken.tkSynObj = TranslateDfnTypeToSOType (lpMemDfnHdr);

                                // We no longer need this ptr
                                MyGlobalUnlock (hGlbFcn); lpMemDfnHdr = NULL;

                                // Increment the refcnt
                                DbgIncrRefCntTkn (&lpplYYLval->tkToken);    // EXAMPLE:  Any named OP2

                                break;

                            defstop
                                break;
                        } // End SWITCH
                    } // End IF

                    // If the SynObj is still the default, ...
                    if (lpplYYLval->tkToken.tkSynObj EQ soA)
                        // Mark it as an DOP so it can't be re-assigned.
                        lpplYYLval->tkToken.tkSynObj = soDOP;
                } // End IF
            } else
                lpplYYLval->tkToken.tkSynObj = soNAM;

            break;

        case TKT_OP3NAMED:
PL_YYLEX_OP3NAMED:
            Assert (GetPtrTypeDir (lpplYYLval->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not a direct fcn/opr, ...
            if (!lpplYYLval->tkToken.tkData.tkSym->stFlags.FcnDir)
            {
                // If we're not assigning into this name,
                if (!lpplYYLval->tkToken.tkFlags.bAssignName)
                {
                    // If we should not restore the token stack ptr,
                    if (!bRestoreStk)
                    {
                        // If it's an immediate, ...
                        if (lpplYYLval->tkToken.tkData.tkSym->stFlags.Imm)
                        {
                            // Convert the named OP3 to an immediate OP3
                            lpplYYLval->tkToken.tkFlags.TknType   = TKT_OP3IMMED;
                            lpplYYLval->tkToken.tkFlags.ImmType   = IMMTYPE_PRIMOP3;
                            lpplYYLval->tkToken.tkSynObj          = soHY;
                            lpplYYLval->tkToken.tkData .tkChar    = lpplYYLval->tkToken.tkData.tkSym->stData.stChar;
                        } else
                        {
                            // Convert this to an unnamed global fcn
                            lpplYYLval->tkToken.tkFlags.TknType   = TKT_FCNARRAY;

                            // Copy the memory in case the hybrid changes to a function or operator
                            lpplYYLval->tkToken.tkData .tkGlbData =
                              CopyArray_EM (lpplYYLval->tkToken.tkData.tkSym->stData.stGlbData, NULL);

                            // If it succeeded, ...
                            if (lpplYYLval->tkToken.tkData.tkGlbData NE NULL)
                                // Make it a global ptr
                                lpplYYLval->tkToken.tkData.tkGlbData = MakePtrTypeGlb (lpplYYLval->tkToken.tkData.tkGlbData);
#ifdef DEBUG
                            else
                            {
                                DbgBrk ();              // #ifdef DEBUG -- ***FIXME***
////                            goto WSFULL_EXIT;       // ***FIXME***
                            } // End IF/ELSE
#endif
                            //***************************************************************************
                            // N.B.:  By not incrementing the RefCnt, this array will be deleted at the end
                            //***************************************************************************
                        } // End IF/ELSE

                        // Mark it as a hybrid so it can't be re-assigned
                        lpplYYLval->tkToken.tkSynObj = soHY;
                    } // End IF
                } else
                    lpplYYLval->tkToken.tkSynObj = soNAM;
            } // End IF

            // If the name is assigned into, ...
            if (lpplLocalVars->lptkNext->tkFlags.bAssignName)
                lpplYYLval->tkToken.tkSynObj = soNAM;

            break;

        case TKT_OP1IMMED:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soMOP);

            break;

        case TKT_OP2IMMED:
            // Split cases based upon the operator symbol
            switch (lpplYYLval->tkToken.tkData.tkChar)
            {
                case UTF16_JOT:
                    // If either the next or previous token is OP2NAMED,
                    //   or the previous token is OP1NAMED, ...
                    if (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP2NAMED
                     || lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP2NAMED
                     || lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP1NAMED

                    // If the previous token is VARNAMED and the name type is OP1 or OP2, ...
                     || (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_VARNAMED
                      && (lpplLocalVars->lptkNext[ 1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP1
                       || lpplLocalVars->lptkNext[ 1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2))

                    // If the next     token is VARNAMED and the name type is OP2, ...
                     || (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_VARNAMED
                      && lpplLocalVars->lptkNext[-1].tkData.tkSym->stFlags.stNameType EQ NAMETYPE_OP2)

                    // If the previous token is OP2IMMED and the value is not JOT, ...
                     || (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP2IMMED
                      && lpplLocalVars->lptkNext[ 1].tkData.tkChar NE UTF16_JOT)

                    // If the next     token is OP2IMMED and the value is not JOT, ...
                     || (lpplLocalVars->lptkNext[-1].tkFlags.TknType EQ TKT_OP2IMMED
                      && lpplLocalVars->lptkNext[-1].tkData.tkChar NE UTF16_JOT)

                    // If the previous token is OP1IMMED, ...
                     || (lpplLocalVars->lptkNext[ 1].tkFlags.TknType EQ TKT_OP1IMMED))
                    {
                        // Change the token type
                        lpplYYLval->tkToken.tkFlags.TknType = TKT_FILLJOT;
                        lpplYYLval->tkToken.tkSynObj        = soF;
                    } // End IF

                    break;

                default:
                    break;
            } // End SWITCH

            break;

        case TKT_OP3IMMED:
            Assert (lpplYYLval->tkToken.tkSynObj EQ soHY);

            break;

        case TKT_COLON:
            lpplYYLval->tkToken.tkSynObj = soSYNR;

            break;

        case TKT_OPJOTDOT:
        case TKT_LEFTPAREN:
        case TKT_RIGHTPAREN:
        case TKT_LEFTBRACKET:
        case TKT_RIGHTBRACKET:
        case TKT_LABELSEP:
        case TKT_EOS:
        case TKT_EOL:
        case TKT_LEFTBRACE:
            break;

        case TKT_RIGHTBRACE:
            // Point to the right brace token
            lpplYYLval->lptkRhtBrace  =  lpplLocalVars->lptkNext;

            // Point to the matching left brace token
            lpplYYLval->lptkLftBrace  = &lpplLocalVars->lptkStart[lpplYYLval->lptkRhtBrace->tkData.tkIndex];

            // If we should not restore the token stack ptr, ...
            if (!bRestoreStk)
            {
                MakeAfo_EM_YY (lpplYYLval, lpplLocalVars);

                // Point the next token to one to the right of the matching left brace
                //   so that the next token we process is the left brace
                lpplLocalVars->lptkNext = &lpplYYLval->lptkLftBrace[1];
            } // End IF

            break;

        case TKT_SYS_NS:            // Namespace
        case TKT_CS_ANDIF:          // Control structure:  ANDIF
        case TKT_CS_ASSERT:         // Control Structure:  ASSERT
        case TKT_CS_CASE:           // Control Structure:  CASE
        case TKT_CS_CASELIST:       // Control Structure:  CASELIST
        case TKT_CS_CONTINUE:       // Control Structure:  CONTINUE
        case TKT_CS_CONTINUEIF:     // Control Structure:  CONTINUEIF
        case TKT_CS_ELSE:           // Control Structure:  ELSE
        case TKT_CS_ELSEIF:         // Control Structure:  ELSEIF
        case TKT_CS_ENDFOR:         // Control Structure:  ENDFOR
        case TKT_CS_ENDFORLCL:      // Control Structure:  ENDFORLCL
        case TKT_CS_ENDREPEAT:      // Control Structure:  ENDREPEAT
        case TKT_CS_ENDWHILE:       // Control Structure:  ENDWHILE
        case TKT_CS_FOR:            // Control Structure:  FOR
        case TKT_CS_FORLCL:         // Control Structure:  FORLCL
        case TKT_CS_GOTO:           // Control Structure:  GOTO
        case TKT_CS_IF:             // Control Structure:  IF
        case TKT_CS_IN:             // Control Structure:  IN
        case TKT_CS_LEAVE:          // Control Structure:  LEAVE
        case TKT_CS_LEAVEIF:        // Control Structure:  LEAVEIF
        case TKT_CS_ORIF:           // Control Structure:  ORIF
        case TKT_CS_RETURN:         // Control Structure:  RETURN
        case TKT_CS_SELECT:         // Control Structure:  SELECT
        case TKT_CS_SKIPCASE:       // Control Structure:  Special token
        case TKT_CS_SKIPEND:        // Control Structure:  Special token
        case TKT_CS_UNTIL:          // Control Structure:  UNTIL
        case TKT_CS_WHILE:          // Control Structure:  WHILE
            break;

        case TKT_CS_REPEAT:         // Control Structure:  REPEAT
        case TKT_CS_ENDIF:          // Control Structure:  ENDIF
        case TKT_CS_ENDSELECT:      // ...                 ENDSELECT
        case TKT_CS_FOR2:           // ...                 FOR2
        case TKT_CS_IF2:            // ...                 IF2
        case TKT_CS_REPEAT2:        // ...                 REPEAT2
        case TKT_CS_SELECT2:        // ...                 SELECT2
        case TKT_CS_WHILE2:         // ...                 WHILE2
        case TKT_GLBDFN:            // Placeholder for hGlbDfnHdr
            goto PL_YYLEX_START;    // Ignore these tokens

        case TKT_SYNTERR:           // Syntax Error
            // Change the token type
            lpplYYLval->tkToken.tkSynObj = soSYNR;

            break;

        case TKT_CS_END:            // Control Structure:  END
        defstop
            break;
    } // End SWITCH

    // If we should restore the token stack ptr, ...
    if (bRestoreStk)
        lpplLocalVars->lptkNext = lptkOrigNext;

    Assert (IsValidSO (lpplYYLval->tkToken.tkSynObj));
} // End pl_yylexCOM
#undef  APPEND_NAME


#ifdef DEBUG
//***************************************************************************
//  $PadMemoryW
//
//  Padd out memory to a specified multiple
//***************************************************************************

void PadMemoryW
    (LPWCHAR lpwszTemp,         // Original location
     APLUINT uLen)              // Length of original location

{
    // Skip to the end of the name
    lpwszTemp += uLen;

    // Use modulus
    uLen = SONAMES_MAXLEN - (uLen % SONAMES_MAXLEN);

    while (uLen--)
        *lpwszTemp++ = L' ';
} // End PadMemoryW
#endif


#ifdef DEBUG
//***************************************************************************
//  $LSTACK
//
//  Display the left stack
//***************************************************************************

LPWCHAR LSTACK
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     LPPL_YYSTYPE *lpplOrgLftStk)   // Ptr to start of left stack

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    static  WCHAR wszTemp[100 * SONAMES_MAXLEN + 1] = {0};
    LPWCHAR       lpwszTemp,
                  lpwszEnd,
                  lpwszName;
    LPTOKEN       lptkNext;
    LPPL_YYSTYPE *lplpYYNext;
    SO_ENUM       tkSynObj;

    // Get the PerTabData ptr
    lpMemPTD = lpplLocalVars->lpMemPTD;

    // Get the ptr to the end of the stack text
    lpwszEnd  =
    lpwszTemp = &wszTemp[countof (wszTemp) - 2];

    // Get the next left stack ptr
    lplpYYNext = lpMemPTD->lpplLftStk;

    // Loop through the left stack (in memory)
    while (--lplpYYNext > lpplOrgLftStk)
    {
        // Get the token's syntax object
        tkSynObj = (*lplpYYNext)->tkToken.tkSynObj;

        Assert (IsValidSO (tkSynObj));

        if (tkSynObj EQ soEOS)
            break;

        // Back off to the next text location
        lpwszTemp -= SONAMES_MAXLEN;

        // Get ptr to name
        lpwszName = soNames[tkSynObj];

        // Append the next name
        strcpyW (lpwszTemp, lpwszName);

        // Pad the name with spaces to a multiple of SONAMES_MAXLEN
        PadMemoryW (lpwszTemp, lstrlenW (lpwszName));
    } // End WHILE

    // Get the next left stack ptr
    lptkNext = lpplLocalVars->lptkNext;

    // Loop through the left stack (from pl_yylex)
    while (lpplLocalVars->lptkEOS < --lptkNext)
    // Filter out various tokens
    if (TRUE
     && lptkNext->tkFlags.TknType NE TKT_CS_END
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDFOR
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDFORLCL
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDIF
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDREPEAT
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDSELECT
     && lptkNext->tkFlags.TknType NE TKT_CS_ENDWHILE
     && lptkNext->tkFlags.TknType NE TKT_CS_FOR2
     && lptkNext->tkFlags.TknType NE TKT_CS_IF2
     && lptkNext->tkFlags.TknType NE TKT_CS_REPEAT2
     && lptkNext->tkFlags.TknType NE TKT_CS_SELECT2
     && lptkNext->tkFlags.TknType NE TKT_CS_SKIPCASE
     && lptkNext->tkFlags.TknType NE TKT_CS_SKIPEND
     && lptkNext->tkFlags.TknType NE TKT_CS_WHILE2
//// && lptkNext->tkFlags.TknType NE TKT_EOS
     && lptkNext->tkFlags.TknType NE TKT_GLBDFN
     && lptkNext->tkFlags.TknType NE TKT_LABELSEP
     && lptkNext->tkFlags.TknType NE TKT_NOP
     && lptkNext->tkFlags.TknType NE TKT_SETALPHA
     && lptkNext->tkFlags.TknType NE TKT_SOS
     && lptkNext->tkFlags.TknType NE TKT_SYNTERR
       )
    {
        if (lptkNext->tkFlags.TknType EQ TKT_DELDEL)
        {
            LPSIS_HEADER lpSISCur;          // Ptr to current SIS layer

            // Search up the SIS chain to see what this is
            lpSISCur = SrchSISForDfn (lpMemPTD);

            // If the ptr is valid, ...
            if (lpSISCur NE NULL)
            {
                // Split case based upon the function type
                switch (lpSISCur->DfnType)
                {
                    case DFNTYPE_FCN:
                        lptkNext->tkSynObj = soF;

                        break;

                    case DFNTYPE_OP1:
                        lptkNext->tkSynObj = soMOP;

                        break;

                    case DFNTYPE_OP2:
                        lptkNext->tkSynObj = soDOP;

                        break;

                    default:
                        // Mark it as a SYNTAX ERROR
                        lptkNext->tkSynObj = soSYNR;

                        DbgStop ();         // #ifdef DEBUG

                        break;
                } // End SWITCH
            } // End IF
        } // End IF

        // Get the token's syntax object
        tkSynObj = lptkNext->tkSynObj;

        Assert (IsValidSO (tkSynObj));

        // Back off to the next text location
        lpwszTemp -= SONAMES_MAXLEN;

        // Get ptr to name
        lpwszName = soNames[tkSynObj];

        // Append the next name
        strcpyW (lpwszTemp, lpwszName);

        // Pad the name with spaces to a multiple of SONAMES_MAXLEN
        PadMemoryW (lpwszTemp, lstrlenW (lpwszName));

        // Stop if we're at an EOS/EOL
        if (lptkNext->tkFlags.TknType EQ TKT_EOS
         || lptkNext->tkFlags.TknType EQ TKT_EOL)
            break;
    } // End WHILE

    // Get ptr to start
    lpwszEnd = &lpwszEnd[-SONAMES_MAXLEN * 4];

    // If we're to pad the buffer, ...
    while (lpwszTemp > lpwszEnd)
        // Pad the buffer
        *--lpwszTemp = L' ';

    return lpwszEnd;
} // End LSTACK
#endif


#ifdef DEBUG
//***************************************************************************
//  $RSTACK
//
//  Display the right stack
//***************************************************************************

LPWCHAR RSTACK
    (LPPLLOCALVARS lpplLocalVars,   // Ptr to local plLocalVars
     LPPL_YYSTYPE *lpplOrgRhtStk)   // Ptr to start of right stack

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    static  WCHAR wszTemp[100 * SONAMES_MAXLEN + 1] = {0};
    LPWCHAR       lpwszTemp,
                  lpwszEnd,
                  lpwszName;
    LPPL_YYSTYPE *lplpYYNext;

    // Get the PerTabData ptr
    lpMemPTD = lpplLocalVars->lpMemPTD;

    // Get the ptr to the start of the stack text
    lpwszTemp = &wszTemp[0];

    // Get the next right stack ptr
    lplpYYNext = lpMemPTD->lpplRhtStk;

    // Loop through the right stack
    while (--lplpYYNext > lpplOrgRhtStk)
    {
        Assert (IsValidSO ((*lplpYYNext)->tkToken.tkSynObj));

        // Get ptr to name
        lpwszName = soNames[(*lplpYYNext)->tkToken.tkSynObj];

        // Append the next name
        strcpyW (lpwszTemp, lpwszName);

        // Pad the name with spaces to a multiple of SONAMES_MAXLEN
        PadMemoryW (lpwszTemp, lstrlenW (lpwszName));

        // Skip to the next text location
        lpwszTemp += SONAMES_MAXLEN;
    } // End WHILE

    // Get ptr to end
    lpwszEnd = &wszTemp[SONAMES_MAXLEN * 3];

    // If we're to pad the buffer, ...
    while (lpwszTemp < lpwszEnd)
        // Pad the buffer
        *lpwszTemp++ = L' ';

    // Ensure properly terminated
    lpwszTemp[0] = WC_EOS;

    return &wszTemp[0];
} // End RSTACK
#endif


//***************************************************************************
//  $plSetDfn
//
//  Subroutine to set UDFO/AFO properties
//***************************************************************************

NAME_TYPES plSetDfn
    (LPTOKEN lptkRes,                   // Ptr to the result
     HGLOBAL hGlbUDFO)                  // UDFO/AFO global memory handle

{
    UBOOL             bAFO,             // TRUE iff function is an AFO
                      bAfoArgs;         // TRUE iff the UDFO/AFO needs args
    LPDFN_HEADER      lpMemDfnHdr;      // Ptr to UDFO/AFO global memory header
    LPFCNARRAY_HEADER lpMemHdrFcn;      // ...    FCNARRAY ...
    DFN_TYPES         DfnType;          // DFN type (see DFN_TYPES)
    NAME_TYPES        nameType;         // The resulting NAMETYPE_xxx

    // Split cases based upon the signature
    switch (GetSignatureGlb_PTB (hGlbUDFO))
    {
        case DFN_HEADER_SIGNATURE:
            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLockDfn (hGlbUDFO);

            // Get the AFO flag
            bAFO = lpMemDfnHdr->bAFO;

            // Get the niladic function flag
            bAfoArgs = lpMemDfnHdr->FcnValence NE FCNVALENCE_NIL;

            // Get the DFN type
            DfnType = lpMemDfnHdr->DfnType;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbUDFO); lpMemDfnHdr = NULL;

            break;

        case FCNARRAY_HEADER_SIGNATURE:
            // Lock the memory to get a otr to it
            lpMemHdrFcn = MyGlobalLockFcn (hGlbUDFO);

            // Mark as not an AFO
            bAFO = FALSE;

            // But with arguments
            bAfoArgs = TRUE;

            // Split cases based upon the name type
            switch (lpMemHdrFcn->fnNameType)
            {
                case NAMETYPE_FN0:
                case NAMETYPE_FN12:
                case NAMETYPE_TRN:
                    DfnType = DFNTYPE_FCN;

                    break;

                case NAMETYPE_OP1:
                    DfnType = DFNTYPE_OP1;

                    break;

                case NAMETYPE_OP2:
                    DfnType = DFNTYPE_OP2;

                    break;

                case NAMETYPE_OP3:
                    DfnType = DFNTYPE_OP1;

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbUDFO); lpMemHdrFcn = NULL;

            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the DFNTYPE_xxx
    switch (DfnType)
    {
        case DFNTYPE_FCN:
            if (bAFO)
                lptkRes->tkFlags.TknType = TKT_FCNAFO;
            lptkRes->tkSynObj            = bAfoArgs ? soF   : soNF;
            nameType                     = bAfoArgs ? NAMETYPE_FN12 : NAMETYPE_FN0;

            break;

        case DFNTYPE_OP1:
            if (bAFO)
                lptkRes->tkFlags.TknType = TKT_OP1AFO;
            lptkRes->tkSynObj            = bAfoArgs ? soMOP : soMOPN;
            nameType                     = NAMETYPE_OP1;

            break;

        case DFNTYPE_OP2:
            if (bAFO)
                lptkRes->tkFlags.TknType = TKT_OP2AFO;
            lptkRes->tkSynObj            = bAfoArgs ? soDOP : soDOPN;
            nameType                     = NAMETYPE_OP2;

            break;

        defstop
            break;
    } // End SWITCH

    return nameType;
} // End plSetDfn


//***************************************************************************
//  $ExecuteCS0
//
//  Execute a CS0
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ExecuteCS0"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE ExecuteCS0
    (LPPLLOCALVARS lpplLocalVars,       // Ptr to local plLocalVars
     LPPL_YYSTYPE  lpplYYCurObj)        // Ptr to current PL_YYSTYPE

{
    LPPL_YYSTYPE lpYYRes = NULL,        // Ptr to the result
                 lpYYVar,               // ...    a temp
                 lpYYTmp;               // ...
    TOKEN        tkToken;               // The incoming token

    // Save the token type
    CopyAll (&tkToken, &lpplYYCurObj->tkToken);

    // YYFree the current object
    YYFree (lpplYYCurObj); lpplYYCurObj = NULL; // curSynObj = soNONE;

    // Split cases based upon the token type
    switch (tkToken.tkFlags.TknType)
    {
        case TKT_CS_CONTINUE:
            // Handle CONTINUE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_CONTINUE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ELSE:
            // Handle ELSE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ELSE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ENDFOR:
        case TKT_CS_ENDFORLCL:
            // Handle ENDFOR statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ENDFOR_Stmt_EM (lpplLocalVars, &tkToken, tkToken.tkFlags.TknType EQ TKT_CS_ENDFORLCL);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ENDREPEAT:
            // Handle ENDREPEAT statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ENDREPEAT_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_ENDWHILE:
            // Handle ENDWHILE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_ENDWHILE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_LEAVE:
            // Handle LEAVE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_LEAVE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_RETURN:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)   // RETURN
                goto SYNTAX_EXIT;

            // Allocate a new YYRes
            lpYYTmp = YYAlloc ();
            lpYYVar = YYAlloc ();

            // Copy a constant 0 as the last right object
            CopyAll (&lpYYTmp->tkToken, &tkZero);

            // Make the token into a YYSTYPE
            CopyAll (&lpYYVar->tkToken, &tkToken);

            // Call common {goto} code
            lpYYRes = plRedGO_A (lpplLocalVars, lpYYVar, lpYYTmp, soEOS);

            // Note these vars have been freed by <plRedGO_A>
            lpYYVar = lpYYTmp = NULL;

            break;

        case TKT_CS_SKIPCASE:
            // Handle SKIPCASE statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_SKIPCASE_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_SKIPEND:
            // Handle SKIPEND statement
            if (CheckCtrlBreak (lpplLocalVars->bCtrlBreak) || lpplLocalVars->bYYERROR)
                lpplLocalVars->bRet = FALSE;
            else
                lpplLocalVars->bRet =
                  CS_SKIPEND_Stmt (lpplLocalVars, &tkToken);

            if (lpplLocalVars->bRet)
            {
                // Make a NoValue entry as the result
                lpYYRes = MakeNoValue_YY (&tkToken);

                // Change the tkSynObj
                lpYYRes->tkToken.tkSynObj = soEOS;
            } else
                // Mark as in error
                lpplLocalVars->ExitType = EXITTYPE_ERROR;

            // No return value needed

            break;

        case TKT_CS_REPEAT:
            // Check for Ctrl Strucs in AFO
            if (lpplLocalVars->bAfoCtrlStruc)   // RETURN
                goto SYNTAX_EXIT;

            break;

///     case TKT_CS_ENDIF:          // CS0  Handled in pl_yylex
///     case TKT_CS_ENDSELECT:      // ...
///     case TKT_CS_FOR2:           // ...
///     case TKT_CS_IF2:            // ...
///     case TKT_CS_REPEAT2:        // ...
///     case TKT_CS_SELECT2:        // ...
///     case TKT_CS_WHILE2:         // ...
        defstop
            break;
    } // End SWITCH

    return lpYYRes;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpplLocalVars->lptkNext);
    return NULL;
} // End ExecuteCS0
#undef  APPEND_NAME


//***************************************************************************
//  $plExecuteFn0
//
//  Execute a niladic function in the context of ParseLine
//***************************************************************************

LPPL_YYSTYPE plExecuteFn0
    (LPPL_YYSTYPE lpYYFn0)              // Ptr to niladic function

{
    LPPL_YYSTYPE lpYYRes;               // Ptr to the result

    Assert (lpYYFn0->lpplYYArgCurry EQ NULL);
    Assert (lpYYFn0->lpplYYIdxCurry EQ NULL);
////Assert (lpYYFn0->lpplYYFcnCurry EQ NULL);   // Handled in UnFcnStrand_EM
////Assert (lpYYFn0->lpplYYOpRCurry EQ NULL);   // ...

    // Unstrand the function if appropriate
    UnFcnStrand_EM (&lpYYFn0, NAMETYPE_FN0, FALSE);

    // Execute the Niladic Function and replace the argument object with the result
    lpYYRes =
      ExecuteFn0 (lpYYFn0);

    // Free the function (including YYFree)
    FreeResult (lpYYFn0); YYFree (lpYYFn0); lpYYFn0 = NULL;

    // Check for error
    if (lpYYRes)
        lpYYRes->tkToken.tkSynObj = soA;

    return lpYYRes;     // Might be NULL
} // End plExecuteFn0


//***************************************************************************
//  End of File: parseline.c
//***************************************************************************
