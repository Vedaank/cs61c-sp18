#!/usr/bin/env python3

import os
import os.path
import tempfile
import subprocess
import time
import signal
import re
import sys
import shutil

file_locations = os.path.expanduser(os.getcwd())
logisim_location = os.path.join(os.getcwd(),"logisim.jar")

create = 0
debug = False

if create:
  new = open('new.out', 'w')

class TestCase():
  """
      Runs specified circuit file and compares output against the provided reference trace file.
  """

  def __init__(self, circfile, tracefile, register_doc):
    self.circfile  = circfile
    self.tracefile = tracefile
    self.register_doc = register_doc

  def __call__(self):
    output = tempfile.TemporaryFile(mode='r+')
    try:
        stdinf = open('/dev/null')
    except Exception as e:
        if debug:
            print("/dev/null not found, attempting different dir..")
        try:
            stdinf = open('nul')
            if debug:
                print("nul dir works!")
        except Exception as e:
            print("The no nul directories. Program will most likely error now.")
    proc = subprocess.Popen(["java","-jar",logisim_location,"-tty","table",self.circfile],
                            stdin=stdinf,
                            stdout=subprocess.PIPE)
    try:
      reference = open(self.tracefile)
      passed = compare_unbounded(proc.stdout, reference)
    finally:
      try:
        os.kill(proc.pid,signal.SIGTERM)
      except Exception as e:
         if debug:
            print("Could not kill process! Perhaps it closed itself?")
    if passed:
      return (True, "Matched expected output")
    else:
      return (False, "Did not match expected output: " + self.register_doc)

def compare_unbounded(student_out, reference_out):
  passed = True
  while True:
    line1 = student_out.readline().rstrip().decode("utf-8", "namereplace")
    line2 = reference_out.readline().rstrip()
    if line2 == '':
      break
    if create:
      new.write(line1)
    m = re.match(line1, line2)
    if m == None or m.start() != 0 or m.end() != len(line2):
      passed = False
      print('Got:      ', line1)
      print('Expected: ', line2)
  return passed


def run_tests(tests):
  print("Testing files...")
  tests_passed = 0
  tests_failed = 0
  for description,test in tests:
    test_passed, reason = test()
    if test_passed:
      print("\tPASSED test: %s" % description)
      tests_passed += 1
    else:
      print("\tFAILED test: %s (%s)" % (description, reason))
      tests_failed += 1

  print("Passed %d/%d tests" % (tests_passed, (tests_passed + tests_failed)))

tests = [
  ("ALU add test",TestCase(os.path.join(file_locations,'alu-add.circ'), os.path.join(file_locations,'reference_output/alu-add.out'), "Reference columns are Test #, Equals Output, Result1")),
  ("RegFile insert test",
        TestCase(os.path.join(file_locations,'regfile-insert.circ'),
                 os.path.join(file_locations,'reference_output/regfile-insert.out'), "Reference columns are Test #, ra, sp, t0, t1, t2, s0, s1, a0, Read Data 1, Read Data 2")),
  ("RegFile zero test",
        TestCase(os.path.join(file_locations,'regfile-zero.circ'),
                 os.path.join(file_locations,'reference_output/regfile-zero.out'), "Reference columns are Test #, ra, sp, t0, t1, t2, s0, s1, a0, Read Data 1, Read Data 2")),
  ("ALU multiply test",TestCase(os.path.join(file_locations,'alu-mult.circ'), os.path.join(file_locations,'reference_output/alu-mult.out'), "Reference columns are Test #, Equals Output, Result1")),
]

if __name__ == '__main__':
  run_tests(tests)
