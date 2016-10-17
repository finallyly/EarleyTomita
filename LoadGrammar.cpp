/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：loadGrammar.cpp
*   创 建 者：unicodeproject
*   创建日期：2016年07月19日
*   描    述：
*   备    注: 
*   更新日志：
*
================================================================*/
 
#include "Grammar.h"
#include <tr1/unordered_map>
#include <tr1/unordered_set>

// please add your code here!
// return 0 success 1 failure
int Grammar::LoadGrammarRules(const char* path) {
    FILE *fin = NULL;
    char line[LINE_LEN_1]={0};
    fin = fopen(path,"r");
    if (NULL == fin) {
        fprintf(stderr,"can not open %s\n",path);
        return 1;
    }
    int id = 0;
    while (fgets(line,LINE_LEN_1,fin)){
        size_t len = strlen(line);
        if (line[len-1] != '\n') {
            continue;
        }
        line[len-1]=0;
        if (!m_rules.count(line)) {
            m_rules.insert(make_pair(line,id));
            m_id2rules.insert(make_pair(id,line));
            id++;
        }
    }

    if (fin != NULL) {
        fclose(fin);
        fin = NULL;
    }
    return 0;
}
int Grammar::LoadNonterminals(const char* path) {
    FILE *fin = NULL;
    char line[LINE_LEN_1]={0};
    fin = fopen(path,"r");
    if (NULL == fin) {
        fprintf(stderr,"can not open %s\n",path);
        return 1;
    }
    int id = 0;
    while (fgets(line,LINE_LEN_1,fin)){
        size_t len = strlen(line);
        if (line[len-1] != '\n') {
            continue;
        }
        line[len-1] = 0;
        if (!m_nonterminals.count(line)) {
            m_nonterminals.insert(make_pair(line,id));
            id++;
        }
    }
    if (fin != NULL) {
        fclose(fin);
        fin = NULL;
    }
    return 0;
}

int Grammar::LoadTerminals(const char * path) {
    FILE *fin = NULL;
    char line[LINE_LEN_1]={0};
    fin = fopen(path,"r");
    if (NULL == fin) {
        fprintf(stderr,"can not open %s\n",path);
        return 1;
    }
    int id = 0;
    while (fgets(line,LINE_LEN_1,fin)) {
        size_t len = strlen(line);
        if (line[len-1] != '\n') {
            continue;
        }
        line[len-1]=0;
        if (!m_terminals.count(line)) {
            m_terminals.insert(make_pair(line,id));
            id++;
        }
    }

    if (fin != NULL) {
        fclose(fin);
        fin = NULL;
    }
    return 0;
}
// join the terminals and nonterminals together
int Grammar::GetSymbols() {
    for (std::tr1::unordered_map<string,int>::iterator it = m_nonterminals.begin();it != m_nonterminals.end(); it++ ){
        m_symbols.insert(it->first);
    }
    for (std::tr1::unordered_map<string,int>::iterator it = m_terminals.begin();it != m_terminals.end(); it++ ){
        m_symbols.insert(it->first);
    }
    return 0;
}
int Grammar::LoadVocab(const char* path) {
    FILE *fin = NULL;
    char line[LINE_LEN_1]={0};
    fin = fopen(path,"r");
    if (NULL == fin) {
        fprintf(stderr,"can not open %s\n",path);
        return 1;
    }
    while (fgets(line,LINE_LEN_1,fin)){
        size_t len = strlen(line);
        if (line[len-1] != '\n') {
            continue;
        }
        line[len-1]=0;
        char *ptr=strchr(line,':');
        char temp1[LINE_LEN_2]={0};
        char temp2[LINE_LEN_2]={0};
        strncpy(temp1,line,ptr-line);
        strncpy(temp2,ptr+1,line+len-1-ptr);
        if (m_morphmap.count(temp2)) {
            m_morphmap[temp2].push_back(temp1);
        }
        else {
            vector<string> tempvec;
            tempvec.push_back(temp1);
            m_morphmap[temp2]=tempvec;
        }
        //m_morphmap.insert(make_pair(temp2,temp1));
    }
    if (fin != NULL) {
        fclose(fin);
        fin = NULL;
    }
    return 0;
}
// go get the elem which  is the first elem in the rule body given the head
int Grammar::GetFirstElemInRuleBody(string head,std::tr1::unordered_set<string> &elems) {
    for (std::tr1::unordered_map<string,int>::iterator it = m_rules.begin(); it != m_rules.end();it++) {
        vector<string> rules_vec;
        SplitString(it->first, rules_vec,":,");
        if (rules_vec.size()<2) {
            continue;
        }
        // if this elem is equal to the head ,do not add to the set;
        if (rules_vec[0] == head && rules_vec[1] != head) {
            elems.insert(rules_vec[1]);
        }
    }
    /* for debug
    for (std::tr1::unordered_set<string>::iterator it =elems.begin(); it != elems.end();it++) {
        fprintf(stdout,"%s\n",(*it).c_str());
    }
    */
    return 0;
}
// to check if all the elem in the given set a terminals
bool Grammar::isAllTerminals(std::tr1::unordered_set<string> &itsset, vector<string>&nontermvec) {
    bool flag = true;
    for (std::tr1::unordered_set<string>::iterator it = itsset.begin(); it !=itsset.end(); it++) {
        if (m_nonterminals.count(*it)) {
            flag = false;
            nontermvec.push_back(*it);
        }
    }
    return flag;
}

