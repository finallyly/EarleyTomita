/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：LR.cpp
*   创 建 者：unicodeproject
*   创建日期：2016年07月20日
*   描    述：
*   备    注: 
*   更新日志：
*
================================================================*/
 
#include "SimpleLR.h"
#include <queue>
#include "common.h"
#include <set>
// please add your code here!
SimpleLR::SimpleLR(string startsymbol): Grammar(startsymbol) {
    LoadGrammarRules("rule.txt");
    LoadNonterminals("Nonterms.txt");
    LoadVocab("rule_low.txt");
    LoadTerminals("terminals.txt");
    GetSymbols();
    ActionTable = NULL;
    GoToTable = NULL;
    m_states_count = 0;
    m_nonterms_count = 0;
    m_terms_count = 0;
}
SimpleLR::~SimpleLR() {
    if (NULL != ActionTable ){
        for (int i = 0 ; i< m_states_count;i++ ) {
            if (NULL != ActionTable[i]){
                for (int j = 0; j < m_terms_count + 1;j++) {
                    if (NULL != ActionTable[i][j]) {
                        delete [] ActionTable[i][j];
                        ActionTable[i][j] = NULL;
                    }
                }
                delete [] ActionTable[i];
                ActionTable[i] = NULL;
            }
        }
        delete [] ActionTable;
        ActionTable = NULL;
    }
    if (NULL != GoToTable) {
        for (int i = 0; i< m_states_count; i++) {
            if (NULL != GoToTable[i]) {
                delete [] GoToTable[i];
                GoToTable[i] = NULL;
            }
        }
        delete [] GoToTable;
        GoToTable = NULL;
    }
    m_closures.clear();
    m_closureNext.clear();
}


int SimpleLR::Closure(std::tr1::unordered_set<string>&itemSet) {
    queue<string> tempque;
    set<string> SymbolVisited;
    //copy the rules in the initial itemSet to the tempque
    for (std::tr1::unordered_set<string>::iterator it=itemSet.begin();
        it!=itemSet.end();it++ ) {
        tempque.push(*it);
    }
    //rule S:dott,v ==>NP S->.vNP dott means .; 
    while(!tempque.empty()) {
        string elem = tempque.front();
        tempque.pop();
        vector<string> results;
        SplitString(elem,results,":,");
        // the rule is not complete
        if (results.size() < 2) {
            fprintf(stderr,"LR.cpp rule incomplete\n");
            exit(1);
        }
        int i = 1;
        for (;i < int(results.size());i++){
            if ("dott" == results[i]){
                break;
            }
        }
        string expelem;//the Grammer Symbol to expand
        if (i < int(results.size() - 1)) {
            expelem = results[i + 1];
        }
        //the one follow dott is a grammer symbol and not visited
        if (m_nonterminals.count(expelem) && !SymbolVisited.count(expelem)) {
            for (std::tr1::unordered_map<string,int>::iterator it = m_rules.begin();
             it != m_rules.end();it++) {
                vector<string> rules_vec;
                SplitString(it->first,rules_vec,":,");
                if (rules_vec.size() < 2) {
                    fprintf(stderr,"LR.cpp rule incomplete\n");
                    exit(1);
                }
                // add new rule to the closure
                if (rules_vec[0] == expelem) {
                    string newrule = expelem+":dott";
                    for (int i = 1; i < int(rules_vec.size());i++ ) {
                        newrule += ",";
                        newrule += rules_vec[i];
                    }
                    if (!itemSet.count(newrule)) {
                        itemSet.insert(newrule);
                        tempque.push(newrule);
                    }
                }
            }
        }
    // end of while
    }
    return 0;
}
/***********************************************************************
 *use goto function to generate new itemset
 ***********************************************************************/
