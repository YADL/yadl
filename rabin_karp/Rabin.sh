#!/bin/sh
make -f Makefile || exit 1
./Rabin_Karp test_data.txt || exit 1
if(diff test_data.txt test_data_copy.txt) then
	echo "Success"
else
	echo "Failure"
fi

