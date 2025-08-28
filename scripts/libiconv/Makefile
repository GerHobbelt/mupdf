
DEPENDENCIES=											            		\
		../../../scripts/libiconv/Makefile


		
.PHONY: all clean forced-rebuild regenerate


all: regenerate
	@echo "ICONV_TOOL: $(ICONV_TOOL)"
	-$(ICONV_TOOL) -h

regenerate: 
	@echo "GPERF: $(GPERF)"
	-$(GPERF) -h

regenerate2: lib/aliases.h lib/canonical.h lib/canonical_local.h
	@echo "Utilities copied to $(DSTDIR)"
	-$(ICONV_TOOL) -h

forced-rebuild: clean all
	@echo "forced-rebuild"

clean:
	@echo "clean"
	-rm -f $(DSTDIR)*.exe


lib/aliases.h lib/canonical.h lib/canonical_local.h : lib/encodings.def lib/encodings_local.def lib/genaliases.c
	$(ICONV_TOOL) genaliases lib/aliases.gperf canonical.sh canonical_local.sh
	dos2unix lib/aliases.gperf
	$(GPERF) -m 10 lib/aliases.gperf > tmp.h
	sed -e 's/^\(const struct alias \)/static \1/' < tmp.h > lib/aliases.h
	sh canonical.sh > lib/canonical.h
	sh canonical_local.sh > lib/canonical_local.h
#	$(RM) tmp.h canonical.sh canonical_local.sh

lib/aliases_sysaix.h lib/canonical_sysaix.h lib/canonical_local_sysaix.h : lib/encodings.def lib/encodings_local.def lib/genaliases.c
	$(CC) $(CFLAGS) -DUSE_AIX_ALIASES lib/genaliases.c -o genaliases
	./genaliases lib/aliases_sysaix.gperf canonical.sh canonical_local.sh
	dos2unix lib/aliases_sysaix.gperf
	$(RM) genaliases
	$(GPERF) -m 10 lib/aliases_sysaix.gperf > tmp.h
	sed -e 's/^\(const struct alias \)/static \1/' < tmp.h > lib/aliases_sysaix.h
	sh canonical.sh > lib/canonical_sysaix.h
	sh canonical_local.sh > lib/canonical_local_sysaix.h
	$(RM) tmp.h canonical.sh canonical_local.sh

lib/aliases_syshpux.h lib/canonical_syshpux.h lib/canonical_local_syshpux.h : lib/encodings.def lib/encodings_local.def lib/genaliases.c
	$(CC) $(CFLAGS) -DUSE_HPUX_ALIASES lib/genaliases.c -o genaliases
	./genaliases lib/aliases_syshpux.gperf canonical.sh canonical_local.sh
	dos2unix lib/aliases_syshpux.gperf
	$(RM) genaliases
	$(GPERF) -m 10 lib/aliases_syshpux.gperf > tmp.h
	sed -e 's/^\(const struct alias \)/static \1/' < tmp.h > lib/aliases_syshpux.h
	sh canonical.sh > lib/canonical_syshpux.h
	sh canonical_local.sh > lib/canonical_local_syshpux.h
	$(RM) tmp.h canonical.sh canonical_local.sh

lib/aliases_sysosf1.h lib/canonical_sysosf1.h lib/canonical_local_sysosf1.h : lib/encodings.def lib/encodings_local.def lib/genaliases.c
	$(CC) $(CFLAGS) -DUSE_OSF1_ALIASES lib/genaliases.c -o genaliases
	./genaliases lib/aliases_sysosf1.gperf canonical.sh canonical_local.sh
	dos2unix lib/aliases_sysosf1.gperf
	$(RM) genaliases
	$(GPERF) -m 10 lib/aliases_sysosf1.gperf > tmp.h
	sed -e 's/^\(const struct alias \)/static \1/' < tmp.h > lib/aliases_sysosf1.h
	sh canonical.sh > lib/canonical_sysosf1.h
	sh canonical_local.sh > lib/canonical_local_sysosf1.h
	$(RM) tmp.h canonical.sh canonical_local.sh

