#!/usr/bin/env python

# This script must be run from within your proj5-xxx directory (the one that has memoizer.go).
# It depends on the sp18-proj5-knownBug package

import os
import glob
import re
import fileinput
import sys
from subprocess import call

# Replace the import path for the memoizer in the given goFilePath to the new import path
def replaceMemoizerImport(goFilePath, newImport):
    # Note that fileinput redirects stdout to the file. It lets you replace all
    # the lines in a file.
    gofile = fileinput.input(goFilePath, inplace=True)
    try: 
        # Find the import block
        for line in gofile:
            sys.stdout.write(line)
            if re.match("import \(", line):
                break

        # Search for the memoizer line and replace it
        origImport = ""
        for line in gofile:
            if re.match("^\s*memoizer.*", line) or re.match("^\s*\"bitbucket\..../*/proj5-.../.*", line):
                origImport = line
                print(newImport)
                break
            elif re.match("\)", line):
                #If we don't find the memoizer import in the import block, stop looking
                sys.stdout.write(line)
                break
            else:
                sys.stdout.write(line)

        # Output the rest of the file normally
        for line in gofile:
            sys.stdout.write(line)

    except:
        #If anything goes wrong, we need to not mess up the original file
        for line in gofile:
            sys.stdout.write(line)

    gofile.close()

    # I can't be bothered to get the whitespace right in the above, let gofmt
    # figure it out for me
    call(["gofmt", "-w", goFilePath])

    return origImport


def main():
    framework_path = "./test_framework.go"
    if not os.path.exists(framework_path):
        print("test_framwork.go not found: Please run this script from within your proj5_tests directory")
        exit()

    # Create an import statement for each buggy implementation in the proj5-bugs directory
    knownBugImportPath = "github.com/61c-teach/proj5-impls/"
    knownBugFullPath = os.path.join(os.environ['GOPATH'], "src", knownBugImportPath)

    imports = [ "memoizer \"" + knownBugImportPath + os.path.basename(dir) + "\"" for dir in glob.glob(knownBugFullPath + "/*")]
    if len(imports) == 0:
        print("Couldn't find known bug packages at: " + knownBugFullPath)
        exit()

    # Store all the return codes, 0 means "pass" and non-0 means "fail"
    retcode = []
    # Unlike everything else, the good test is expected to pass, this is it's index (we find it below)
    goodIdx = 0
    
    # remember what they had in there originally so we can replace it
    orig = replaceMemoizerImport(framework_path, imports[0])
    for i, imp in enumerate(imports):
        replaceMemoizerImport(framework_path, imp)
        print("Running bug test: " + imp)
        retcode.append(call(["go", "test", "-v"]))
        if "good" in imp:
            goodIdx = i

    #Add 2 newline for clarity
    print("\n")
    if retcode[goodIdx] != 0:
        print("Reference implementation: failed a test! While it's possible that" +
              " you've found a bug in our implementation, you should probably" +
              "  double check things just in case.")
    else:
        print("Reference implementation: passed your tests!")

    del retcode[goodIdx]
    del imports[goodIdx]

    for i in range(len(imports)):
        if retcode[i] != 0:
            print("Test " + imports[i] + ": Success")
        else:
            print("Test " + imports[i] + ": Failure (passed when it should have failed)")

    replaceMemoizerImport(framework_path, orig)
main()
