#Written by Stephan Kaminsky 1/26/2018
import random
import math

version = "v1.4"

minStrlen = 1 # This is the minimum size string that will be generated
maxStrlen = 1000 # This is the maximum size string that will be generated
numLines = 5000 # This is the number of entries into the dictionary

singleSpaceDictionary = True # This is for wellformed dictionarys. The spec is that there is only one space. You can set it to false if you want it to generate more spaces.

testlen = 5000 # Number of words to use in it
newlinechance = 0.25 # 0 - 1 Chance to print \n
PullFromDictChance = 0.2 # 0 - 1 Chance to directly pull next word from dict
RandomCharChance = 0.3 # 0 - 1 This is the chance to gen a random non alphanumeric character after a word

#These are the names of the output files
dictfile = 'newrep.txt' # This is the dictionary file
testfile = 'newtest.txt' # This is the file before the replacements occure
reffile = 'newref.txt' # This is the file with all replacements in it



dictionary = {}

def genalphanumerStr(min, max):
    return ''.join(random.choice('0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz') for i in range(random.randint(min, max)))

def getprintableStr(min, max):
    return ''.join(random.choice('0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!"#$%&\'()*+,-./:;?@[\\]^_`{|}~') for i in range(random.randint(min, max)))

def genSpaces(min, max):
    return ' ' * random.randint(min, max)

def getRandChar():
    return ''.join(random.choice('!"#$%&\'()*+,-./:;?@[\\]^_`{|}~') for i in range(1))

message = "| Test case generator " + version + " by Stephan Kaminsky. |"

print("|" + "-" * (len(message) - 2) + "|")
print(message)
print("|" + "-" * (len(message) - 2) + "|")

print("Settings:")
print("      Min String Length       | " + str(minStrlen))
print("      Max String Length       | " + str(maxStrlen))
print("  Number of Dictionary Pairs  | " + str(numLines))
print("   Single Space dictionary    | " + str(singleSpaceDictionary))
print(" Number of words in test file | " + str(testlen))
print("       New Line Chance        | " + str(newlinechance))
print("  Pull From dictionary Chance | " + str(PullFromDictChance))
print("   Random Character Chance    | " + str(RandomCharChance))

print("\nBuilding dictionary...")

d = open(dictfile, 'w')

for i in range(numLines):
    print("Number of pairs made: {}/".format(i + 1) + str(numLines), end="\r")
    astr = genalphanumerStr(minStrlen, maxStrlen)
    pstr = getprintableStr(minStrlen, maxStrlen)
    dictionary[astr] = pstr
    spstr = " " if singleSpaceDictionary else genSpaces(minStrlen, maxStrlen)
    d.write(astr + spstr + pstr + "\n")

print("\nDictionary finished!")
print("\nBuilding the text in and reference files...")

numDictEnt = 0

t = open(testfile, 'w')
r = open(reffile, 'w')

for i in range(testlen):
    print("Number of words progress: {}/".format(i + 1) + str(testlen) + ". Number of words from the dictionary: {}".format(numDictEnt), end="\r")
    genNewWord = random.random() > PullFromDictChance
    addNewLine = random.random() < newlinechance
    addRandChar = random.random() < RandomCharChance


    if (genNewWord):
        w = genalphanumerStr(minStrlen, maxStrlen)
        if (w in dictionary.keys()):
            numDictEnt += 1
            r.write(dictionary[w])
        else:
            wAllLower = w.lower()
            wLowerButFirst = w[0] + wAllLower[1:]
            if (wLowerButFirst in dictionary.keys()):
                numDictEnt += 1
                r.write(dictionary[wLowerButFirst])
            else:
                if(wAllLower in dictionary.keys()):
                    numDictEnt += 1
                    r.write(dictionary[wAllLower])
                else:
                    r.write(w)
    else:
        w = random.choice(list(dictionary.keys()))
        numDictEnt += 1
        r.write(dictionary[w])

    t.write(w)


    if (addRandChar):
        c = getRandChar()
        t.write(c)
        r.write(c)

    minsp = 1
    maxsp = maxStrlen
    if (addRandChar):
        minsp = 1
        maxsp = math.ceil(maxStrlen * 0.25)

    sp = genSpaces(minsp, maxsp)

    if (addNewLine):
        t.write("\n")
        r.write("\n")
    else:
        t.write(sp)
        r.write(sp)

d.close()
t.close()
r.close()
dashes = "-" * ((len(message) - 2 - 5) // 2)
print("\n<" + dashes + "Done!" + dashes  + ">\n")
