############################# APLCHARS EXTENSION #####################

EXNAM=aplchars                      # Extension name
CHR=chrome\content\$(EXNAM)\        # Directory for chrome files
LCL=chrome\locale\en-US\$(EXNAM)\   # ...           locale ...
SKN=chrome\skin\classic\$(EXNAM)\   # ...           skin   ...
###ZIPPRG=                          # Path and name of zip program (7-zip)
LCLDIR=P:\SudleyPlace.com\htdocs\APL\aplchars\ # Local directory

!ifndef ZIPPRG
!error Environment variable ZIPPRG not defined.
!endif

ALL:    $(EXNAM).xpi                        \
        $(LCLDIR)$(EXNAM).xpi               \
        $(LCLDIR)update.rdf

chrome\$(EXNAM).jar:    $(CHR)*.*           \
                        $(LCL)*.*           \
                        $(SKN)*.*           \
                        upd.inp
        @php ..\incver.php install.rdf $(CHR)about.xul update.rdf
        @cd chrome
        @if exist $(EXNAM).jar del $(EXNAM).jar
        @$(ZIPPRG) a -tzip -r $(EXNAM).jar *.* -x@..\upd.inp
        @cd ..

$(EXNAM).xpi:           chrome.manifest             \
                        install.rdf                 \
                        LICENSE.TXT                 \
                        upd.inp                     \
                        defaults\preferences\*.js   \
                        chrome\$(EXNAM).jar         \
                        makefile
        @if exist $(EXNAM).xpi del $(EXNAM).xpi
        @$(ZIPPRG) a -tzip $(EXNAM).xpi *.* -x@upd.inp
        @$(ZIPPRG) a -tzip -r $(EXNAM).xpi defaults\preferences\*.js
        @$(ZIPPRG) a -tzip $(EXNAM).xpi chrome\$(EXNAM).jar

$(LCLDIR)$(EXNAM).xpi:  $(EXNAM).xpi
        copy %s $@ >NUL

$(LCLDIR)update.rdf:    update.rdf
        copy %s $@ >NUL

