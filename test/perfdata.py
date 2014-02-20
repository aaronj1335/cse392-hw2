#!/usr/bin/env python

import sys
from random import random

LEN = 300 * 1e6

def main():
    dimension = int(sys.argv[1])
    for i in xrange(int(LEN)):
        for j in xrange(dimension):
            sys.stdout.write('%f ' % random())
        sys.stdout.write('\n')

if __name__ == '__main__': main()
