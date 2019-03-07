ifndef VERBOSE
VERBOSE = false
endif
ifeq ($(VERBOSE), false)
.SILENT:
endif

Complier = gcc
Flags = -g -std=c99 -fopenmp -o
Libraries = -lm -lpng

build:
	make build-student; make build-naive;

build-student: student/ced.c student/student.c student/ced.h student/student.h
	$(Complier) $(Flags) student/ced student/ced.c student/student.c $(Libraries) || (echo "[ERROR]: Could not compile the student code!";)

build-naive: naive/ced.c naive/student.c naive/ced.h naive/student.h
	$(Complier) $(Flags) naive/ced naive/ced.c naive/student.c $(Libraries) || (echo "[ERROR]: Could not compile the naive code!";)

build-correctness: check-correctness.c
	$(Complier) $(Flags) check-correctness check-correctness.c $(Libraries) || (echo "[ERROR]: Could not compile the check-correctness code!";)

clean:
	make clean-student; make clean-naive

clean-student:
	rm -r student/out; mkdir student/out; rm student/ced;

clean-naive:
	rm -r naive/out; mkdir naive/out; rm naive/ced;

clean-correctness:
	rm check-correctness;

batch:
	echo -ne "Cleaning..."\\r; make clean; echo "Cleaning...Done!"; echo -ne "Building..."\\r; make build; make build-correctness; echo -e "Building...Done!\nBatch testing your project..."; ./run-test.py batch; make clean-correctness;

view:
	echo -ne "Cleaning..."\\r; make clean-student; echo "Cleaning...Done!"; echo -ne "Building..."\\r; make build-student; echo  -e "Building...Done!\nViewing..."; ./run-test.py view;

correctness:
	echo -ne "Cleaning..."\\r; make clean-student; echo "Cleaning...Done!"; echo -ne "Building..."\\r; make build-student; make build-correctness; echo  -e "Building...Done!\nTesting the correctness of your project..."; ./run-test.py correctness; make clean-correctness;

valgrind:
	make clean-student; make build-student; cd student; valgrind --leak-check=yes ./ced  ../input/valve.png ../input/weaver.png ../input/bigbrain.png

cpu:
	chmod u+x cpu_usage.sh
	./cpu_usage.sh;

.PHONY: build build-student build-naive build-correctness clean clean-student clean-naive clean-correctness batch view correctness valgrind
