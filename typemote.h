//***************************************************************************
//  NARS2000 -- Type Promote/Demote Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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

#define TP_MAT                                                                                                                                                     \
{/*     BOOL          INT          FLT         CHAR         HETERO       NESTED       LIST          APA          RAT          VFP         INIT     */              \
   {M(BOOL,BOOL),M(BOOL,INT ),M(BOOL,FLT ),M(BOOL,HETE),M(BOOL,HETE),M(BOOL,NEST),M(BOOL,ERR ),M(BOOL,INT ),M(BOOL,RAT ),M(BOOL,VFP ),M(BOOL,BOOL)},   /* BOOL */  \
   {M(INT ,INT ),M(INT ,INT ),M(INT ,FLT ),M(INT ,HETE),M(INT ,HETE),M(INT ,NEST),M(INT ,ERR ),M(INT ,INT ),M(INT ,RAT ),M(INT ,VFP ),M(INT ,INT )},   /* INT  */  \
   {M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,FLT ),M(FLT ,HETE),M(FLT ,HETE),M(FLT ,NEST),M(FLT ,ERR ),M(FLT ,FLT ),M(FLT ,VFP ),M(FLT ,VFP ),M(FLT ,FLT )},   /* FLT  */  \
   {M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,CHAR),M(CHAR,HETE),M(CHAR,NEST),M(CHAR,ERR ),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,HETE),M(CHAR,CHAR)},   /* CHAR */  \
   {M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,NEST),M(HETE,ERR ),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE),M(HETE,HETE)},   /* HETE */  \
   {M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,ERR ),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST),M(NEST,NEST)},   /* NEST */  \
   {M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR )},   /* LIST */  \
   {M(APA ,INT ),M(APA ,INT ),M(APA ,FLT ),M(APA ,HETE),M(APA ,HETE),M(APA ,NEST),M(APA ,ERR ),M(APA ,INT ),M(APA ,RAT ),M(APA ,VFP ),M(APA ,INT )},   /* APA  */  \
   {M(RAT ,RAT ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,HETE),M(RAT ,HETE),M(RAT ,NEST),M(RAT ,ERR ),M(RAT ,RAT ),M(RAT ,RAT ),M(RAT ,VFP ),M(RAT ,RAT )},   /* RAT  */  \
   {M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,HETE),M(VFP ,HETE),M(VFP ,NEST),M(VFP ,ERR ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP ),M(VFP ,VFP )},   /* VFP  */  \
   {M(INIT,BOOL),M(INIT,INT ),M(INIT,FLT ),M(INIT,CHAR),M(INIT,HETE),M(INIT,NEST),M(INIT,ERR ),M(INIT,INT ),M(INIT,RAT ),M(INIT,VFP ),M(INIT,INIT)},   /* INIT */  \
}

// ARRAY_xxx Type Promotion result matrix
EXTERN
APLSTYPE aTypePromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES

#define ARRAY_FLT       ARRAY_FLOAT
#define ARRAY_HETE      ARRAY_HETERO
#define ARRAY_NEST      ARRAY_NESTED
#define ARRAY_ERR       ARRAY_ERROR
#define ARRAY_INIT      ARRAY_ERROR

#define M(a,b)          ARRAY_##b
= TP_MAT
#undef  M

#undef  ARRAY_INIT
#undef  ARRAY_ERR
#undef  ARRAY_NEST
#undef  ARRAY_HETE
#undef  ARRAY_FLT

#endif
;

typedef void (*TPT_ACTION)  (LPTOKEN);

#ifdef DEFINE_VALUES
void           TPT_IDENT    (LPTOKEN);
void           TPT_ERROR    (LPTOKEN);
void           TPT_BOOL2INT (LPTOKEN);
void           TPT_BOOL2FLT (LPTOKEN);
void           TPT_BOOL2RAT (LPTOKEN);
void           TPT_BOOL2VFP (LPTOKEN);
void           TPT_INT2FLT  (LPTOKEN);
void           TPT_INT2RAT  (LPTOKEN);
void           TPT_INT2VFP  (LPTOKEN);
void           TPT_FLT2VFP  (LPTOKEN);
void           TPT_APA2FLT  (LPTOKEN);
void           TPT_APA2RAT  (LPTOKEN);
void           TPT_APA2VFP  (LPTOKEN);
void           TPT_RAT2VFP  (LPTOKEN);
#endif

