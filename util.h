/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：util.h
*   创 建 者：刘禹 finallyly liuyusi0121@sogou-inc.com(6141)
*   创建日期：2016年04月19日
*   描    述：utf-8转unicode,unicode转utf-8
*   备    注: 
*   更新日志：
*
================================================================*/
 
#ifndef _UTIL_H
#define _UTIL_H
#include <string>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "typedefs.h"
using namespace std;
 //将一个unicode转成utf8编码
int enc_unicode_to_utf8_one(const UINT32 unic, uchar *pOutput, int outSize);
int enc_calc_utf8_outsize(const UINT32 *unicArray,int unicArraySize);
int enc_calc_unicode_outsize(const uchar* pInput);
//将unicode序列转成utf8编码序列；
int enc_unicode_to_utf8_batch(const UINT32 *unicArray, int unicArraySize, uchar *pOutput, int outSize);
//将一个utf8编码转换成unicode
int enc_utf8_to_unicode_one(const uchar* pInput,UINT32 *Unic);
//将一个utf8编码序列转换成unicode序列
int enc_utf8_to_unicode_batch(const uchar* pInput,UINT32 *UnicSeq, int outSize);
//根据开始字节判定该utf8串一共占几个字节
int enc_get_utf8_size(uchar c);
#endif