int SimpleLR:: GOTO(std::tr1::unordered_set<string>&from_itemSet, std::tr1::unordered_set<string>&to_itemSet, string Symbol) {
    for (std::tr1::unordered_set<string>::iterator it = from_itemSet.begin(); 
    it!=from_itemSet.end();it++) {
            vector<string>rules_vec;
            SplitString((*it),rules_vec,":,");
            if (rules_vec.size() < 2) {
                fprintf(stderr,"rule incomplete\n");
                exit(1);
            }
            int dottindex = -1;
            int i = 1;
            int wantflag = 0;
            for (;i<int(rules_vec.size());i++) {
               if (rules_vec[i]=="dott" && i < int(rules_vec.size()-1)  && rules_vec[i+1]== Symbol) {
                   wantflag = 1;
                   dottindex = i;
               }
            }
            if (1 == wantflag) {
                string newrule = rules_vec[0]+":";
                for (i = 1;i <int(rules_vec.size());i++) {
                    if (i == dottindex ) {
                        newrule += rules_vec[i+1];
                    }
                    else if (i == dottindex + 1) {
                        newrule +="dott";
                    }
                    else {
                        newrule += rules_vec[i];
                    }
                    if (i < int(rules_vec.size() - 1)) {
                        newrule += ",";
                    }
                    
                // end of for    
                }
                to_itemSet.insert(newrule);
            // end of if wantflag;
            }

    //end of for;
    }
    return 0;
}

/*******************************************************
 *generate all the states of SLR Automation
 * *****************************************************/
