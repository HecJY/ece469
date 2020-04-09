default:
	cd makeprocs; make
	cd co_injection; make
	cd s2_injection; make
	cd co_break; make
	cd s2_break; make
	cd so4_form; make

clean:
	cd makeprocs; make clean
	cd co_injection; make clean
	cd s2_injection; make clean
	cd co_break; make clean
	cd s2_break; make clean
	cd so4_form; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 6; ee469_fixterminal
