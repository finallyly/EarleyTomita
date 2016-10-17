/*===============================================================
 *
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：common.h
*   创 建 者：unicodeproject
*   创建日期：2016年07月20日
*   描    述：
*   备    注: 
*   更新日志：
*
================================================================*/
#ifndef _COMMON_H
#define _COMMON_H
// please add your code here!
#include<iostream>
#include<string>
#include<vector>
using namespace std;
#define TEXT_LEN1 8192
#define ACTION_TABLE_LEN 256 //define the max length of  action[0][1]'s content;
#define LINE_LEN_1 1024
#define LINE_LEN_2  512
#define LINE_LEN_3 2048
#define LINE_LEN_4 8192
#define LINE_LEN_5 96
#define CONTENT_NODE_COUNT 5000

void SplitString(string orignal, vector<string>&results, char *delims);
typedef struct StackNode {
    string key;
    char *elemlist;
    char *symbollist;
    int *visited;
    StackNode *parent;
    public:
    StackNode() {
        key = "";
        elemlist = NULL;
        symbollist = NULL;
        visited = NULL; 
        parent = NULL;
    }
    ~StackNode() {
        key = "";
        if (NULL != elemlist) {
            delete [] elemlist;
            elemlist = NULL;
        }
        if (NULL != symbollist) {
            delete [] symbollist;
            symbollist = NULL;
        }
        if (NULL != visited) {
            delete [] visited;
            visited = NULL;
        }
        /*
        if (NULL != parent) {
            delete parent;
            parent = NULL;
        }
        */
    }
}  StackNode;
#endif
