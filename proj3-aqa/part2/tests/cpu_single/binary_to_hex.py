from __future__ import print_function
import sys
def main(args):
	file = open(args[1])
	lines = [l for l in file.readlines()]
	def mapper(strr):
		return hex(int(strr, 2))[2:]
	results = []
	for l in lines:
		hexes = list(map(mapper,l.split()))
		r1 = ''.join(hexes[:8])
		r2 = ''.join(hexes[8:16])
		r3 = ''.join(hexes[16:24])
		r4 = ''.join(hexes[24:32])
		r5 = ''.join(hexes[32:40])
		r6 = ''.join(hexes[40:48])
		r7 = ''.join(hexes[48:56])
		r8 = ''.join(hexes[56:64])
		r9 = ''.join(hexes[64:68])
		r10 = ''.join(hexes[68:76])
		r11 = ''.join(hexes[76:])
		result = [r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11]
		results.append(result)
	print ("ra       sp       t0       t1       t2       s0       s1       a0       Line# PC      Inst")
	for r in results:
		string = ' '.join(r)
		print (string)
		print ('\n')


if __name__ == "__main__":
	main(sys.argv)
