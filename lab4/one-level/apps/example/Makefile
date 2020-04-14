default:
	cd makeprocs; make
	cd test1; make
	cd test2; make
	cd test3; make
	cd test5; make

clean:
	cd makeprocs; make clean
	cd test1; make clean
	cd test2; make clean
	cd test3; make clean
	cd test5; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 5; ee469_fixterminal

