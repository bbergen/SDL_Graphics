#!/usr/bin/env bash

usage="Usage: $0 [-a] file"
append=false
output_file="mem_report.txt"

if [ $# -ne 1 ] && [ $# -ne 2 ];
then
    echo "$usage" 1>&2
    exit
fi

if [ $# -eq 2 ] && [ $1 != '-a' ];
then
    echo "$usage" 1>&2
    exit
fi

if [ $1 = '-a' ];
then
    append=true
fi

if [ "$append" = true ] && [ -x $2 ];
then
    $(valgrind --leak-check=yes ./"$2" >> "$output_file" 2>&1)
elif [ -x $1 ];
then
    $(valgrind --leak-check=yes ./"$1" > "$output_file" 2>&1)
else
    echo "Error: Not an executable file"
    echo "$usage"
    exit
fi
