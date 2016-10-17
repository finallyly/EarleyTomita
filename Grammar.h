/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：Grammer.h
*   创 建 者：unicodeproject
*   创建日期：2016年07月20日
*   描    述：do not take empty prodution into consideration
*   备    注: 
*   更新日志：
*
================================================================*/

#ifndef _GRAMMER_H
#define _GRAMMER_H
#include<iostream>
#include<string>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>
#include "common.h"
#include <stack>
 using namespace std;
// please add your code here!
class Grammar
{
public:
    string StartSymbol;
    Grammar(string startsymbol) {
        StartSymbol=startsymbol;
    }
    ~Grammar() {
    m_terminals.clear();
    m_nonterminals.clear();
    m_rules.clear();
    m_id2rules.clear();
    m_symbols.clear();
    m_firstset.clear();
    m_followset.clear();
    }
    std::tr1::unordered_map<string,vector<string> > m_morphmap;
    std::tr1::unordered_map<string,int> m_terminals;
    std::tr1::unordered_map<string,int> m_nonterminals;
    std::tr1::unordered_map<string,int> m_rules;
    std::tr1::unordered_map<int,string> m_id2rules;
    std::tr1::unordered_set<string> m_symbols;
    std::tr1::unordered_map<string,string> m_firstset;
    std::tr1::unordered_map<string,string> m_followset;
    //load the grammar rules 
    int LoadGrammarRules(const char* path);
    int LoadVocab(const char* path);
    // load Nonterminals of the grammar
    int LoadNonterminals(const char* path);
    // load Terminals of the Grammar
    int LoadTerminals(const char* path);
    // get the terminals and nonterminals;
    int GetSymbols();
    // the Nonterminals' first set;
    std::tr1::unordered_map<string,string> m_firsts;
    // the Nonterminals's follow set;
    std::tr1::unordered_map<string,string> m_follows;
    // given a Nonterminal calc the firstset;
    int GetSingleFirst(string elem, string &firststr);
    // given a Nonterminal calc the followset;
    int GetSingleFollow(string elem, string &firststr);
    // get all Nonterminals's first set;
    int GetFirstSet();
    // get all Nonterminal's follow set;
    int GetFollowSet();
    //given a head ,return the string that appears first in production
    int GetFirstElemInRuleBody(string head,std::tr1::unordered_set<string> &elems);
    // assist follow set calc
    int GetHeadAndNeighborSet(string bodyelem,std::tr1::unordered_set<string> &headelems, std::tr1::unordered_set<string> &neighborelems);
    // used after GetFirstElemInRuleBody, to see if all the elem appears first is Nonterminals or not
    bool isAllTerminals(std::tr1::unordered_set<string> &itsset, vector<string>& nontermvec); 
    
};
#endif
