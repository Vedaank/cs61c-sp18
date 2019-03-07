# Build file for CS61C MapReduce Lab
# You should not need to edit this file.

# This file requires GNU make and depends on paths on instruction machines.

####

## Variables

# Source files (java code). wildcard selects all files matching a pattern.
SOURCES = $(wildcard *.java)
# Output JAR file
TARGET = wc.jar
# Extra JARs to have on the classpath when compiling.
CLASSPATH = $(shell hadoop classpath)
# javac command to use
JAVAC = javac -g -deprecation -cp $(CLASSPATH)
# jar command to use
JAR = jar

## Make targets

# General form is target: dependencies (targets or files), followed by
# commands to run to build the target from the dependencies.

# Default target.
all: $(TARGET)

$(TARGET): classes $(SOURCES)
	$(JAVAC) -d classes $(SOURCES)
	$(JAR) cf $(TARGET) -C classes .

wordcount-small:
	rm -rf wc-out-wordcount-small
	spark-submit wordcount.py /home/ff/cs61c/data/billOfRights.txt.seq wc-out-wordcount-small

wordcount-medium:
	rm -rf wc-out-wordcount-medium
	spark-submit wordcount.py /home/ff/cs61c/data/complete-works-mark-twain.txt.seq wc-out-wordcount-medium

wordcount:
	rm -rf wc-out-wordcount
	spark-submit wordcount.py $(myinput) wc-out-wordcount

docwordcount-small:
	rm -rf wc-out-docwordcount-small
	spark-submit docwordcount.py /home/ff/cs61c/data/billOfRights.txt.seq wc-out-docwordcount-small

docwordcount-medium:
	rm -rf wc-out-docwordcount-medium
	spark-submit docwordcount.py /home/ff/cs61c/data/complete-works-mark-twain.txt.seq wc-out-docwordcount-medium

docwordcount:
	rm -rf wc-out-docwordcount
	spark-submit docwordcount.py $(myinput) wc-out-docwordcount


index-small:
	rm -rf wc-out-index-small
	spark-submit index.py /home/ff/cs61c/data/billOfRights.txt.seq wc-out-index-small

index-medium:
	rm -rf wc-out-index-medium
	spark-submit index.py /home/ff/cs61c/data/complete-works-mark-twain.txt.seq wc-out-index-medium

index:
	rm -rf wc-out-index
	spark-submit index.py $(myinput) wc-out-index


generate-input: $(TARGET)
	hadoop jar wc.jar Importer $(myinput)

classes:
	mkdir classes

clean:
	rm -rf classes $(TARGET)

destroy-all:
	rm -rf wc-out*
	rm -rf classes $(TARGET)

.PHONY: clean all
