#########################################################################
#   Copyright (C) 2016 All rights reserved.
#   
#   文件名称：make.sh
#   创 建 者：unicodeproject
#   创建日期：2016年07月20日
#   描    述：
#
#   备    注：
#
#########################################################################
#!/bin/bash
g++ -g -Wall -gstabs+  test.cpp typedefs.h util.h util.cpp LoadGrammar.cpp Grammar.h  SimpleLR.h SimpleLR.cpp common.h common.cpp GraphStack.h GraphStack.cpp -o test
#g++ -g   test.cpp LoadGrammar.cpp Grammar.h  SimpleLR.h SimpleLR.cpp common.h common.cpp GraphStack.h GraphStack.cpp -o test
# please add your code here!
#g++ -g -Wall -gstabs+ 
