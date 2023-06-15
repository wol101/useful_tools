# Makefile for dynamechs

all:
	cd dynamechs_3.0; make
	cd galib245/ga; make
	cd gibbon_sym; make
	cd gibbon_ga; make
	cd gibbon_sym_feedback; make
	cd gibbon_ga_feedback; make

clean:
	cd dynamechs_3.0; make clean
	cd galib245/ga; make clean
	cd gibbon_sym; make clean
	cd gibbon_ga; make clean
	cd gibbon_sym_feedback; make clean
	cd gibbon_ga_feedback; make clean
	rm -f *.o *~ *.bak core *.bck

depend:
	cd dynamechs_3.0/dm; make depend
	cd dynamechs_3.0/dmGL; make depend
	cd dynamechs_3.0/dmu; make depend
	cd galib245/ga; make depend
	cd gibbon_sym; make depend
	cd gibbon_ga; make depend
	cd gibbon_sym_feedback; make depend
	cd gibbon_ga_feedback; make depend