#define TPT_BOOL2BOOL   TPT_IDENT
#define TPT_INT2INT     TPT_IDENT
#define TPT_FLT2FLT     TPT_IDENT
#define TPT_CHAR2CHAR   TPT_IDENT
#define TPT_HETE2HETE   TPT_IDENT
#define TPT_HETE2NEST   TPT_IDENT
#define TPT_NEST2NEST   TPT_IDENT
#define TPT_RAT2RAT     TPT_IDENT
#define TPT_VFP2VFP     TPT_IDENT

#define TPT_BOOL2ERR    TPT_ERROR
#define TPT_INT2ERR     TPT_ERROR
#define TPT_FLT2ERR     TPT_ERROR
#define TPT_CHAR2ERR    TPT_ERROR
#define TPT_HETE2ERR    TPT_ERROR
#define TPT_NEST2ERR    TPT_ERROR
#define TPT_LIST2ERR    TPT_ERROR
#define TPT_APA2ERR     TPT_ERROR
#define TPT_RAT2ERR     TPT_ERROR
#define TPT_VFP2ERR     TPT_ERROR
#define TPT_INIT2ERR    TPT_ERROR

#define TPT_INIT2BOOL   TPT_ERROR
#define TPT_INIT2INT    TPT_ERROR
#define TPT_INIT2FLT    TPT_ERROR
#define TPT_INIT2CHAR   TPT_ERROR
#define TPT_INIT2HETE   TPT_ERROR
#define TPT_INIT2NEST   TPT_ERROR
#define TPT_INIT2LIST   TPT_ERROR
#define TPT_INIT2APA    TPT_ERROR
#define TPT_INIT2RAT    TPT_ERROR
#define TPT_INIT2VFP    TPT_ERROR
#define TPT_INIT2INIT   TPT_ERROR

#define TPT_BOOL2HETE   TPT_IDENT
#define TPT_BOOL2NEST   TPT_IDENT
#define TPT_INT2HETE    TPT_IDENT
#define TPT_INT2NEST    TPT_IDENT
#define TPT_FLT2HETE    TPT_IDENT
#define TPT_FLT2NEST    TPT_IDENT
#define TPT_CHAR2HETE   TPT_IDENT
#define TPT_CHAR2NEST   TPT_IDENT
#define TPT_APA2HETE    TPT_IDENT
#define TPT_APA2NEST    TPT_IDENT
#define TPT_RAT2HETE    TPT_IDENT
#define TPT_RAT2NEST    TPT_IDENT
#define TPT_VFP2HETE    TPT_IDENT
#define TPT_VFP2NEST    TPT_IDENT

#define TPT_APA2INT     TPT_IDENT

// Type Promotion Token matrix
EXTERN
TPT_ACTION aTypeTknPromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES

#define M(a,b)          TPT_##a##2##b
 = TP_MAT
#undef  M

#endif
;

#undef  TPT_APA2INT

#undef  TPT_VFP2NEST
#undef  TPT_VFP2HETE
#undef  TPT_RAT2NEST
#undef  TPT_RAT2HETE
#undef  TPT_APA2NEST
#undef  TPT_APA2HETE
#undef  TPT_CHAR2NEST
#undef  TPT_CHAR2HETE
#undef  TPT_FLT2NEST
#undef  TPT_FLT2HETE
#undef  TPT_INT2NEST
#undef  TPT_INT2HETE
#undef  TPT_BOOL2NEST
#undef  TPT_BOOL2HETE

#undef  TPT_INIT2INIT
#undef  TPT_INIT2VFP
#undef  TPT_INIT2RAT
#undef  TPT_INIT2APA
#undef  TPT_INIT2LIST
#undef  TPT_INIT2NEST
#undef  TPT_INIT2HETE
#undef  TPT_INIT2CHAR
#undef  TPT_INIT2FLT
#undef  TPT_INIT2INT
#undef  TPT_INIT2BOOL

#undef  TPT_INIT2ERR
#undef  TPT_VFP2ERR
#undef  TPT_RAT2ERR
#undef  TPT_APA2ERR
#undef  TPT_LIST2ERR
#undef  TPT_NEST2ERR
#undef  TPT_HETE2ERR
#undef  TPT_CHAR2ERR
#undef  TPT_FLT2ERR
#undef  TPT_INT2ERR
#undef  TPT_BOOL2ERR

