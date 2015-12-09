#! /bin/sh

for testfile in good*.cminor
do
	if ./cminor -typecheck $testfile > $testfile.out
	then
		echo "$testfile success (as expected)"
	else
		echo "$testfile failure (INCORRECT)"
	fi
done

for testfile in bad*.cminor
do
	if ./cminor -typecheck $testfile > $testfile.out
	then
		echo "$testfile success (INCORRECT)"
	else
		echo "$testfile failure (as expected)"
	fi
done

