
all: philphix 

test: philphix test.txt replace.txt
	touch output.txt
	rm output.txt
	@echo "================Running Program...================="
	cat test.txt | ./philphix replace.txt > output.txt
	@echo "================Program Finished!=================="
	@echo ""
	@echo "Difference between test output and reference output"
	@echo "---------------------------------------------------"
	@diff output.txt reference.txt
	@echo "-----------------------None!-----------------------"
	@echo

testgend: philphix gendtest.txt gendrep.txt
	touch output.txt
	rm output.txt
	@echo "================Running Program...================="
	cat gendtest.txt | ./philphix gendrep.txt > output.txt
	@echo "================Program Finished!=================="
	@echo ""
	@echo "Difference between test output and reference output"
	@echo "---------------------------------------------------"
	@diff output.txt gendref.txt
	@echo "-----------------------None!-----------------------"
	@echo

testautogen: philphix
	@echo "Making the test files!"
	@echo
	python3 gendict.py
	touch output.txt
	rm output.txt
	@echo "================Running Program...================="
	cat newtest.txt | ./philphix newrep.txt > output.txt
	@echo "================Program Finished!=================="
	@echo ""
	@echo "Difference between test output and reference output"
	@echo "---------------------------------------------------"
	@diff output.txt newref.txt
	@echo "-----------------------None!-----------------------"
	@echo

testnull: philphix
	@echo "Making the test files!"
	@echo
	@echo "A A" > null.txt
	@echo "================Running Program...================="
	cat philphix | ./philphix null.txt > newphilphix
	@echo "================Program Finished!=================="
	@echo ""
	@echo "Difference between test output and reference output"
	@echo "---------------------------------------------------"
	@diff philphix newphilphix
	@echo "-----------------------None!-----------------------"
	@echo rm -f null.txt
	@echo

testedge: philphix
	@echo "Making the test files!"
	@echo
	@echo "A The" > edgeDict.txt
	@echo "e o" >> edgeDict.txt
	@echo -n "A cute dog.E" > edgetest.txt
	@echo -n "The cute dog.o" > edgeref.txt
	@echo "================Running Program...================="
	cat edgetest.txt | ./philphix edgeDict.txt > edgeout.txt
	@echo "================Program Finished!=================="
	@echo ""
	@echo "Difference between test output and reference output"
	@echo "---------------------------------------------------"
	@diff edgeref.txt edgeout.txt
	@echo "-----------------------None!-----------------------"
	@rm -f edgeDict.txt edgeout.txt edgeref.txt edgetest.txt
	@echo
philphix: philphix.o hashtable.o
	gcc -g -Wall -o philphix philphix.o hashtable.o

philphix.o: philphix.c philphix.h hashtable.h
	gcc -g -c -Wall philphix.c

hashtable.o: hashtable.c hashtable.h
	gcc -g -c -Wall hashtable.c

clean:
	rm -f *.o philphix

testall: test testautogen testnull testedge