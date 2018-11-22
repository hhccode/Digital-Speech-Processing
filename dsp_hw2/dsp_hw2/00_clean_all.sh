#!/bin/bash

if [ -d MFCC/ ]; then
	echo "rm -r MFCC/"
	rm -r MFCC/
	echo "mkdir MFCC"
	mkdir MFCC
fi

if [ -d hmm/ ]; then
	echo "rm -r hmm/"
	rm -r hmm/
	echo "mkdir hmm"
	mkdir hmm
fi


if [ -d result/ ]; then
	echo "rm -r result/"
	rm -r result/
	echo "mkdir result"
	mkdir result
fi


cd bin; make clean; cd ..
