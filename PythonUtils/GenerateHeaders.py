import os
import glob
import argparse
import fnmatch

parser = argparse.ArgumentParser(description='Generates a top-level header file to include all headers in a directory tree')
parser.add_argument('--outfile', dest='outfile', required=True, help='The output header file to generate')
parser.add_argument('--dirs', dest='dirs', default='.', help='The directories to parse (relative or absolute path)')
parser.add_argument('--ignore-path', dest='ignores', default='', help='Path patterns to ignore (does not work with wildcards)')
parser.add_argument('--pattern', dest='pattern', default='*.h', help='A wildcard pattern of files to include (*.h by default)')

args = parser.parse_args()


if args.ignores == '': ignorelist = []
else: ignorelist = args.ignores.split(',')

dirlist = args.dirs.split(',')
outputfile = open(args.outfile, 'w+')

for dir in dirlist:
    if not os.path.exists(dir):
        print "Path not found: ", dir
        continue

    for curpath, dirs, files in os.walk(dir):
        ignore = False

        # Check if we should ignore the path
        for ig in ignorelist:        
            if -1 != curpath.lower().find(ig.lower()):
                ignore = True
                break;

        if not ignore:
            # Filter the list of files so the results matches the given pattern
            files = fnmatch.filter(files, args.pattern)

            padding_created = False            
            
            for f in files:
                file_path = os.path.join(curpath, f)
                
                # Ignore the output file, if it happens to be in the target path                
                if not fnmatch.fnmatch(args.outfile, file_path):
                    if not padding_created:
                        outputfile.write("\n")
                        padding_created = True
                    outputfile.write("#include \"")
                    outputfile.write(file_path)
                    outputfile.write("\"\n")

# Padding at the end of file
outputfile.write("\n")
                                     
outputfile.close()
