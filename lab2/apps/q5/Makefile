default:
	cd makeprocs; make
	cd so4_injection; make
	cd h2o_injection; make
	cd h2o_break; make
	cd so4_break; make
	cd h2so4_form; make

clean:
	cd makeprocs; make clean
	cd so4_injection; make clean
	cd h2o_injection; make clean
	cd h2o_break; make clean
	cd so4_break; make clean
	cd h2so4_form; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 6 3; ee469_fixterminal
