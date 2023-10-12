#!/bin/bash

root_path="`pwd`"
bin_path="`pwd`/bin"
output_path=$bin_path"/output.txt"
utils_path="`pwd`/utils"
magic_parser=$utils_path"/magic_parser.c"

file_path="`pwd`/silly_sort.c"
compiler="gcc"
flags="-O3 -fopenmp"
max_iter=$1
number_threads=$2

echo "Running "$max_iter" iterations."

# ** Folder verification
if [ ! -d "$bin_path" ]; then
	mkdir "$bin_path"
	echo "Folder '$bin_path' created."
else
	echo "Folder '$bin_path' already exists."
	if [ -n "$(ls -A $bin_path)" ]; then
		echo "Deleting pre-existing files. . ."
		rm $output_path
	fi
fi

# ** Compilation
$compiler "-Wall" $magic_parser "-lm" "-o"$utils_path"/a.out"
$compiler $file_path $flags



for ((i = 1; i <= $max_iter; i++));
	

	do
		(time ./a.out $number_threads) 2>> $output_path;
	done


# ** Executing magic_parser
cd "$utils_path"
(./a.out $output_path $max_iter $number_threads)
cd "$root_path"
cat `pwd`"/result.txt"
