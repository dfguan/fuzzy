#Author  : Dengfeng Guan
#Purpose : function can access outside variable

test() {

	echo $dir # can access $dir; NB: doesn't work with bsub
}

export -f test

if [ "$#" -gt 0 ]
then
	dir=$1
	test
fi
