# /***************************************************************************
#     NARS2000 -- An Experimental APL Interpreter
#     Copyright (C) 2006-2014 Sudley Place Software
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ***************************************************************************/

# This makefile generates the .pro files and is
#   called from VC Express as a pre-build event.

WS=WineHQ\                      # Wine source
MS=$(NARSROOT)msieve\           # Msieve source
MPFR=MPFR\                      # MPFR source

!ifndef O
!error makefile.pro:  Macro O is not defined.
!endif

!include makefile.src

{}.c{$(O)}.pro:
        @echo $(@F) #1
        @php $(MAKEPRO) %s $@

{}.cpp{$(O)}.pro:
        @echo $(@F) #1
        @php $(MAKEPRO) %s $@

{$(O)}.c{$(O)}.pro:
        @echo $(@F) #1
        @php $(MAKEPRO) %s $@

{$(WS)}.c{$(O)}.pro:
        @echo $(@F) #1
        @php $(MAKEPRO) %s $@

{$(MS)}.c{$(O)}.pro:
        @echo $(@F) #1
        @php $(MAKEPRO) %s $@

{$(MPFR)}.c{$(O)}.pro:
        @echo $(@F) #1
        @php $(MAKEPRO) %s $@

ALL:$(SRC:???=pro)

