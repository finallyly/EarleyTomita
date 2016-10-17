#########################################################################
#   Copyright (C) 2016 All rights reserved.
#   
#   文件名称：run.sh
#   创 建 者：unicodeproject
#   创建日期：2016年07月20日
#   描    述：
#
#   备    注：
#
#########################################################################
#!/bin/bash

# please add your code here!
if [ 0 -ne $# ]; then
	echo "USAGE: prog [IN]input_file" >&2;
	exit 1;
fi
#source /etc/profile
#source ~/.bash_profile
ulimit -c unlimited
./test thai.check.toreview.txt  thai.result.txt lrinfo.txt
echo "$0 has finished,congratulations!">&2;
