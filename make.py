#!/usr/bin/env python

import os
import sys
import subprocess

def find_arg_0param(expected_arg):
    for arg in sys.argv:
        if arg == expected_arg:
            return True

    return False

def fatal(msg):
    print(msg, file=sys.stderr)
    sys.exit(1)

def message(msg):
    if not find_arg_0param('--quiet'):
        print(msg)

def shell(cmd):
    if not find_arg_0param('--quiet'):
        print(' '.join(cmd))
    cp = subprocess.run(cmd)

    if cp.returncode != 0:
        fatal("%s failed" % ' '.join(cmd))

if __name__ == '__main__':

    os.chdir('src')
    shell(['jfdi'])
