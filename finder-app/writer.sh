#!/bin/bash

writefile=$1
writestr=$2

if [ $# -ne 2 ];then
   echo "Error: Requires two arguments"
   echo "Usage: $0 <writefile> <writestr>"
   exit 1
else
   touch $writefile
   echo "$writestr" >> $writefile
fi

