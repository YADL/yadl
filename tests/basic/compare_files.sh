#!/bin/bash
# Script to compare two files

file1=`md5deep -q $1`

file2=`md5deep -q $2`

if [ "$file1" = "$file2" ]
then
        echo  "Files are Equal"
else
        echo "Files are Not Equal"
fi
 