// do not take epsilon into consideration
int Grammar::GetSingleFirst(string head,string &firststr){
    std::tr1::unordered_set<string> elems;
    vector<string>nontermvec;
    vector<string>tempvec;
    GetFirstElemInRuleBody(head,elems);
    bool flag = isAllTerminals(elems,nontermvec);
    // if all is terminals
    if (flag) {
        int i = 0;
        for (std::tr1::unordered_set<string>::iterator it = elems.begin(); it != elems.end();it++) {
            firststr += *it;
            if (i<int(elems.size() -1)) {
                firststr += ",";
            }
            i++;
        }
    }
    else {
        // headsInStack to avoid circle
        std::tr1::unordered_set<string> headsInStack;
        StackNode* stNode = NULL;
        StackNode* stNodeOld = NULL;
        stNode = new StackNode;
        stNode->key = head;
        stNode->elemlist = new char [LINE_LEN_3];
        stNode->symbollist = new char [LINE_LEN_4];
        stNode->visited = new int [nontermvec.size()];
        memset(stNode->visited, 0, nontermvec.size()*sizeof(int));
        memset(stNode->symbollist, 0, LINE_LEN_4);
        memset(stNode->elemlist,0,LINE_LEN_3);
        for (int i = 0; i < int(nontermvec.size()); i++) {
            strncat(stNode->elemlist,nontermvec[i].c_str(), nontermvec[i].size());
            if (i < int(nontermvec.size() - 1) ) {
                 strncat (stNode->elemlist,",",1);
            }
        }
        for (std::tr1::unordered_set<string>::iterator it = elems.begin();it != elems.end();it++) {
            if (m_nonterminals.count(*it)) {
                continue;
            }
            strncat(stNode->symbollist, (*it).c_str(), (*it).size());
            strncat(stNode->symbollist,",",1);
        }
        stack<StackNode*> mystack;
        mystack.push(stNode);
        headsInStack.insert(stNode->key);
        while (!mystack.empty()) {
            elems.clear();
            nontermvec.clear();
            tempvec.clear();
            stNode = mystack.top();
            SplitString(stNode->elemlist,tempvec,",");
            // find first unvisited descendant
            int index = -1;
            for (int i = 0; i < int(tempvec.size()); i++) {
                if (stNode->visited[i] == 0) {
                    index = i;
                    break;
                }
            }
            // all the descendants of teh top elem has been visited
            if (index == -1) {
                stNodeOld = stNode->parent;
                if (stNodeOld != NULL) {
                    strncat(stNodeOld->symbollist, stNode->symbollist, strlen(stNode->symbollist));
                    vector<string> oldtempvec;
                    SplitString(stNodeOld->elemlist,oldtempvec,",");
                    int oldindex = 0;
                    for (;oldindex<int(oldtempvec.size());oldindex++) {
                        if (oldtempvec[oldindex] == stNode->key) {
                            break;
                        }
                    }
                    stNodeOld->visited[oldindex]=1;
                 }
                 if (mystack.size()==1) {
                     firststr.assign(stNode->symbollist);
                 }
                delete stNode;
                mystack.pop();
            }
            else {
                    string deshead = tempvec[index];
                    elems.clear();
                    nontermvec.clear();
                    GetFirstElemInRuleBody(deshead,elems);
                    flag = isAllTerminals(elems,nontermvec);
                    string temp="";
                    for (std::tr1::unordered_set<string>::iterator it = elems.begin(); it!=elems.end(); it++) {
                        if (m_nonterminals.count(*it)) {
                            continue;
                        }
                        temp+=*it;
                        temp+=",";
                    }
                    if (flag) {
                      stNode->visited[index] = 1;
                      strncat(stNode->symbollist,temp.c_str(),strlen(temp.c_str()));
                    }
                    else {
                         bool isIn = false;
                          if (headsInStack.count(deshead)) {
                                isIn = true;
                                stNode->visited[index] =1;
                                strncat(stNode->symbollist,temp.c_str(),strlen(temp.c_str()));
                         }
                         if (!isIn) {
                            StackNode *newNode = NULL;
                            newNode = new StackNode;
                            newNode->key = deshead;
                            newNode->elemlist = new char [LINE_LEN_3];
                            newNode->symbollist = new char [LINE_LEN_4];
                            newNode->visited = new int [nontermvec.size()];
                            memset(newNode->visited, 0 ,nontermvec.size()*sizeof(int));
                            memset(newNode->elemlist, 0 ,LINE_LEN_3);
                            memset(newNode->symbollist, 0 ,LINE_LEN_4);
                            for (int i = 0 ; i < int(nontermvec.size());i++) {
                                strncat(newNode->elemlist,nontermvec[i].c_str(), nontermvec[i].size());
                                if (i < int(nontermvec.size() - 1)) {
                                    strncat (newNode->elemlist,",",1);
                                }
                            }
                            strncat(newNode->symbollist,temp.c_str(),strlen(temp.c_str()));
                            newNode->parent = stNode;
                            mystack.push(newNode);
                            headsInStack.insert(newNode->key);
                        // endof isIn
                         }
                    // end of else
                    }
            }
        //end of while stack
        }

   // end of else
    }
    return 0;
}