int SimpleLR::GenerateItemSets() {
    std::tr1::unordered_set<string> itemSet;
    //initial itemset
    string initialstr="Start:dott,"+StartSymbol;
    itemSet.insert(initialstr);
    // construct closure
    Closure(itemSet);
    m_closures.push_back(itemSet);
    queue<std::tr1::unordered_set<string> > ques;
    ques.push(itemSet);
    while (! ques.empty()) {
        itemSet = ques.front();
        ques.pop();
        for (std::tr1::unordered_set<string>::iterator it = m_symbols.begin(); it != m_symbols.end();it++) {
            std::tr1::unordered_set<string> to_itemSet;
            GOTO(itemSet, to_itemSet,*it);
            if (to_itemSet.size() > 0) {
                bool flag = false;
                Closure(to_itemSet);
                for (vector<std::tr1::unordered_set<string> >::iterator closure_it = m_closures.begin(); closure_it != m_closures.end(); closure_it++) {
                    if (to_itemSet.size() == (*closure_it).size()) {
                        bool same = true;
                        for (std::tr1::unordered_set<string>:: iterator subit = to_itemSet.begin(); subit != to_itemSet.end(); subit++ ) {
                            if (!(*closure_it).count(*subit)) {
                                same = false;
                                break;
                            }
                        }
                        if (same) {
                            flag = true;
                            break;
                        }
                    }
                }
                // add itemSet to closures;
                if (!flag) {
                    m_closures.push_back(to_itemSet);
                    ques.push(to_itemSet);
                }
            }
        // end of for scan nonterminals;
        }

// end of while
    }
    return 0;
}
// set the values of states_count, terms_count, Nonterms_count;
int SimpleLR::GetCount() {
    m_states_count = m_closures.size();
    m_terms_count = m_terminals.size();
    m_nonterms_count = m_nonterminals.size();
    return 0;
}
//get the itemset's next edge
int SimpleLR::NextEdge() {
  for (int i = 0;i < m_states_count; i++) {
      std::tr1::unordered_set<string> itemSet;
      itemSet =m_closures[i];
      vector<string> gotolist;
      for (std::tr1::unordered_set<string>::iterator it = itemSet.begin(); it != itemSet.end(); it++) {
          vector<string>rules_vec;
          SplitString((*it),rules_vec,":,");
          if (rules_vec.size() < 2) {
              fprintf(stderr,"rule incomplete\n");
              exit(1);
          }
          int k = 1;
          for (;k<int(rules_vec.size());k++) {
              if (rules_vec[k]=="dott" && k < int(rules_vec.size()-1)) {
                  gotolist.push_back(rules_vec[k+1]);
              }
          }
      }
      string newline="";
      for (int j = 0; j < int(gotolist.size());j++) {
          newline += gotolist[j];
          if (j < int(gotolist.size() - 1)) {
              newline+=":";
          }
      }
      if (gotolist.size() > 0) {
        m_closureNext.insert(make_pair(i,newline));
      }
  }
  return 0;
}
//construct GOTO and ACTION TABLE
int SimpleLR::ConstructTables() {
    //Initiallize ActionTable
    ActionTable = new char**[m_states_count];
    char temp[ACTION_TABLE_LEN] = {0};
    string acc_rule="Start:"+StartSymbol+","+"dott";
    memset(ActionTable,0, m_states_count*sizeof(char**));
    for (int i = 0; i < m_states_count;i++) {
        ActionTable[i] = new char* [m_terms_count + 1];
        memset(ActionTable[i],0,(m_terms_count+1)*sizeof(char*));
        for (int j = 0; j < m_terms_count + 1; j++) {
            ActionTable[i][j] = new char [ACTION_TABLE_LEN];
            memset(ActionTable[i][j],0,ACTION_TABLE_LEN); 
        }
    }
    //Initiallize GoToTable
    GoToTable = new int* [m_states_count];
    memset(GoToTable,0,m_states_count*sizeof(int*));
    for (int i = 0;i< m_states_count;i++) {
        GoToTable[i] = new int [m_nonterms_count];
        memset(GoToTable[i],-1,m_nonterms_count*sizeof(int));
    }
    // construct tables;
    for (int i = 0;i < m_states_count;i++) {
        std::tr1::unordered_set<string>  itemSet;
        itemSet = m_closures[i];
        //actiontable
        // add entries to actiontable, acc and reduce rules included
        if (m_closures[i].count(acc_rule)) {
            if (0==strcmp(ActionTable[i][m_terms_count],""))
            {
                strcpy(ActionTable[i][m_terms_count],"acc");
            }
            else {
                char temp2[ACTION_TABLE_LEN]={0};
                vector<string> myvectemp;
                strcpy(temp2,ActionTable[i][m_terms_count]);
                SplitString(temp2,myvectemp,",");
                bool toadd=true;
                for (vector<string>::iterator itt = myvectemp.begin();itt!=myvectemp.end();itt++) {
                    if (strcmp((*itt).c_str(),"acc")==0) {
                        toadd=false;
                        break;
                    }
                }
                if (toadd) {
                    strncat(ActionTable[i][m_terms_count],",acc",4);
                }
            }
        }
        for (std::tr1::unordered_set<string,int>::iterator it = m_closures[i].begin(); it != m_closures[i].end(); it++ ) {

            vector<string>rules_vec;
            SplitString(*it,rules_vec,":,");
            if (rules_vec.size() < 2){
                continue;
            }
            bool isdottend = false;
            string rule_prototype = "";
            string head = rules_vec[0];
            rule_prototype = head+":";
            for (int j = 1; j < int(rules_vec.size());j++) {
                if (rules_vec[j] != "dott") {
                    rule_prototype += rules_vec[j];
                }
                if (j < int(rules_vec.size()-2)) {
                    rule_prototype += ",";
                }
                if (rules_vec[j]=="dott"&&j==int(rules_vec.size()-1)&& head!="Start") {
                    isdottend = true;
                }
                if (isdottend) {
                    if (m_followset.count(head)) {
                        string followstr = m_followset[head];
                        vector<string> follow_vec;
                        SplitString(followstr,follow_vec,",");
                        int rule_id = -1;
                        if (m_rules.count(rule_prototype)) {
                            rule_id=m_rules[rule_prototype];
                        }
                        for (vector<string>::iterator subit=follow_vec.begin(); subit!=follow_vec.end(); subit++) {
                            char buf[LINE_LEN_2] = {0};
                            sprintf(buf,"r%d",rule_id);
                            if (*subit == "$") {
                                if (0 == strcmp(ActionTable[i][m_terms_count],"")) {
                                    strcpy(ActionTable[i][m_terms_count],buf);
                                }
                                else {
                                    char temp2[ACTION_TABLE_LEN]={0};
                                    vector<string> myvectemp;
                                    strcpy(temp2,ActionTable[i][m_terms_count]);
                                    SplitString(temp2,myvectemp,",");
                                    bool toadd=true;
                                    for (vector<string>::iterator itt = myvectemp.begin();itt!=myvectemp.end();itt++) {
                                        if (strcmp((*itt).c_str(),buf)==0) {
                                            toadd=false;
                                            break;
                                        }
                                    }
                                    if (toadd) {
                                        strncat(ActionTable[i][m_terms_count],",",1);
                                        strncat(ActionTable[i][m_terms_count],buf,strlen(buf));
                                    }

                                }
                            }
                            if (m_terminals.count(*subit)){
                                if (0 == strcmp(ActionTable[i][m_terminals[*subit]],"")) {
                                    strcpy(ActionTable[i][m_terminals[*subit]],buf);
                                }
                                else {
                                    char temp2[ACTION_TABLE_LEN]={0};
                                    vector<string> myvectemp;
                                    strcpy(temp2,ActionTable[i][m_terminals[*subit]]);
                                    SplitString(temp2,myvectemp,",");
                                    bool toadd=true;
                                    for (vector<string>::iterator itt = myvectemp.begin();itt!=myvectemp.end();itt++) {
                                        if (strcmp((*itt).c_str(),buf)==0) {
                                            toadd=false;
                                            break;
                                        }
                                    }
                                    if (toadd) {
                                        strncat(ActionTable[i][m_terminals[*subit]],",",1);
                                        strncat(ActionTable[i][m_terminals[*subit]],buf,strlen(buf));
                                    }
                                }
                                /*
                                if (0 != strcmp(ActionTable[i][m_terminals[*subit]],"")) {
                                    strncat(ActionTable[i][m_terminals[*subit]],",",1);
                                }
                                strncat(ActionTable[i][m_terminals[*subit]],buf,strlen(buf));
                                */
                            }
                        }
                        // end of check followset
                    }
                    // end of check isdottend  
                }
                // end of  check rules_vec
            }
            // end of traverse closures[i]
        }

        vector<string> nextvariables;
        if (m_closureNext.count(i)) {
            SplitString(m_closureNext[i],nextvariables,":");
        }
        for (vector<string>::iterator it = nextvariables.begin(); it != nextvariables.end();it++) {
            // contruct action table;
            if (m_terminals.count(*it)) {
                std::tr1::unordered_set<string> toItemSet;
                GOTO(itemSet, toItemSet,*it);
                if (toItemSet.size() > 0) {
                    for (int j = 0; j< m_states_count; j++) {
                        bool inflag = true;
                        for (std::tr1::unordered_set<string>::iterator subit = toItemSet.begin(); subit != toItemSet.end(); subit++) {
                            if (!m_closures[j].count(*subit)) {
                                inflag = false;
                                break;
                            }
                        }
                        if (inflag) {
                            memset(temp,0,ACTION_TABLE_LEN);
                            sprintf(temp,"s%d",j);
                            if (0 == strcmp(ActionTable[i][m_terminals[*it]],"")) { 
                                strcpy(ActionTable[i][m_terminals[*it]],temp);
                            }
                            else {
                                char temp2[ACTION_TABLE_LEN]={0};
                                vector<string> myvectemp;
                                strcpy(temp2,ActionTable[i][m_terminals[*it]]);
                                SplitString(temp2,myvectemp,",");
                                bool toadd=true;
                                for (vector<string>::iterator itt = myvectemp.begin();itt!=myvectemp.end();itt++) {
                                    if (strcmp((*itt).c_str(),temp)==0) {
                                        toadd=false;
                                        break;
                                    }
                                }
                                if (toadd) {
                                    strncat(ActionTable[i][m_terminals[*it]],",",1);
                                    strncat(ActionTable[i][m_terminals[*it]],temp,strlen(temp));
                                }
                            }
                        }
                    }
                }
            // end of terminals
            }
        if (m_nonterminals.count(*it)) {
            std::tr1::unordered_set<string> toItemSet;
            GOTO(itemSet, toItemSet,*it);
            if (toItemSet.size() > 0) {
                for (int j = 0; j< m_states_count; j++) {
                    bool inflag = true;
                    for (std::tr1::unordered_set<string>::iterator subit = toItemSet.begin(); 
                        subit != toItemSet.end(); subit++) {
                        if (!m_closures[j].count(*subit)) {
                            inflag = false;
                            break;
                        }
                    }
                    if (inflag) {
                        GoToTable[i][m_nonterminals[*it]] = j;
                    }
                }
            }
        //end of Nonterms
        }
        
    // end of next variables;
    }
 // end of traverse closures    
 }
 return 0;
}
