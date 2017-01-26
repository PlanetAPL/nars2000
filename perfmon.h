//***************************************************************************
//  NARS2000 -- Performance Monitoring Header
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

// Toggle this between defined and not to turn performance monitoring on/off
//#define PERFMONON

#ifdef PERFMONON
  #define PERFMONINIT PerfMonInit
  #define PERFMON     PerfMonAccum (FNLN);
  #define PERFMONSHOW PerfMonShow
#else
  #define PERFMONINIT(a)
  #define PERFMON
  #define PERFMONSHOW(a)
#endif


//***************************************************************************
//  End of File: perfmon.h
//***************************************************************************
