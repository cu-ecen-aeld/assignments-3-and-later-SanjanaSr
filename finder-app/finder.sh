#!/bin/sh
filesdir=$1
searchstr=$2

if [ $# -lt 2 ];then
   echo "Error: Requires two arguments"
   echo "Usage: $0 <filesdir> <searchstr>"
   exit 1
fi

#Check if  directory filesdir exists
if [ ! -d "$filesdir" ]
then
  echo "Error: $filesdir does not exist"
  exit 1
else
 file_count=$(find "$filesdir" -type f | wc -l)
 line_count=$(grep -rc  "$filesdir" -e "$searchstr" | wc -l)
 echo "The number of files are ${file_count} and the number of matching lines are ${line_count} "
fi



