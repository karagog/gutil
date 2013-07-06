# Copyright 2010-2013 George Karagoulis
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


#*********************************************************************************
#
# This script makes symlinks to the .so libs for a linux build.  This takes care
#  of the case when it builds a *.so.1 file, you can use this script to make a *.so
#  file which links to that.
#
#*********************************************************************************

import os
from os import listdir
from os.path import isfile, join
import argparse
import fnmatch
import re
from subprocess import call

parser = argparse.ArgumentParser(description='Generates helpful symlinks to shared libs which end in a number (.so.1).')
parser.add_argument('--lib-version', dest='lib_version', default='', help='Tells the program what version of shared library to expect.  Example: 1')
parser.add_argument('--working-dir', dest='cwd', default='.', help='The current directory in which we should be running.  All other given paths, when relative, are relative to this path.')

args = parser.parse_args()
dir=args.cwd

if not os.path.exists(dir):
    print("The path you specified does not exist: %s" % dir)
    exit(1)

if '' == args.lib_version:
    # We want to match every file that ends with *.so.<any number>
    regexp_string = r".so\.[1-9]*$"
else:
    regexp_string = r".so\.(" + args.lib_version + ")$"

files = [ f for f in listdir(dir) if isfile(join(os.path.abspath(dir),f)) ]
re_obj = re.compile(regexp_string)

for f in files:
    if None != re_obj.search(f):
#        print("Found a match: %s" % f)
        new_name = f[:f.index(".so.")] + ".so"
        call(["ln", "-sf", f, os.path.join(dir, new_name)])

