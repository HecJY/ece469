default:
	cd makeprocs; make
	cd apps; make

clean:
	cd makeprocs; make clean
	cd apps; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj; ee469_fixterminal
