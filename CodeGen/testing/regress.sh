#!/bin/bash

# usage: ./regress.sh dirname testfilename#1 testfilename#2 ... testfilename#n

if [ $# -lt 2 ]
then
	echo usage: regress.sh dirname testfilename...
	exit 1
fi

if [ -f $1 ]; then	
	echo first argument \(\'$1\'\) not a directory.
	exit 1
fi

# get, through the date command, the name of the file where the test results will be stored in.
frontpart=$(date +%F | sed 's/-//g')
backpart=$(date +%H:%M:%S | sed 's/://g');
newdir=$(echo ${frontpart}:${backpart})

# make the new directory, corresponding to this specific call of this regression testing command.
mkdir ${newdir}

# see if the previous command succeeded.
if [ $? -ne 0 ]
then
	echo Error: the creation of new file for this regression testing attempt failed.
	exit 2
fi
echo $newdir

dirname=$1		# The directory where the results of the tests will be stored (first usage) or are stored.
first=0			# Should test results be stored in dirname? Default: NO
if [ ! -d $dirname ]		# if the directory $dirname does not exist
then
	mv ${newdir} ${dirname}		# rename $newdir (date,time) to $dirname and store test results there
	if [ $? -ne 0 ]; then
		echo "failed to save results in ${dirname}; they remain in ${newdir}"
		dirname=$newdir
	else
		newdir=$dirname	
		first=1							# yes, test results should be stored in $dirname
	fi
fi

shift 		# we do not need first argument anymore, so shift
for file in "$@" 			# loop through the rest of the arguments
do
	if [ ! -f $file ] || [ ! -r $file ]; then 	
		echo "$file is not a file or readable"
		rm -r $newdir # delete everything that happened. This regression testing failed to complete.
		exit 1
	fi
	
	# test this file
	cat $file > ./${newdir}/${file}.test < /dev/null   # copy the file
	bash $file </dev/null > ./${newdir}/${file}.stdout 2> ./${newdir}/${file}.stderr # redirect stdout and stderr
   echo ${PIPESTATUS[0]} > ./${newdir}/${file}.status < /dev/null
done

if [ $first -eq 1 ]; then
	echo Saved test results in $newdir.
elif [ "$newdir" == "$dirname" ]; then
	exit 0
else
	echo Comparing $newdir with $dirname...
	if [ $(diff -q $newdir $dirname | wc -w) -eq 0 ]; then
		echo no differences
	else
		diff -q $newdir $dirname
	fi
fi

exit 0


