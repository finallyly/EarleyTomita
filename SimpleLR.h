/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：LR.h
*   创 建 者：unicodeproject
*   创建日期：2016年07月20日
*   描    述：
*   备    注: 
*   更新日志：
*
================================================================*/
 
#ifndef _LR_H
#define _LR_H
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "Grammar.h"
#include <vector>
// please add your code here!
typedef vector<std::tr1::unordered_set<string> > ClosureSets;
class SimpleLR : public Grammar {
    public: 
        //ItemSets itemSets;
        SimpleLR(string startsymbol);
        ~SimpleLR();
        int Closure(std::tr1::unordered_set<string>&itemSet);
        ClosureSets m_closures;
        std::tr1::unordered_map<int,string> m_closureNext;
        int GOTO(std::tr1::unordered_set<string>&itemSet, std::tr1::unordered_set<string>&toitemSet, string Symbol);
        int GenerateItemSets();
        int ConstructActionTable();
        int ConstructGoToTable();
        int GetCount();
        int ConstructTables();
        int NextEdge();
        char ***ActionTable;
        int **GoToTable;
        int m_states_count;//LR表的state数量
        int m_nonterms_count;//符号表中数量
        int m_terms_count;
};
#endif
