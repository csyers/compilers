#! /bin/sh

for testfile in good/good*.cminor
do
	if ./cminor -codegen $testfile $testfile.s > $testfile.out
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done
