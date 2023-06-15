# -*- Mode: makefile -*-
# Copyright (c) 1994-1996 Massachusetts Institute of Technology
# Copyright (c) 1996-1999 Matthew Wall
# -----------------------------------------------------------------------------

include ../makevars
include makefile.sources

INC_DIRS= -I..

$(LIB): $(OBJS) 
	$(PRELINK)
	$(AR) $(LIB) $?
	@echo $(LIB) is now up-to-date
.C.a:;

clean:
	$(RM) $(LIB)
	$(RM) *.o *~ *.bak core ii_files ptrepository
	$(RM) vc60.* ga.lib

install:: $(LIB)
	$(INSTALL) $(LIB) $(LIB_DEST_DIR)
	$(RM) $(HDR_DEST_DIR)/ga; $(MKDIR) $(HDR_DEST_DIR)/ga
	$(CP) $(HDRS) $(TMPL_SRCS) ../VERSION $(HDR_DEST_DIR)/ga

remove:
	$(RM) $(LIB_DEST_DIR)/$(LIB) $(HDR_DEST_DIR)/ga

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.

garandom.o: ../ga/garandom.h /usr/include/stdlib.h /usr/include/features.h
garandom.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
garandom.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
garandom.o: /usr/include/sys/types.h /usr/include/bits/types.h
garandom.o: /usr/include/time.h /usr/include/endian.h
garandom.o: /usr/include/bits/endian.h /usr/include/sys/select.h
garandom.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
garandom.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
garandom.o: ../ga/gatypes.h ../ga/gaconfig.h /usr/include/math.h
garandom.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
garandom.o: /usr/include/bits/mathcalls.h
garandom.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
garandom.o: /usr/include/string.h /usr/include/unistd.h
garandom.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
garandom.o: /usr/include/getopt.h
gaerror.o: /usr/include/string.h /usr/include/features.h
gaerror.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
gaerror.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
gaerror.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/gaconfig.h
gaerror.o: /usr/include/stdio.h
gaerror.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
gaerror.o: /usr/include/bits/types.h /usr/include/libio.h
gaerror.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GAParameter.o: /usr/include/stdlib.h /usr/include/features.h
GAParameter.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GAParameter.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAParameter.o: /usr/include/sys/types.h /usr/include/bits/types.h
GAParameter.o: /usr/include/time.h /usr/include/endian.h
GAParameter.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GAParameter.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GAParameter.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GAParameter.o: /usr/include/string.h /usr/include/ctype.h ../ga/gaconfig.h
GAParameter.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/GAParameter.h
GAStatistics.o: /usr/include/string.h /usr/include/features.h
GAStatistics.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GAStatistics.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAStatistics.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/gaconfig.h
GAStatistics.o: ../ga/GAStatistics.h ../ga/GAGenome.h /usr/include/stdlib.h
GAStatistics.o: /usr/include/sys/types.h /usr/include/bits/types.h
GAStatistics.o: /usr/include/time.h /usr/include/endian.h
GAStatistics.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GAStatistics.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GAStatistics.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GAStatistics.o: ../ga/gaid.h ../ga/GAEvalData.h ../ga/GAPopulation.h
GAStatistics.o: ../ga/GASelector.h ../ga/GAScaling.h
GABaseGA.o: /usr/include/stdio.h /usr/include/features.h
GABaseGA.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GABaseGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GABaseGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GABaseGA.o: /usr/include/bits/types.h /usr/include/libio.h
GABaseGA.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GABaseGA.o: /usr/include/string.h ../ga/GABaseGA.h ../ga/gaconfig.h
GABaseGA.o: ../ga/gaid.h ../ga/GAParameter.h ../ga/gatypes.h
GABaseGA.o: ../ga/GAStatistics.h ../ga/GAGenome.h /usr/include/stdlib.h
GABaseGA.o: /usr/include/sys/types.h /usr/include/time.h
GABaseGA.o: /usr/include/endian.h /usr/include/bits/endian.h
GABaseGA.o: /usr/include/sys/select.h /usr/include/bits/select.h
GABaseGA.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
GABaseGA.o: /usr/include/alloca.h ../ga/gaerror.h ../ga/GAEvalData.h
GABaseGA.o: ../ga/GAPopulation.h ../ga/GASelector.h ../ga/GAScaling.h
GABaseGA.o: ../ga/garandom.h ../ga/gaversion.h
GASStateGA.o: ../ga/GASStateGA.h ../ga/GABaseGA.h /usr/include/stdio.h
GASStateGA.o: /usr/include/features.h /usr/include/sys/cdefs.h
GASStateGA.o: /usr/include/gnu/stubs.h
GASStateGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GASStateGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GASStateGA.o: /usr/include/bits/types.h /usr/include/libio.h
GASStateGA.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GASStateGA.o: /usr/include/string.h ../ga/gaconfig.h ../ga/gaid.h
GASStateGA.o: ../ga/GAParameter.h ../ga/gatypes.h ../ga/GAStatistics.h
GASStateGA.o: ../ga/GAGenome.h /usr/include/stdlib.h /usr/include/sys/types.h
GASStateGA.o: /usr/include/time.h /usr/include/endian.h
GASStateGA.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GASStateGA.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GASStateGA.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GASStateGA.o: ../ga/gaerror.h ../ga/GAEvalData.h ../ga/GAPopulation.h
GASStateGA.o: ../ga/GASelector.h ../ga/GAScaling.h ../ga/garandom.h
GASimpleGA.o: ../ga/GASimpleGA.h ../ga/GABaseGA.h /usr/include/stdio.h
GASimpleGA.o: /usr/include/features.h /usr/include/sys/cdefs.h
GASimpleGA.o: /usr/include/gnu/stubs.h
GASimpleGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GASimpleGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GASimpleGA.o: /usr/include/bits/types.h /usr/include/libio.h
GASimpleGA.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GASimpleGA.o: /usr/include/string.h ../ga/gaconfig.h ../ga/gaid.h
GASimpleGA.o: ../ga/GAParameter.h ../ga/gatypes.h ../ga/GAStatistics.h
GASimpleGA.o: ../ga/GAGenome.h /usr/include/stdlib.h /usr/include/sys/types.h
GASimpleGA.o: /usr/include/time.h /usr/include/endian.h
GASimpleGA.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GASimpleGA.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GASimpleGA.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GASimpleGA.o: ../ga/gaerror.h ../ga/GAEvalData.h ../ga/GAPopulation.h
GASimpleGA.o: ../ga/GASelector.h ../ga/GAScaling.h ../ga/garandom.h
GAIncGA.o: ../ga/GAIncGA.h ../ga/GABaseGA.h /usr/include/stdio.h
GAIncGA.o: /usr/include/features.h /usr/include/sys/cdefs.h
GAIncGA.o: /usr/include/gnu/stubs.h
GAIncGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAIncGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GAIncGA.o: /usr/include/bits/types.h /usr/include/libio.h
GAIncGA.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GAIncGA.o: /usr/include/string.h ../ga/gaconfig.h ../ga/gaid.h
GAIncGA.o: ../ga/GAParameter.h ../ga/gatypes.h ../ga/GAStatistics.h
GAIncGA.o: ../ga/GAGenome.h /usr/include/stdlib.h /usr/include/sys/types.h
GAIncGA.o: /usr/include/time.h /usr/include/endian.h
GAIncGA.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GAIncGA.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GAIncGA.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h ../ga/gaerror.h
GAIncGA.o: ../ga/GAEvalData.h ../ga/GAPopulation.h ../ga/GASelector.h
GAIncGA.o: ../ga/GAScaling.h ../ga/garandom.h
GADemeGA.o: ../ga/garandom.h /usr/include/stdlib.h /usr/include/features.h
GADemeGA.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GADemeGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GADemeGA.o: /usr/include/sys/types.h /usr/include/bits/types.h
GADemeGA.o: /usr/include/time.h /usr/include/endian.h
GADemeGA.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GADemeGA.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GADemeGA.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GADemeGA.o: ../ga/gatypes.h ../ga/gaconfig.h ../ga/GADemeGA.h
GADemeGA.o: ../ga/GABaseGA.h /usr/include/stdio.h
GADemeGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GADemeGA.o: /usr/include/libio.h /usr/include/_G_config.h
GADemeGA.o: /usr/include/bits/stdio_lim.h /usr/include/string.h ../ga/gaid.h
GADemeGA.o: ../ga/GAParameter.h ../ga/GAStatistics.h ../ga/GAGenome.h
GADemeGA.o: ../ga/gaerror.h ../ga/GAEvalData.h ../ga/GAPopulation.h
GADemeGA.o: ../ga/GASelector.h ../ga/GAScaling.h
GADCrowdingGA.o: GADCrowdingGA.h ../ga/GABaseGA.h /usr/include/stdio.h
GADCrowdingGA.o: /usr/include/features.h /usr/include/sys/cdefs.h
GADCrowdingGA.o: /usr/include/gnu/stubs.h
GADCrowdingGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GADCrowdingGA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GADCrowdingGA.o: /usr/include/bits/types.h /usr/include/libio.h
GADCrowdingGA.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GADCrowdingGA.o: /usr/include/string.h ../ga/gaconfig.h ../ga/gaid.h
GADCrowdingGA.o: ../ga/GAParameter.h ../ga/gatypes.h ../ga/GAStatistics.h
GADCrowdingGA.o: ../ga/GAGenome.h /usr/include/stdlib.h
GADCrowdingGA.o: /usr/include/sys/types.h /usr/include/time.h
GADCrowdingGA.o: /usr/include/endian.h /usr/include/bits/endian.h
GADCrowdingGA.o: /usr/include/sys/select.h /usr/include/bits/select.h
GADCrowdingGA.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
GADCrowdingGA.o: /usr/include/alloca.h ../ga/gaerror.h ../ga/GAEvalData.h
GADCrowdingGA.o: ../ga/GAPopulation.h ../ga/GASelector.h ../ga/GAScaling.h
GADCrowdingGA.o: GAList.h ../ga/GAListBASE.h ../ga/GANode.h ../ga/GAList.C
GADCrowdingGA.o: ../ga/GAList.h garandom.h
GASelector.o: ../ga/gaconfig.h ../ga/GAPopulation.h ../ga/gaid.h
GASelector.o: ../ga/GASelector.h /usr/include/string.h
GASelector.o: /usr/include/features.h /usr/include/sys/cdefs.h
GASelector.o: /usr/include/gnu/stubs.h
GASelector.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GASelector.o: ../ga/GAScaling.h ../ga/gatypes.h ../ga/GAGenome.h
GASelector.o: /usr/include/stdlib.h /usr/include/sys/types.h
GASelector.o: /usr/include/bits/types.h /usr/include/time.h
GASelector.o: /usr/include/endian.h /usr/include/bits/endian.h
GASelector.o: /usr/include/sys/select.h /usr/include/bits/select.h
GASelector.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
GASelector.o: /usr/include/alloca.h ../ga/gaerror.h ../ga/GAEvalData.h
GASelector.o: ../ga/garandom.h
GAScaling.o: /usr/include/math.h /usr/include/features.h
GAScaling.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GAScaling.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
GAScaling.o: /usr/include/bits/mathcalls.h
GAScaling.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
GAScaling.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/gaconfig.h
GAScaling.o: ../ga/GAScaling.h /usr/include/string.h
GAScaling.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAScaling.o: ../ga/gaid.h ../ga/GAGenome.h /usr/include/stdlib.h
GAScaling.o: /usr/include/sys/types.h /usr/include/bits/types.h
GAScaling.o: /usr/include/time.h /usr/include/endian.h
GAScaling.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GAScaling.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GAScaling.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GAScaling.o: ../ga/GAEvalData.h ../ga/GAPopulation.h ../ga/GASelector.h
GAScaling.o: ../ga/GABaseGA.h /usr/include/stdio.h
GAScaling.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GAScaling.o: /usr/include/libio.h /usr/include/_G_config.h
GAScaling.o: /usr/include/bits/stdio_lim.h ../ga/GAParameter.h
GAScaling.o: ../ga/GAStatistics.h
GAPopulation.o: /usr/include/string.h /usr/include/features.h
GAPopulation.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GAPopulation.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAPopulation.o: /usr/include/math.h /usr/include/bits/huge_val.h
GAPopulation.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
GAPopulation.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
GAPopulation.o: ../ga/GAPopulation.h ../ga/gaconfig.h ../ga/gaid.h
GAPopulation.o: ../ga/GASelector.h ../ga/GAScaling.h ../ga/gatypes.h
GAPopulation.o: ../ga/GAGenome.h /usr/include/stdlib.h
GAPopulation.o: /usr/include/sys/types.h /usr/include/bits/types.h
GAPopulation.o: /usr/include/time.h /usr/include/endian.h
GAPopulation.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GAPopulation.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GAPopulation.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GAPopulation.o: ../ga/gaerror.h ../ga/GAEvalData.h ../ga/garandom.h
GAPopulation.o: ../ga/GABaseGA.h /usr/include/stdio.h
GAPopulation.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GAPopulation.o: /usr/include/libio.h /usr/include/_G_config.h
GAPopulation.o: /usr/include/bits/stdio_lim.h ../ga/GAParameter.h
GAPopulation.o: ../ga/GAStatistics.h
GAGenome.o: ../ga/GAGenome.h /usr/include/stdlib.h /usr/include/features.h
GAGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GAGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAGenome.o: /usr/include/sys/types.h /usr/include/bits/types.h
GAGenome.o: /usr/include/time.h /usr/include/endian.h
GAGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GAGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GAGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GAGenome.o: /usr/include/string.h ../ga/gaid.h ../ga/gaconfig.h
GAGenome.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/GAEvalData.h
GABinStr.o: /usr/include/stdio.h /usr/include/features.h
GABinStr.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GABinStr.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GABinStr.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GABinStr.o: /usr/include/bits/types.h /usr/include/libio.h
GABinStr.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GABinStr.o: /usr/include/stdlib.h /usr/include/sys/types.h
GABinStr.o: /usr/include/time.h /usr/include/endian.h
GABinStr.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GABinStr.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GABinStr.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GABinStr.o: /usr/include/string.h ../ga/gaerror.h ../ga/gatypes.h
GABinStr.o: ../ga/gaconfig.h ../ga/GABinStr.h ../ga/garandom.h
gabincvt.o: /usr/include/stdio.h /usr/include/features.h
gabincvt.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
gabincvt.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
gabincvt.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
gabincvt.o: /usr/include/bits/types.h /usr/include/libio.h
gabincvt.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
gabincvt.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
gabincvt.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
gabincvt.o: /usr/include/bits/posix2_lim.h /usr/include/string.h
gabincvt.o: ../ga/gaconfig.h ../ga/gaerror.h ../ga/gatypes.h ../ga/gabincvt.h
GAAllele.o: /usr/include/string.h /usr/include/features.h
GAAllele.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GAAllele.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAAllele.o: ../ga/GAAllele.h ../ga/gaconfig.h ../ga/garandom.h
GAAllele.o: /usr/include/stdlib.h /usr/include/sys/types.h
GAAllele.o: /usr/include/bits/types.h /usr/include/time.h
GAAllele.o: /usr/include/endian.h /usr/include/bits/endian.h
GAAllele.o: /usr/include/sys/select.h /usr/include/bits/select.h
GAAllele.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
GAAllele.o: /usr/include/alloca.h ../ga/gatypes.h ../ga/GAAllele.C
GAAllele.o: ../ga/gaerror.h
GAStringGenome.o: ../ga/GAStringGenome.h ../ga/GAAllele.h
GAStringGenome.o: /usr/include/string.h /usr/include/features.h
GAStringGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GAStringGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAStringGenome.o: ../ga/gaconfig.h ../ga/garandom.h /usr/include/stdlib.h
GAStringGenome.o: /usr/include/sys/types.h /usr/include/bits/types.h
GAStringGenome.o: /usr/include/time.h /usr/include/endian.h
GAStringGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GAStringGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GAStringGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GAStringGenome.o: ../ga/gatypes.h ../ga/GAAllele.C ../ga/GA1DArrayGenome.h
GAStringGenome.o: ../ga/GAArray.h ../ga/GAGenome.h ../ga/gaid.h
GAStringGenome.o: ../ga/gaerror.h ../ga/GAEvalData.h ../ga/GA1DArrayGenome.C
GAStringGenome.o: /usr/include/stdio.h
GAStringGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GAStringGenome.o: /usr/include/libio.h /usr/include/_G_config.h
GAStringGenome.o: /usr/include/bits/stdio_lim.h ../ga/GAMask.h
GARealGenome.o: ../ga/GARealGenome.h ../ga/GAAllele.h /usr/include/string.h
GARealGenome.o: /usr/include/features.h /usr/include/sys/cdefs.h
GARealGenome.o: /usr/include/gnu/stubs.h
GARealGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GARealGenome.o: ../ga/gaconfig.h ../ga/garandom.h /usr/include/stdlib.h
GARealGenome.o: /usr/include/sys/types.h /usr/include/bits/types.h
GARealGenome.o: /usr/include/time.h /usr/include/endian.h
GARealGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GARealGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GARealGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GARealGenome.o: ../ga/gatypes.h ../ga/GAAllele.C ../ga/GA1DArrayGenome.h
GARealGenome.o: ../ga/GAArray.h ../ga/GAGenome.h ../ga/gaid.h ../ga/gaerror.h
GARealGenome.o: ../ga/GAEvalData.h ../ga/GA1DArrayGenome.C
GARealGenome.o: /usr/include/stdio.h
GARealGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GARealGenome.o: /usr/include/libio.h /usr/include/_G_config.h
GARealGenome.o: /usr/include/bits/stdio_lim.h ../ga/GAMask.h
GA1DBinStrGenome.o: /usr/include/stdio.h /usr/include/features.h
GA1DBinStrGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GA1DBinStrGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GA1DBinStrGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GA1DBinStrGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GA1DBinStrGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GA1DBinStrGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GA1DBinStrGenome.o: /usr/include/time.h /usr/include/endian.h
GA1DBinStrGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GA1DBinStrGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GA1DBinStrGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GA1DBinStrGenome.o: /usr/include/ctype.h /usr/include/string.h
GA1DBinStrGenome.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/gaconfig.h
GA1DBinStrGenome.o: ../ga/garandom.h ../ga/GA1DBinStrGenome.h
GA1DBinStrGenome.o: ../ga/GABinStr.h ../ga/GAGenome.h ../ga/gaid.h
GA1DBinStrGenome.o: ../ga/GAEvalData.h ../ga/GAMask.h
GA2DBinStrGenome.o: /usr/include/stdio.h /usr/include/features.h
GA2DBinStrGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GA2DBinStrGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GA2DBinStrGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GA2DBinStrGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GA2DBinStrGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GA2DBinStrGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GA2DBinStrGenome.o: /usr/include/time.h /usr/include/endian.h
GA2DBinStrGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GA2DBinStrGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GA2DBinStrGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GA2DBinStrGenome.o: /usr/include/ctype.h /usr/include/string.h
GA2DBinStrGenome.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/gaconfig.h
GA2DBinStrGenome.o: ../ga/garandom.h ../ga/GA2DBinStrGenome.h
GA2DBinStrGenome.o: ../ga/GABinStr.h ../ga/GAGenome.h ../ga/gaid.h
GA2DBinStrGenome.o: ../ga/GAEvalData.h ../ga/GAMask.h
GA3DBinStrGenome.o: /usr/include/stdio.h /usr/include/features.h
GA3DBinStrGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GA3DBinStrGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GA3DBinStrGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GA3DBinStrGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GA3DBinStrGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GA3DBinStrGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GA3DBinStrGenome.o: /usr/include/time.h /usr/include/endian.h
GA3DBinStrGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GA3DBinStrGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GA3DBinStrGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GA3DBinStrGenome.o: /usr/include/ctype.h /usr/include/string.h
GA3DBinStrGenome.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/gaconfig.h
GA3DBinStrGenome.o: ../ga/garandom.h ../ga/GA3DBinStrGenome.h
GA3DBinStrGenome.o: ../ga/GABinStr.h ../ga/GAGenome.h ../ga/gaid.h
GA3DBinStrGenome.o: ../ga/GAEvalData.h ../ga/GAMask.h
GABin2DecGenome.o: /usr/include/stdio.h /usr/include/features.h
GABin2DecGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GABin2DecGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GABin2DecGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GABin2DecGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GABin2DecGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GABin2DecGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GABin2DecGenome.o: /usr/include/time.h /usr/include/endian.h
GABin2DecGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GABin2DecGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GABin2DecGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GABin2DecGenome.o: /usr/include/ctype.h /usr/include/string.h ../ga/gaerror.h
GABin2DecGenome.o: ../ga/gatypes.h ../ga/gaconfig.h ../ga/GABin2DecGenome.h
GABin2DecGenome.o: ../ga/GA1DBinStrGenome.h ../ga/GABinStr.h ../ga/garandom.h
GABin2DecGenome.o: ../ga/GAGenome.h ../ga/gaid.h ../ga/GAEvalData.h
GABin2DecGenome.o: ../ga/gabincvt.h
GA1DArrayGenome.o: /usr/include/stdio.h /usr/include/features.h
GA1DArrayGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GA1DArrayGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GA1DArrayGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GA1DArrayGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GA1DArrayGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GA1DArrayGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GA1DArrayGenome.o: /usr/include/time.h /usr/include/endian.h
GA1DArrayGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GA1DArrayGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GA1DArrayGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GA1DArrayGenome.o: /usr/include/string.h ../ga/garandom.h ../ga/gatypes.h
GA1DArrayGenome.o: ../ga/gaconfig.h ../ga/GA1DArrayGenome.h ../ga/GAArray.h
GA1DArrayGenome.o: ../ga/GAGenome.h ../ga/gaid.h ../ga/gaerror.h
GA1DArrayGenome.o: ../ga/GAEvalData.h ../ga/GAAllele.h ../ga/GAAllele.C
GA1DArrayGenome.o: ../ga/GA1DArrayGenome.C ../ga/GAMask.h
GA2DArrayGenome.o: /usr/include/stdio.h /usr/include/features.h
GA2DArrayGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GA2DArrayGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GA2DArrayGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GA2DArrayGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GA2DArrayGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GA2DArrayGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GA2DArrayGenome.o: /usr/include/time.h /usr/include/endian.h
GA2DArrayGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GA2DArrayGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GA2DArrayGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GA2DArrayGenome.o: /usr/include/string.h ../ga/garandom.h ../ga/gatypes.h
GA2DArrayGenome.o: ../ga/gaconfig.h ../ga/GA2DArrayGenome.h ../ga/GAArray.h
GA2DArrayGenome.o: ../ga/GAGenome.h ../ga/gaid.h ../ga/gaerror.h
GA2DArrayGenome.o: ../ga/GAEvalData.h ../ga/GAAllele.h ../ga/GAAllele.C
GA2DArrayGenome.o: ../ga/GA2DArrayGenome.C ../ga/GAMask.h
GA3DArrayGenome.o: /usr/include/stdio.h /usr/include/features.h
GA3DArrayGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GA3DArrayGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GA3DArrayGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GA3DArrayGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GA3DArrayGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GA3DArrayGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GA3DArrayGenome.o: /usr/include/time.h /usr/include/endian.h
GA3DArrayGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GA3DArrayGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GA3DArrayGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GA3DArrayGenome.o: /usr/include/string.h ../ga/garandom.h ../ga/gatypes.h
GA3DArrayGenome.o: ../ga/gaconfig.h ../ga/GA3DArrayGenome.h ../ga/GAArray.h
GA3DArrayGenome.o: ../ga/GAGenome.h ../ga/gaid.h ../ga/gaerror.h
GA3DArrayGenome.o: ../ga/GAEvalData.h ../ga/GAAllele.h ../ga/GAAllele.C
GA3DArrayGenome.o: ../ga/GA3DArrayGenome.C ../ga/GAMask.h
GATreeBASE.o: ../ga/GATreeBASE.h ../ga/GANode.h ../ga/gaconfig.h
GATreeBASE.o: ../ga/gaerror.h ../ga/gatypes.h
GATree.o: ../ga/GATree.h ../ga/gaerror.h ../ga/gatypes.h ../ga/gaconfig.h
GATree.o: ../ga/GATreeBASE.h ../ga/GANode.h ../ga/GATree.C
GATreeGenome.o: /usr/include/stdio.h /usr/include/features.h
GATreeGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GATreeGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GATreeGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GATreeGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GATreeGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GATreeGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GATreeGenome.o: /usr/include/time.h /usr/include/endian.h
GATreeGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GATreeGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GATreeGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GATreeGenome.o: ../ga/GATreeGenome.h ../ga/GATree.h ../ga/gaerror.h
GATreeGenome.o: ../ga/gatypes.h ../ga/gaconfig.h ../ga/GATreeBASE.h
GATreeGenome.o: ../ga/GANode.h ../ga/GATree.C ../ga/GAGenome.h
GATreeGenome.o: /usr/include/string.h ../ga/gaid.h ../ga/GAEvalData.h
GATreeGenome.o: ../ga/GATreeGenome.C
GAListBASE.o: ../ga/GAListBASE.h ../ga/GANode.h ../ga/gaconfig.h
GAListBASE.o: ../ga/gaerror.h ../ga/gatypes.h
GAList.o: ../ga/GAList.h
GAListGenome.o: /usr/include/stdio.h /usr/include/features.h
GAListGenome.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
GAListGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GAListGenome.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GAListGenome.o: /usr/include/bits/types.h /usr/include/libio.h
GAListGenome.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
GAListGenome.o: /usr/include/stdlib.h /usr/include/sys/types.h
GAListGenome.o: /usr/include/time.h /usr/include/endian.h
GAListGenome.o: /usr/include/bits/endian.h /usr/include/sys/select.h
GAListGenome.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
GAListGenome.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h
GAListGenome.o: ../ga/GAListGenome.h ../ga/GAList.h ../ga/GAGenome.h
GAListGenome.o: /usr/include/string.h ../ga/gaid.h ../ga/gaconfig.h
GAListGenome.o: ../ga/gaerror.h ../ga/gatypes.h ../ga/GAEvalData.h
GAListGenome.o: ../ga/GAListGenome.C ../ga/GAMask.h
