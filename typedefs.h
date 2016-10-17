/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：typedef.h
*   创 建 者：刘禹 finallyly liuyusi0121@sogou-inc.com(6141)
*   创建日期：2016年04月19日
*   描    述：
*   备    注: 
*   更新日志：
*
================================================================*/
#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H
#include <stdio.h>
#define MAX_WORD_LEN 15
#define MAX_TRIE_LEN 16
#define MAX_UTF8_SIZE 3
//17*65536 unicode的全部字符范围
#define ENTRY_LEN 1114112
#define WCOST 2
#define MIN_WORD_FREQ 1
using namespace std;
typedef unsigned int UINT32;
typedef unsigned char uchar;
typedef unsigned short UINT16;
#endif
