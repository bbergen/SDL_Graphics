#!/usr/bin/env bash

usage="Usage: $0 [-a] file"
base_command="valgrind --leak-check=yes"
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

if [ $1 = '-a' ] && [ -x $2 ];
then
    base_command="$base_command ./$2 >>"
    executable=$2
elif [ -x $1 ];
then
    base_command="$base_command ./$1 >"
    executable=$1
else
    echo "Error: Not an executable file"
    echo "$usage"
    exit
fi

output_file="mem_report_${executable}.html"

# run valigrind
eval "${base_command} ${output_file} 2>&1"

# now let's pretty up the report
sed -i 's/$/<br>/' ${output_file}
sed -i 's/<br><br>/<br>/' ${output_file}

# lets open it up to view
if [ -x '/usr/bin/xdg-open' ];
then
    xdg-open ${output_file} &
fi
