#! /bin/bash

LIBS_DIR="../lib"

declare -i num_libs
num_libs=`ls -l $LIBS_DIR/*.a | wc -l`

if [ $num_libs -gt 0 ]; then
	rm $LIBS_DIR/*.a
fi;

exit 0;
