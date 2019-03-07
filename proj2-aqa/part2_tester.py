import re
import sys


trace_format = "riscvcode/{0}/{1}.trace"

max_num_instructions = 10000


def run_test(name):
    pass_bool = True
    # open trace 
    ref_trace = open(trace_format.format("ref", name), 'r')
    student_trace = open(trace_format.format("out", name), 'r')

    ref_registers = [0] * 32
    student_registers = [0] * 32
    for k in range(0, max_num_instructions):

        # read one set of registers
        for i in range(0,8):
            ref_line = ref_trace.readline()
            student_line = student_trace.readline()

            # handle invalid instructions
            if "Invalid" in student_line:
                print "ERROR: found an invalid instruction in the student trace file. Line: " + student_line
                return False

            # handle end of files
            student_finished = "exiting" in student_line or len(student_line) == 0
            ref_finished = "exiting" in ref_line or len(ref_line) == 0
            if student_finished and ref_finished:
                return pass_bool
            elif student_finished and not ref_finished:
                print "ERROR: student trace finished before reference trace"
                return False
            elif ref_finished and not student_finished:
                print "ERROR: reference trace finished before student trace"
                return False

            # remove r's and spaces
            ref_match = re.findall("r[^=]+=([0-9a-f]+)", ref_line)
            student_match = re.findall("r[^=]+=([0-9a-f]+)", student_line)
            if len(student_match) < 4:
                print "ERROR: could not parse line: " + student_line

            # reading registers
            for j in range(0,4):
                ref_val = int(ref_match[j], 16)
                student_val = int(student_match[j], 16)

                if student_val - student_registers[i*4+j] != ref_val - ref_registers[i*4+j] and ref_val != student_val:
                    pass_bool = False
                    print "ERROR: instruction {}, register {}. Expected: 0x{:08x}, Actual: 0x{:08x}".format(k, i*4+j, ref_val, student_val)

                ref_registers[i*4+j] = ref_val
                student_registers[i*4+j] = student_val

        ref_trace.readline() # newline
        student_trace.readline()

    print "ERROR: Should never reach here, possible infinite loop"
    return False


if __name__ == "__main__":
    test = sys.argv[1]
    print("")
    print "Starting {0} test".format(test)
    has_passed = run_test(test)
    if has_passed:
        print "{0} test has passed.".format(test)
    else:
        print "{0} test has failed.".format(test)
            