int Grammar::GetFirstSet() {
    for (std::tr1::unordered_map<string,int>::iterator it = m_nonterminals.begin(); it != m_nonterminals.end(); it++) {
        vector<string> tempvec;
        std::tr1::unordered_set<string> tempset;
        string firststr;
        GetSingleFirst(it->first,firststr);
        SplitString(firststr,tempvec,",");
        for (vector<string>::iterator it2 = tempvec.begin(); it2 != tempvec.end(); it2++) {
            tempset.insert(*it2);
        }
        firststr = "";
        int count = 0;
        for (std::tr1::unordered_set<string>::iterator it2 = tempset.begin(); it2 != tempset.end(); it2++) {
            firststr += *it2;
            if (count < int(tempset.size() - 1)) {
                firststr += ",";
            }
            count++;
        }
        m_firstset.insert(make_pair(it->first, firststr));
    }
    return 0;
}
int Grammar::GetHeadAndNeighborSet(string bodyelem, std::tr1::unordered_set<string> &headelems,std::tr1::unordered_set<string> &neighborelems) {
    for (std::tr1::unordered_map<string,int>::iterator it = m_rules.begin(); it != m_rules.end();it++) {
        vector<string> rules_vec;
        SplitString(it->first, rules_vec,":,");
        if (rules_vec.size()<2) {
            continue;
        }
        // if this elem is equal to the head ,do not add to the set;
        for (int i = 1; i< int(rules_vec.size());i++) {
            if (rules_vec[i] == bodyelem && rules_vec[0] != bodyelem && i == int(rules_vec.size()-1)) {
                headelems.insert(rules_vec[0]);
                
            }
            if (rules_vec[i] == bodyelem && i<int(rules_vec.size()-1)) {
                neighborelems.insert(rules_vec[i+1]);
            }
        }
    }
    return 0;
}

/****************************************************
 *  calc the given elem's follow set;
 *  elem must be nonterminals
 ***************************************************/
