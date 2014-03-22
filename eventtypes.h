//***************************************************************************
//  NARS2000 -- Event Types
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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

// Use this macro to join major and minor events into a single value
#define MAKE_ET(Major,Minor)    ((((USHORT) Major) << 16) | (USHORT) Minor)

// Use these macros to split apart an EventType into its major and minor parts
#define ET_MAJOR(et)            (((UINT) et) >> 16)
#define ET_MINOR(et)            (((UINT) et) & 0xFFFF)

// The EventType type
#define EVENT_TYPES             UINT

// The event types, taken from the APL2 Language Reference Manual (Feb 1994)
typedef enum tagEVENT_TYPES
{
    EVENTTYPE_NOERROR                   = MAKE_ET (0,  0),
    EVENTTYPE_UNK                       = MAKE_ET (0,  1),

// Resource errors
    EVENTTYPE_BREAK                     = MAKE_ET (1,  1),
    EVENTTYPE_SYSTEM_ERROR              = MAKE_ET (1,  2),
    EVENTTYPE_WS_FULL                   = MAKE_ET (1,  3),
    EVENTTYPE_LIMIT_ST                  = MAKE_ET (1,  4),
    EVENTTYPE_LIMIT_RANK                = MAKE_ET (1,  8),
    EVENTTYPE_LIMIT_IMPL                = MAKE_ET (1, 13),

// SYNTAX ERRORs
    EVENTTYPE_SYNTAX_ERROR_ARGOMITTED   = MAKE_ET (2,  1),
    EVENTTYPE_SYNTAX_ERROR_ILLFORMED    = MAKE_ET (2,  2),
    EVENTTYPE_SYNTAX_ERROR_NAMECLASS    = MAKE_ET (2,  3),
    EVENTTYPE_SYNTAX_ERROR_CONTEXT      = MAKE_ET (2,  4),

// VALUE_ERRORs
    EVENTTYPE_VALUE_ERROR_NAME          = MAKE_ET (3,  1),
    EVENTTYPE_VALUE_ERROR_FCN           = MAKE_ET (3,  2),

// Explicit argument errors
    EVENTTYPE_VALENCE_ERROR             = MAKE_ET (5,  1),
    EVENTTYPE_RANK_ERROR                = MAKE_ET (5,  2),
    EVENTTYPE_LENGTH_ERROR              = MAKE_ET (5,  3),
    EVENTTYPE_DOMAIN_ERROR              = MAKE_ET (5,  4),
    EVENTTYPE_INDEX_ERROR               = MAKE_ET (5,  5),
    EVENTTYPE_AXIS_ERROR                = MAKE_ET (5,  6),
} EVENT_TYPES, *LPEVENT_TYPES;

// N.B.:  Whenever changing the above enum
//   be sure to make a corresponding change to
//   <SysFnDydES_EM_YY> in <qf_es.c>.


//***************************************************************************
//  End of File: eventtypes.h
//***************************************************************************
