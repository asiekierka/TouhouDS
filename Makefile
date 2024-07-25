export SHORTNAME 		:= Touhou DS
export LONGNAME 		:= Touhou DS
export VERSION	 		:= 0.9
ICON 		:= -b logo.bmp
EFS			:= $(CURDIR)/tools/efs/efs

#------------------------------------------------------------------------------
.SUFFIXES:
#------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

export TARGET := $(shell basename $(CURDIR))
export TOPDIR := $(CURDIR)

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#------------------------------------------------------------------------------
# main targets
#------------------------------------------------------------------------------
all: $(TARGET).nds $(TARGET)-EFS.nds

#------------------------------------------------------------------------------

dist-shared:
	@rm -rf dist
	@mkdir dist
	@mkdir dist/TouhouDS
	cp license.txt dist/TouhouDS
	cp readme.txt dist/TouhouDS
	cp changelog.txt dist/TouhouDS
	cp -r manual/ dist/TouhouDS
	cp -r tools/ dist/TouhouDS

dist: $(TARGET).nds
	$(MAKE) dist-shared
	cp $(TARGET).nds dist/TouhouDS
	cp -r TouhouDS/* dist/TouhouDS
	@find dist/TouhouDS/ -type d -name ".svn" | xargs rm -r
	@rm -f $(TARGET)-$(VERSION).7z
	cd dist && 7za a ../$(TARGET)-$(VERSION).7z TouhouDS

dist-src:
	$(MAKE) dist-shared
	cp run.bat dist/TouhouDS
	cp convert-graphics.bat dist/TouhouDS
	cp exception-to-src.bat dist/TouhouDS
	cp generate-lua-bindings.bat dist/TouhouDS
	cp Makefile dist/TouhouDS
	cp logo.bmp dist/TouhouDS
	cp -r src/ dist/TouhouDS
	cp -r src-art/ dist/TouhouDS
	@mkdir dist/TouhouDS/TouhouDS
	cp -r TouhouDS/* dist/TouhouDS/TouhouDS
	@find dist/TouhouDS/src/ -type d -name "build" | xargs rm -r
	@find dist/TouhouDS/     -type d -name ".svn"  | xargs rm -r
	@rm -f $(TARGET)-$(VERSION)-src.7z
	cd dist && 7za a ../$(TARGET)-$(VERSION)-src.7z TouhouDS

$(TARGET)-EFS.nds: $(TARGET).nds
	@rm -rf dist
	@mkdir dist
	cp -r TouhouDS/* dist/
	cp $(TARGET).nds dist/
	@find dist/ -type d -name ".svn" | xargs rm -r
	@ndstool -c $(TARGET)-EFS.nds -7 $(TARGET).arm7 -9 $(TARGET).arm9 $(ICON) "$(SHORTNAME);$(LONGNAME);$(VERSION)" -d $(CURDIR)/dist
	@$(EFS) $(notdir $@)

$(TARGET).nds: $(TARGET).arm7 $(TARGET).arm9
	@ndstool -c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9 $(ICON) "$(SHORTNAME);$(LONGNAME);$(VERSION)"

#------------------------------------------------------------------------------
$(TARGET).arm7	: arm7/$(TARGET).elf
$(TARGET).arm9	: arm9/$(TARGET).elf

#------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C src/arm7
	
#------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C src/arm9

#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
clean:
	$(MAKE) -C src/arm9 clean
	$(MAKE) -C src/arm7 clean
	rm -f $(TARGET).nds $(TARGET)-EFS.nds $(TARGET).arm7 $(TARGET).arm9 $(TARGET)-$(VERSION).7z $(TARGET)-$(VERSION)-src.7z
	rm -rf dist