int Grammar::GetSingleFollow(string elem, string &followstr) {
    std::tr1::unordered_set<string> headelems;
    std::tr1::unordered_set<string> neighborelems;
    GetHeadAndNeighborSet(elem,headelems,neighborelems);
    followstr = "";
    string temp="";
    for (std::tr1::unordered_set<string>::iterator it = neighborelems.begin(); it != neighborelems.end(); it++){
        if (m_nonterminals.count(*it)) {
            temp+=m_firstset[*it];
            temp+=",";
        }
        else {
            temp+=*it;
            temp+=",";
        }
    }
    if (elem==StartSymbol) {
        temp += "$";
        temp += ",";
    }
    if (headelems.size() > 0) {
        std::tr1::unordered_set<string> keyInStack;
        StackNode *stNode = NULL;
        StackNode *stNodeOld = NULL;
        stNode = new StackNode;
        stNode->key = elem;
        stNode->symbollist = new char [LINE_LEN_4];
        stNode->elemlist = new char [LINE_LEN_3];
        stNode->visited = new int [headelems.size()];
        memset(stNode->symbollist,0,LINE_LEN_4);
        memset(stNode->elemlist,0,LINE_LEN_3);
        memset(stNode->visited,0,sizeof(int)*headelems.size());
        strncpy(stNode->symbollist,temp.c_str(),strlen(temp.c_str()));
        int count = 0;
        for (std::tr1::unordered_set<string>:: iterator it = headelems.begin(); it != headelems.end();it++) {
            strncat(stNode->elemlist,(*it).c_str(),(*it).size());
            if (count < int(headelems.size()-1)) {
                strncat(stNode->elemlist,",",1);
            }
            count++;
        }
        stack<StackNode*> mystack;
        mystack.push(stNode);
        keyInStack.insert(stNode->key);
        while (!mystack.empty()) {
            stNode = mystack.top();
            vector<string> elemvec;
            SplitString(stNode->elemlist,elemvec,",");
            int index = -1;
            for (int i = 0; i<int(elemvec.size());i++) {
                if (stNode->visited[i]==0) {
                    index = i;
                    break;
                }
            }
            // all the children of the topelem has been visited
            if (-1==index) {
                stNodeOld = stNode->parent;
                if (stNodeOld != NULL) {
                    strncat(stNodeOld->symbollist,stNode->symbollist,strlen(stNode->symbollist));
                    vector<string> oldelemvec;
                    SplitString(stNodeOld->elemlist,oldelemvec,",");
                    for (int i=0; i<int(oldelemvec.size());i++) {
                        if (oldelemvec[i]==stNode->key) {
                            stNodeOld->visited[i] = 1;
                        }
                    }
                }
                if (mystack.size()==1) {
                    followstr.assign(stNode->symbollist);
                }
                delete stNode;
                mystack.pop();
            }
            else {
                neighborelems.clear();
                headelems.clear();
                string key=elemvec[index];
                GetHeadAndNeighborSet(key,headelems,neighborelems);
                string tempstr="";
                for (std::tr1::unordered_set<string>::iterator it = neighborelems.begin(); it != neighborelems.end();it++) {
                    if (m_nonterminals.count(*it)) {
                        tempstr += m_firstset[*it];
                        tempstr += ",";
                    } 
                    else {
                        tempstr += *it;
                        tempstr += ",";
                    }
                }
                if (key==StartSymbol) {
                    tempstr += "$";
                    tempstr += ",";
                }
                if (headelems.size() > 0) {
                    if (!keyInStack.count(key)) {
                        StackNode* newnode=NULL;
                        newnode = new StackNode;
                        newnode->key = key;
                        newnode->visited= new int [headelems.size()];
                        newnode->elemlist = new char [LINE_LEN_3];
                        newnode->symbollist = new char [LINE_LEN_4];
                        memset(newnode->symbollist,0,LINE_LEN_4);
                        memset(newnode->elemlist,0,LINE_LEN_3);
                        memset(newnode->visited,0,sizeof(int)*headelems.size());
                        strncpy(newnode->symbollist,tempstr.c_str(),strlen(tempstr.c_str()));
                        int count = 0;
                        for (std::tr1::unordered_set<string>:: iterator it = headelems.begin(); 
                                it != headelems.end();it++) {
                            strncat(newnode->elemlist,(*it).c_str(),(*it).size());
                            if (count < int(headelems.size()-1)) {
                                strncat(newnode->elemlist,",",1);
                            }
                            count++;
                        }
                        newnode->parent=stNode;
                        keyInStack.insert(newnode->key);
                        mystack.push(newnode);
                    }
                    else {
                        stNode->visited[index] = 1;
                        strncat(stNode->symbollist,tempstr.c_str(),strlen(tempstr.c_str()));
                    }
                }
                else {
                    strncat(stNode->symbollist,tempstr.c_str(),strlen(tempstr.c_str()));
                    stNode->visited[index] = 1;
                }

            // end of else
            }
        // end of while    
        }
    //end of if     
    } 
    else {
        followstr = temp;
    }
  return 0;
}

int Grammar::GetFollowSet() {
    int index = 0;
    for (std::tr1::unordered_map<string,int>::iterator it = m_nonterminals.begin(); it != m_nonterminals.end(); it++) {
        vector<string> tempvec;
        std::tr1::unordered_set<string> tempset;
        string followstr;
        GetSingleFollow(it->first,followstr);
        SplitString(followstr,tempvec,",");
        index++;
        for (vector<string>::iterator it2 = tempvec.begin(); it2 != tempvec.end(); it2++) {
            tempset.insert(*it2);
        }
        followstr = "";
        int count = 0;
        for (std::tr1::unordered_set<string>::iterator it2 = tempset.begin(); it2 != tempset.end(); it2++) {
            followstr += *it2;
            if (count < int(tempset.size() - 1)) {
                followstr += ",";
            }
            count++;
        }
        m_followset.insert(make_pair(it->first, followstr));
    }
    return 0;
}
