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
# This script will generate all the top-level headers for a software project.
#  You simply show it where the source code directory is (and optionally patterns
#  to ignore, a working dir, etc...)
#  and it will generate one top-level header per actual source code header file,
#  and put it in an 'include' sub-directory.
#
# With this system, you can have a source tree of arbitrary complexity, and yet
#  have an easy-to-use API for which a developer doesn't need to understand the
#  structure of the source code, they only need to know that such a class exists,
#  and they can find the appropriate header in the include directory
#
#*********************************************************************************


import os
import glob
import argparse
import fnmatch
import datetime

parser = argparse.ArgumentParser(description='Generates a top-level header file to include all headers in a directory tree')
parser.add_argument('-v', '--verbose', dest='verbose', action='store_true', help='Switches on verbosity output')
parser.add_argument('--output-prefix', dest='prefix', default='', help='A prefix to put on the front of the generated headers (ex. prefix_header.h).  There is no prefix by default.')
parser.add_argument('--input-dirs', dest='dirs', default='.', help='The directories to parse (relative or absolute path)')
parser.add_argument('--ignore-path', dest='ignores', default='', help='Path patterns to ignore (does not work with wildcards)')
parser.add_argument('--ignore-patterns', dest='ignore_patterns', default='', help='Wildcard filename patterns to ignore')
parser.add_argument('--infile-pattern', dest='pattern', default='*.h', help='A wildcard pattern of files to include (*.h by default)')
parser.add_argument('--working-dir', dest='cwd', default='.', help='The current directory in which we should be running.  All other given paths, when relative, are relative to this path.')
parser.add_argument('--output-dir', dest='outdir', default='include', help='The directory into which to place the generated headers.  The default value is \"include\"')

args = parser.parse_args()
runtime = datetime.datetime.now().isoformat(' ')

file_patterns = args.pattern.split(',')
ignore_patterns = args.ignore_patterns.split(',')

verbosity = 0
if args.verbose:
    verbosity = 1

if 1 == verbosity:
    print
    print("Looking for patterns like %s" %  file_patterns)
    print("Ignoring patterns like %s" % ignore_patterns)

# Change our current working directory first
if(os.path.exists(args.cwd)):
    dest_path = os.path.abspath(args.cwd)
    os.chdir(dest_path)
    if os.getcwd() != dest_path:
        print("Unable to change directories to \"%s\"" % args.cwd)
        exit(1)
else:
    print("Working directory does not exist: \"%s\"" % args.cwd)
    exit(1)

# Check that the output directory exists, and create it if not
if(not os.path.exists(args.outdir)):
    os.makedirs(args.outdir)
    if(not os.path.exists(args.outdir)):
        print("Output directory does not exist, and I couldn't create it: \"%s\"" % args.outdir)
        exit(1)

if args.ignores == '': ignorelist = []
else: ignorelist = args.ignores.split(',')

# We have to ignore the output directory, otherwise we may have problems
ignorelist.append(args.outdir)

dirlist = args.dirs.split(',')


if 1 == verbosity:
    print('Searching in the following directories:')
    for dir in dirlist:
        print('\t%s' % dir)

    print('\nIgnoring the following directories:')
    for ig in ignorelist:
        print('\t%s' % ig)

    print


for dir in dirlist:
    if not os.path.exists(dir):
        print("Path not found, \"%s\", continuing..." % dir)
        continue

    for curpath, dirs, files in os.walk(dir):
        # Search the current directory for headers
        if 1 == verbosity:
            print('\tSearching %s' % curpath)

        # Filter the list of files so the results match the given input patterns,
        #   and ignore the given ignore file patterns
        file_list = []
        ignore_files = []

        # Get a list of all the files that match the input patterns
        for fp in file_patterns:
            for tmp in fnmatch.filter(files, fp):
                if 0 == file_list.count(tmp):
                    file_list.append(tmp)

        # Get a list of all the files that match the ignore patterns
        for ip in ignore_patterns:
            for tmp in fnmatch.filter(files, ip):
                if 0 == ignore_files.count(tmp):
                    ignore_files.append(tmp)

        # Remove the ignored files from the file list
        for i_f in ignore_files:
            while 0 < file_list.count(i_f):
                if 1 == verbosity:
                    print('\tIgnoring file because it matches the ignore pattern: %s' % i_f)
                file_list.remove(i_f)

        for f in file_list:
            file_path = os.path.join(curpath, f)

            # Ignore the output file, if it happens to be in the target path
#            if(fnmatch.fnmatch(args.outfile, file_path)):
#                continue

            without_extension, extension = os.path.splitext(f)

            ap_dest = os.path.abspath(args.outdir)
            ap_src, file_name  = os.path.split(os.path.abspath(file_path))
            p_dest = ""
            p_src  = ""
            
            # Find the point where the paths deviate
            ind_first_difference = 0
            diff_found = 0
            for i in range(len(ap_src)):
                ind_first_difference = i
                if i >= len(ap_dest) or i >= len(ap_src):
                    break
                elif ap_dest[i] != ap_src[i]:
                    diff_found = 1
                    break
            
            # The number of '..' we need to include in the header file
            updir_cnt = 0

            if diff_found:
                p_dest = ap_dest[ind_first_difference:]
                p_src  = ap_src[ind_first_difference:]
                updir_cnt = len(p_dest.split('/'))
            
            rel_path = ""
            for i in range(updir_cnt):
                rel_path = os.path.join(rel_path, "..")
            rel_path = os.path.join(rel_path, p_src)
            
            #print("\n\nAbs:  %s\nAbs:  %s" % (ap_src, ap_dest))
            #print("\nDiff: %s\nDiff: %s\n%s" % (p_src, p_dest, rel_path))
            

            print("Discovered: %s" % file_path)

            outfile_path = os.path.join(args.outdir, "%s%s%s" % (args.prefix, without_extension, extension))
            outputfile = open(outfile_path, 'w+')

            outputfile.write("/* This file was auto-generated by scripts on %s */\n" % runtime);
            outputfile.write("#include \"%s\"\n" % os.path.join(rel_path, file_name))
            outputfile.close()

        # Check if we should ignore any subdirectories before descending into them
        if 1 == verbosity:
            print('Subdirectories: %s' % dirs)

        for d_index in range(len(dirs) - 1, -1, -1):
            for ig in ignorelist:
                if -1 != dirs[d_index].lower().find(ig.lower()):
                    if 1 == verbosity:
                        print('\tIgnoring %s' % dirs[d_index])
                    dirs.pop(d_index)
                    break;

print("Header Generation Complete")