#undef  TPT_VFP2VFP
#undef  TPT_RAT2RAT
#undef  TPT_NEST2NEST
#undef  TPT_HETE2NEST
#undef  TPT_HETE2HETE
#undef  TPT_CHAR2CHAR
#undef  TPT_FLT2FLT
#undef  TPT_INT2INT
#undef  TPT_BOOL2BOOL


typedef void (*TP_ACTION)    (LPVOID, APLINT, LPALLTYPES);

#ifdef DEFINE_VALUES
void           TPA_ERROR     (LPVOID      , APLINT, LPALLTYPES);
void           TPA_BOOL2BOOL (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2INT  (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2FLT  (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2HETE (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2NEST (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2RAT  (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_BOOL2VFP  (LPAPLBOOL   , APLINT, LPALLTYPES);
void           TPA_INT2INT   (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2FLT   (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2HETE  (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2NEST  (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2RAT   (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_INT2VFP   (LPAPLINT    , APLINT, LPALLTYPES);
void           TPA_APA2INT   (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2FLT   (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2HETE  (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2NEST  (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2RAT   (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_APA2VFP   (LPAPLAPA    , APLINT, LPALLTYPES);
void           TPA_FLT2FLT   (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TPA_FLT2HETE  (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TPA_FLT2NEST  (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TPA_FLT2VFP   (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TPA_CHAR2CHAR (LPAPLCHAR   , APLINT, LPALLTYPES);
void           TPA_CHAR2HETE (LPAPLCHAR   , APLINT, LPALLTYPES);
void           TPA_CHAR2NEST (LPAPLCHAR   , APLINT, LPALLTYPES);
void           TPA_HETE2HETE (LPAPLHETERO , APLINT, LPALLTYPES);
void           TPA_HETE2NEST (LPAPLHETERO , APLINT, LPALLTYPES);
void           TPA_NEST2NEST (LPAPLNESTED , APLINT, LPALLTYPES);
void           TPA_RAT2HETE  (LPAPLRAT    , APLINT, LPALLTYPES);
void           TPA_RAT2NEST  (LPAPLRAT    , APLINT, LPALLTYPES);
void           TPA_RAT2RAT   (LPAPLRAT    , APLINT, LPALLTYPES);
void           TPA_RAT2VFP   (LPAPLRAT    , APLINT, LPALLTYPES);
void           TPA_VFP2HETE  (LPAPLVFP    , APLINT, LPALLTYPES);
void           TPA_VFP2NEST  (LPAPLVFP    , APLINT, LPALLTYPES);
void           TPA_VFP2VFP   (LPAPLVFP    , APLINT, LPALLTYPES);
#endif

#define TPA_BOOL2ERR    TPA_ERROR
#define TPA_INT2ERR     TPA_ERROR
#define TPA_FLT2ERR     TPA_ERROR
#define TPA_CHAR2ERR    TPA_ERROR
#define TPA_HETE2ERR    TPA_ERROR
#define TPA_NEST2ERR    TPA_ERROR
#define TPA_LIST2ERR    TPA_ERROR
#define TPA_APA2ERR     TPA_ERROR
#define TPA_RAT2ERR     TPA_ERROR
#define TPA_VFP2ERR     TPA_ERROR
#define TPA_INIT2ERR    TPA_ERROR

#define TPA_INIT2BOOL   TPA_ERROR
#define TPA_INIT2INT    TPA_ERROR
#define TPA_INIT2FLT    TPA_ERROR
#define TPA_INIT2CHAR   TPA_ERROR
#define TPA_INIT2HETE   TPA_ERROR
#define TPA_INIT2NEST   TPA_ERROR
#define TPA_INIT2LIST   TPA_ERROR
#define TPA_INIT2APA    TPA_ERROR
#define TPA_INIT2RAT    TPA_ERROR
#define TPA_INIT2VFP    TPA_ERROR
#define TPA_INIT2INIT   TPA_ERROR

// Type Promotion Action matrix
TP_ACTION aTypeActPromote[ARRAY_LENGTH + 1][ARRAY_LENGTH + 1]
#ifdef DEFINE_VALUES

#define M(a,b)          TPA_##a##2##b
 = TP_MAT
#undef  M

#endif
;

#undef  TPA_INIT2INIT
#undef  TPA_INIT2VFP
#undef  TPA_INIT2RAT
#undef  TPA_INIT2APA
#undef  TPA_INIT2LIST
#undef  TPA_INIT2NEST
#undef  TPA_INIT2HETE
#undef  TPA_INIT2CHAR
#undef  TPA_INIT2FLT
#undef  TPA_INIT2INT
#undef  TPA_INIT2BOOL

#undef  TPA_INIT2ERR
#undef  TPA_VFP2ERR
#undef  TPA_RAT2ERR
#undef  TPA_APA2ERR
#undef  TPA_LIST2ERR
#undef  TPA_NEST2ERR
#undef  TPA_HETE2ERR
#undef  TPA_CHAR2ERR
#undef  TPA_FLT2ERR
#undef  TPA_INT2ERR
#undef  TPA_BOOL2ERR


#define TC_MAT                                                                                                                                                     \
{/*     BOOL          INT          FLT         CHAR         HETERO       NESTED       LIST          APA          RAT          VFP     */              \
   {M(BOOL,BOOL),M(BOOL,INT ),M(BOOL,FLT ),M(BOOL,ERR ),M(BOOL,ERR ),M(BOOL,ERR ),M(BOOL,ERR ),M(BOOL,ERR ),M(BOOL,RAT ),M(BOOL,VFP )},   /* BOOL */  \
   {M(INT ,BOOL),M(INT ,INT ),M(INT ,FLT ),M(INT ,ERR ),M(INT ,ERR ),M(INT ,ERR ),M(INT ,ERR ),M(INT ,ERR ),M(INT ,RAT ),M(INT ,VFP )},   /* INT  */  \
   {M(FLT ,BOOL),M(FLT ,INT ),M(FLT ,FLT ),M(FLT ,ERR ),M(FLT ,ERR ),M(FLT ,ERR ),M(FLT ,ERR ),M(FLT ,ERR ),M(FLT ,RAT ),M(FLT ,VFP )},   /* FLT  */  \
   {M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,CHAR),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR ),M(CHAR,ERR )},   /* CHAR */  \
   {M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,HETE),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR ),M(HETE,ERR )},   /* HETE */  \
   {M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,NEST),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR ),M(NEST,ERR )},   /* NEST */  \
   {M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR ),M(LIST,ERR )},   /* LIST */  \
   {M(APA ,BOOL),M(APA ,INT ),M(APA ,FLT ),M(APA ,ERR ),M(APA ,ERR ),M(APA ,ERR ),M(APA ,ERR ),M(APA ,ERR ),M(APA ,RAT ),M(APA ,VFP )},   /* APA  */  \
   {M(RAT ,BOOL),M(RAT ,INT ),M(RAT ,FLT ),M(RAT ,ERR ),M(RAT ,ERR ),M(RAT ,ERR ),M(RAT ,ERR ),M(RAT ,ERR ),M(RAT ,RAT ),M(RAT ,VFP )},   /* RAT  */  \
   {M(VFP ,BOOL),M(VFP ,INT ),M(VFP ,FLT ),M(VFP ,ERR ),M(VFP ,ERR ),M(VFP ,ERR ),M(VFP ,ERR ),M(VFP ,ERR ),M(VFP ,RAT ),M(VFP ,VFP )},   /* VFP  */  \
}

typedef void (*TC_ACTION)    (LPVOID, APLINT, LPALLTYPES);

#ifdef DEFINE_VALUES
void           TCA_FLT2BOOL  (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TCA_FLT2INT   (LPAPLFLOAT  , APLINT, LPALLTYPES);
void           TCA_FLT2RAT   (LPAPLFLOAT  , APLINT, LPALLTYPES);

void           TCA_RAT2BOOL  (LPAPLRAT    , APLINT, LPALLTYPES);
void           TCA_RAT2INT   (LPAPLRAT    , APLINT, LPALLTYPES);
void           TCA_RAT2FLT   (LPAPLRAT    , APLINT, LPALLTYPES);

void           TCA_VFP2BOOL  (LPAPLVFP    , APLINT, LPALLTYPES);
void           TCA_VFP2INT   (LPAPLVFP    , APLINT, LPALLTYPES);
void           TCA_VFP2FLT   (LPAPLVFP    , APLINT, LPALLTYPES);
void           TCA_VFP2RAT   (LPAPLVFP    , APLINT, LPALLTYPES);
#endif

#define TCA_BOOL2ERR    TCA_ERROR
#define TCA_INT2ERR     TCA_ERROR
#define TCA_FLT2ERR     TCA_ERROR
#define TCA_CHAR2ERR    TCA_ERROR
#define TCA_HETE2ERR    TCA_ERROR
#define TCA_NEST2ERR    TCA_ERROR
#define TCA_LIST2ERR    TCA_ERROR
#define TCA_APA2ERR     TCA_ERROR
#define TCA_RAT2ERR     TCA_ERROR
#define TCA_VFP2ERR     TCA_ERROR
#define TCA_INIT2ERR    TCA_ERROR

#define TCA_ERROR       TPA_ERROR
#define TCA_BOOL2BOOL   TPA_BOOL2BOOL
#define TCA_BOOL2INT    TPA_BOOL2INT
#define TCA_BOOL2FLT    TPA_BOOL2FLT
#define TCA_BOOL2RAT    TPA_BOOL2RAT
#define TCA_BOOL2VFP    TPA_BOOL2VFP
#define TCA_INT2BOOL    TPA_INT2INT
#define TCA_INT2INT     TPA_INT2INT
#define TCA_INT2FLT     TPA_INT2FLT
#define TCA_INT2RAT     TPA_INT2RAT
#define TCA_INT2VFP     TPA_INT2VFP
#define TCA_FLT2FLT     TPA_FLT2FLT
#define TCA_FLT2VFP     TPA_FLT2VFP
#define TCA_CHAR2CHAR   TPA_CHAR2CHAR
#define TCA_APA2BOOL    TPA_APA2INT
#define TCA_APA2INT     TPA_APA2INT
#define TCA_APA2FLT     TPA_APA2FLT
#define TCA_APA2RAT     TPA_APA2RAT
#define TCA_APA2VFP     TPA_APA2VFP
#define TCA_RAT2VFP     TPA_RAT2VFP
#define TCA_HETE2HETE   TPA_HETE2HETE
#define TCA_NEST2NEST   TPA_NEST2NEST
#define TCA_RAT2RAT     TPA_RAT2RAT
#define TCA_VFP2VFP     TPA_VFP2VFP

#define TCA_FLT2BOOL    TCA_FLT2INT
#define TCA_RAT2BOOL    TCA_RAT2INT
#define TCA_VFP2BOOL    TCA_VFP2INT

// Type Conversion Action matrix
TC_ACTION aTypeActConvert[ARRAY_LENGTH][ARRAY_LENGTH]
#ifdef DEFINE_VALUES

#define M(a,b)          TCA_##a##2##b
 = TC_MAT
#undef  M

#endif
;

#undef  TCA_VFP2BOOL
#undef  TCA_RAT2BOOL
#undef  TCA_FLT2BOOL

#undef  TCA_VFP2VFP
#undef  TCA_RAT2RAT
#undef  TCA_NEST2NEST
#undef  TCA_HETE2HETE
#undef  TCA_RAT2VFP
#undef  TCA_APA2VFP
#undef  TCA_APA2RAT
#undef  TCA_APA2FLT
#undef  TCA_APA2INT
#undef  TCA_APA2BOOL
#undef  TCA_CHAR2CHAR
#undef  TCA_FLT2VFP
#undef  TCA_FLT2FLT
#undef  TCA_INT2VFP
#undef  TCA_INT2RAT
#undef  TCA_INT2FLT
#undef  TCA_INT2INT
#undef  TCA_INT2BOOL
#undef  TCA_BOOL2VFP
#undef  TCA_BOOL2RAT
#undef  TCA_BOOL2FLT
#undef  TCA_BOOL2INT
#undef  TCA_BOOL2BOOL
#undef  TCA_ERROR

#undef  TCA_INIT2ERR
#undef  TCA_VFP2ERR
#undef  TCA_RAT2ERR
#undef  TCA_APA2ERR
#undef  TCA_LIST2ERR
#undef  TCA_NEST2ERR
#undef  TCA_HETE2ERR
#undef  TCA_CHAR2ERR
#undef  TCA_FLT2ERR
#undef  TCA_INT2ERR
#undef  TCA_BOOL2ERR


//***************************************************************************
//  End of File: typemote.h
//***************************************************************************