lib/aliases_syssolaris.h lib/canonical_syssolaris.h lib/canonical_local_syssolaris.h : lib/encodings.def lib/encodings_local.def lib/genaliases.c
	$(CC) $(CFLAGS) -DUSE_SOLARIS_ALIASES lib/genaliases.c -o genaliases
	./genaliases lib/aliases_syssolaris.gperf canonical.sh canonical_local.sh
	dos2unix lib/aliases_syssolaris.gperf
	$(RM) genaliases
	$(GPERF) -m 10 lib/aliases_syssolaris.gperf > tmp.h
	sed -e 's/^\(const struct alias \)/static \1/' < tmp.h > lib/aliases_syssolaris.h
	sh canonical.sh > lib/canonical_syssolaris.h
	sh canonical_local.sh > lib/canonical_local_syssolaris.h
	$(RM) tmp.h canonical.sh canonical_local.sh

lib/aliases_aix.h lib/canonical_aix.h : lib/encodings_aix.def lib/genaliases2.c
	$(CC) $(CFLAGS) -DUSE_AIX lib/genaliases2.c -o genaliases
	./genaliases aix lib/aliases_aix.h lib/canonical_aix.h
	$(RM) genaliases

lib/aliases_aix_sysaix.h lib/canonical_aix_sysaix.h : lib/encodings_aix.def lib/genaliases2.c
	$(CC) $(CFLAGS) -DUSE_AIX -DUSE_AIX_ALIASES lib/genaliases2.c -o genaliases
	./genaliases aix lib/aliases_aix_sysaix.h lib/canonical_aix_sysaix.h
	$(RM) genaliases

lib/aliases_osf1.h lib/canonical_osf1.h : lib/encodings_osf1.def lib/genaliases2.c
	$(CC) $(CFLAGS) -DUSE_OSF1 lib/genaliases2.c -o genaliases
	./genaliases osf1 lib/aliases_osf1.h lib/canonical_osf1.h
	$(RM) genaliases

lib/aliases_osf1_sysosf1.h lib/canonical_osf1_sysosf1.h : lib/encodings_osf1.def lib/genaliases2.c
	$(CC) $(CFLAGS) -DUSE_OSF1 -DUSE_OSF1_ALIASES lib/genaliases2.c -o genaliases
	./genaliases osf1 lib/aliases_osf1_sysosf1.h lib/canonical_osf1_sysosf1.h
	$(RM) genaliases

lib/aliases_dos.h lib/canonical_dos.h : lib/encodings_dos.def lib/genaliases2.c
	$(CC) $(CFLAGS) -DUSE_DOS lib/genaliases2.c -o genaliases
	./genaliases dos lib/aliases_dos.h lib/canonical_dos.h
	$(RM) genaliases

lib/aliases_zos.h lib/canonical_zos.h : lib/encodings_zos.def lib/genaliases2.c
	$(CC) $(CFLAGS) -DUSE_ZOS lib/genaliases2.c -o genaliases
	./genaliases zos lib/aliases_zos.h lib/canonical_zos.h
	$(RM) genaliases

lib/aliases_extra.h lib/canonical_extra.h : lib/encodings_extra.def lib/genaliases2.c
	$(CC) $(CFLAGS) -DUSE_EXTRA lib/genaliases2.c -o genaliases
	./genaliases extra lib/aliases_extra.h lib/canonical_extra.h
	$(RM) genaliases


lib/flags.h : lib/encodings.def lib/encodings_aix.def lib/encodings_osf1.def lib/encodings_dos.def lib/encodings_zos.def lib/encodings_extra.def lib/converters.h lib/genflags.c
	$(CC) $(CFLAGS) lib/genflags.c -o genflags
	./genflags > lib/flags.h
	$(RM) genflags


lib/translit.h : lib/translit.def lib/gentranslit.c
	$(CC) $(CFLAGS) lib/gentranslit.c -o gentranslit
	./gentranslit < lib/translit.def > lib/translit.h
	$(RM) gentranslit


