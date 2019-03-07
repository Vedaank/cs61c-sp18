#!/usr/bin/env python3

import sys, os, time

batch_args = "-b ../input/baboon.png ../input/balloons.png ../input/bigbrain.png ../input/bird.png ../input/bottle.png ../input/bowtie.png ../input/c.png ../input/chair.png ../input/darkknight.png ../input/darknight.png ../input/dollar.png ../input/flag.png ../input/guitar.png ../input/house.png ../input/knight.png ../input/night.png ../input/ocean.png ../input/oski.png ../input/playground.png ../input/rainbow.png ../input/silver.png ../input/smile.png ../input/snorlax.png ../input/square.png ../input/stool.png ../input/sword.png ../input/tree.png ../input/valve.png ../input/wallet.png ../input/weaver.png"

view_args = "-v ../input/flag.png"

naive_time = 1
student_time = 1


def main(mode):
	if mode == 'view':
		run_view()
	elif mode == 'batch':
		run_naive(True)
		run_student(True)
		print ("Speedup by {} times".format(naive_time / student_time))
		check_correctness()
	elif mode == 'correctness':
		run_student(False)
		check_correctness()
	else:
		print ("Unknown mode specified to run")


def run_naive(use_timer):
	global naive_time
	if use_timer:
		timer = time.time()
	os.system("cd naive; ./ced " + batch_args)
	finish_timer = time.time()
	if use_timer:
		print ("Run time for naive was {} seconds".format(finish_timer - timer))
		naive_time = finish_timer - timer

def run_student(use_timer):
	global student_time
	if use_timer:
		timer = time.time()
	os.system("cd student; ./ced " + batch_args)
	finish_timer = time.time()
	if use_timer:
		print ("Run time for student was {}".format(finish_timer - timer))
		student_time = finish_timer - timer

def run_view():
	os.system("cd student; ./ced " + view_args)

def check_correctness():
	os.system("./check-correctness")

if __name__ == '__main__':
	if len(sys.argv) != 2:
		print ("Wrong input number. Need exactly 1 input")
		sys.exit(1)
	main(sys.argv[1])
