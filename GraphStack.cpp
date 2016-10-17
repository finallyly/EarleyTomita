/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：GraphStack.cpp
*   创 建 者：unicodeproject
*   创建日期：2016年08月10日
*   描    述：
*   备    注: 
*   更新日志：
*
================================================================*/
 
#include "GraphStack.h"
// please add your code here!
 GraphStack::GraphStack( ) {
    head = new GraphStackNode();
    head->stateId = 0;
    head->next = NULL;
    head->infirstedge = NULL;
    head->outfirstedge = NULL;
    ahead = new ActiveNode;
    ahead->gnode=head;
    ahead->next=NULL;
    atail=ahead;
    contentoffset = 0;
    contentlist = new ContentNode[CONTENT_NODE_COUNT];

}
GraphStack::~GraphStack() {
   clear();
}
void GraphStack::clear( ) {
    GraphStackNode *p = NULL;
    p = head;
    queue<GraphStackNode*> myque;
    stack<GraphStackNode*> mystack;
    // use que to  update the current edgeinfo of the graph 
    //p=head;
    //
    if (p!=NULL) {
        if (p->visited != NULL) {
            delete p->visited;
            p->visited = NULL;
        }
        myque.push(p);
        while (!myque.empty()) {
            p = myque.front();
            AdjustOutedgeCount(p);
            myque.pop();
            if (p->outedgecount>0) {
                if (p->visited==NULL) {
                    p->visited = new int [p->outedgecount];
                    memset(p->visited,0, (p->outedgecount)*sizeof(int));
                }
                EdgeNode *edge = p->outfirstedge;
                while (edge) {
                    GraphStackNode *child = edge->destvertex;
                    if(child->visited==NULL){
                        myque.push(child);
                    }
                    edge = edge->next;
                    //end of edge
                }
                // end of p->outedgecount>0
            }
            // end of myque.empty()
        }
        // deconstruct the residual  stack
        p=head;
        mystack.push(head);
        while (!mystack.empty()) {
            p = mystack.top();
            int visitedcount = 0;
            AdjustOutedgeCount(p);
            for (int i=0; i<p->outedgecount;i++) {
                if ((p->visited)[i]==1) {
                    visitedcount++;
                }
            }
            if (visitedcount==p->outedgecount) {
                EdgeNode *edge = p->infirstedge;
                while (edge) {
                    GraphStackNode *node = edge->srcvertex;
                    EdgeNode *edge2 = node->outfirstedge;
                    AdjustOutedgeCount(node);
                    int j=0;
                    while (edge2) {
                        if (node->visited[j]==0&&edge2->destvertex==p) {
                            node->visited[j]=1;
                            break;
                        }
                        j++;
                        edge2 = edge2->next;
                        // end of while edge2
                    }
                    edge = edge->next;
                    // end of while edge;
                }
                // end of visitedcount==p->outedgecount;
                DeleteAllInEdge(p);
                DeleteAllOutEdge(p);
                if (p->visited!=NULL) {
                    delete [] (p->visited);
                }
                delete p;
                p=NULL;
                mystack.pop();
            }
            else {
                int i;
                for(i=0;i<p->outedgecount;i++) {
                    if ((p->visited)[i]==0) {
                        break;
                    }
                }
                EdgeNode *edge = p->outfirstedge;
                int j=0;
                while(edge) {
                    if (j==i) {
                        mystack.push(edge->destvertex);
                        break;
                    }
                    j++;
                    edge=edge->next;
                }

                // end of else;
            }
            // end of while mystack
        }

        //end of test head    
    }
    head = NULL;
    if (contentlist) {
        delete [] contentlist;
    }
    contentlist = NULL;
    ActiveNode *ap = ahead;
    ActiveNode *apold = NULL;
    while (ap) {
        apold = ap;
        ap=ap->next;
        delete apold;
    }
    ahead = NULL;
    atail = NULL;
}
// a node given ,find
vector<PathNode*> GraphStack::GetPathByLength(GraphStackNode *node, int nlen) {
    vector<PathNode*> node_vec;
    stack<GraphStackNode*> mystack;
    AdjustInedgeCount(node);
    if (node->visited==NULL) {
        node->visited = new int [node->inedgecount];
    }
     memset(node->visited,0, node->inedgecount*sizeof(int));
     node->npath = 1;
     node->parent = NULL;
     mystack.push(node);
     while (!mystack.empty()) {
        GraphStackNode* topelem = mystack.top();
        bool bFinish = true;
        for (int i = 0; i< topelem->inedgecount; i++) {
            if ((topelem->visited)[i]==0) {
                bFinish = false;
                break;
            }
        }
        if ((topelem->npath) == nlen) {
            EdgeNode *edge= NULL;
            if (topelem->parent != NULL) {
                int i = 0;
                edge = (topelem->parent)->infirstedge;
                while(edge) {
                    if ((edge->srcvertex)==topelem) {
                            break;
                    }
                    edge = edge->next;
                    i++;
                // end of traverse edge
                }
                ((topelem->parent)->visited)[i] = 1;
            // end of topelem->parent!=NULL
            }
            PathNode *pathnode = new PathNode;
            PathNode *p1 = pathnode;
            GraphStackNode *p = topelem;
            pathnode->gnode = topelem;
            pathnode->next = NULL;
            p=p->parent;
            while (p) {
                PathNode* p2 = new PathNode;
                p2->gnode = p;
                p2->next = NULL;
                p1->next = p2;
                p2->prev = p1;
                p=p->parent;
                p1=p1->next;
            }
            node_vec.push_back(pathnode);
        // end of (topelem->npath) == nlen
        }
        if (bFinish) {
            if (topelem->parent != NULL) {
                int i = 0;
                EdgeNode *edge = (topelem->parent)->infirstedge;
                while(edge) {
                    if ((edge->srcvertex)==topelem) {
                            break;
                    }
                    edge = edge->next;
                    i++;
                // end of traverse edge
                }
                ((topelem->parent)->visited)[i] = 1;
            // end of topelem->parent!=NULL
            }
        }
        if ((topelem->npath)== nlen || bFinish){
            if ((topelem->visited)!=NULL) {
                delete [] (topelem->visited);
                (topelem->visited) = NULL;
            }
            mystack.pop();
        // end of if;
        }
        else {
            EdgeNode* edge = (topelem->infirstedge);
            GraphStackNode *newnode = NULL;
            int i = 0;
            while (edge) {
                if ((topelem->visited[i])==0) {
                    newnode = edge->srcvertex; 
                    break;
                }
                edge = edge->next;
                i++;
                // end of traverse edge
            }
            newnode->npath = (topelem->npath)+1;
            newnode->parent = topelem;
            AdjustInedgeCount(newnode);
            if (newnode->visited == NULL) {
                newnode->visited = new int [newnode->inedgecount];
            }
            memset(newnode->visited,0, newnode->inedgecount*sizeof(int));
            mystack.push(newnode); 
        // end of else
        }
     // end of mystack    
     }
     return node_vec;
}
string GraphStack::GetContentString(GraphStackNode*node,PathNode*path) {
    char temp[LINE_LEN_3]={0};
    char idtemp[LINE_LEN_2]={0};
    PathNode *p1 = path;
    GraphStackNode *p2 =node;
    EdgeNode *edge = node->outfirstedge;
    while (edge) {
        if (edge->destvertex ==p1->gnode){
            sprintf(idtemp,"%d",edge->contentid);
            strcat(temp,idtemp);
            break;
        }
        edge=edge->next;
    }
    p2= p1->gnode;
    p1=p1->next;
    while (p1) {
        edge = p2->outfirstedge;
        while (edge) {
            if (edge->destvertex ==p1->gnode){
                sprintf(idtemp,"%d",edge->contentid);
                strcat(temp,",");
                strcat(temp,idtemp);
                break;
            }
            edge=edge->next;
        }
        p2=p1->gnode;
        p1=p1->next;
    }
    string result;
    result.assign(temp);
    return result;

}
ActiveNode* GraphStack::InActiveSet(int stateid) {
    ActiveNode *pwork =ahead;
    ActiveNode *p =NULL;
    while (pwork) {
        if (pwork->gnode!=NULL&&pwork->visited==0&&(pwork->gnode)->stateId==stateid) {
            p=pwork;
            break;
        }
        pwork = pwork ->next;
    }
    return p;
}
void GraphStack::DeleteOutEdge(GraphStackNode*node, GraphStackNode*srcnode,GraphStackNode*destnode,int contentid) {
    EdgeNode *edge = node->outfirstedge;
    EdgeNode* edgeold=NULL;
    while(edge) {
        if (edge->srcvertex == srcnode && edge->destvertex==destnode && edge->contentid==contentid) {
                if (edgeold == NULL) {
                    node->outfirstedge = edge->next;
                }
                else {
                    edgeold->next = edge->next;
                }
                delete edge;
                break;
            }
        edgeold = edge;
        edge=edge->next;
    }
    
}


void GraphStack::DeleteOutEdge(GraphStackNode*node, GraphStackNode*srcnode,GraphStackNode*destnode) {
    EdgeNode *edge = node->outfirstedge;
    EdgeNode* edgeold=NULL;
    EdgeNode* prev = NULL;
    while(edge) {
        edgeold = edge;
        edge=edge->next;
        if (edgeold->srcvertex == srcnode && edgeold->destvertex==destnode) {
                if (prev == NULL) {
                    node->outfirstedge = edgeold->next;
                }
                else {
                    prev->next = edgeold->next;
                }
                delete edgeold;
                break;
        }
        prev = edgeold;
    }
    
}
void GraphStack::DeleteAllInEdge(GraphStackNode *node) {
    EdgeNode *edge = node->infirstedge;
    EdgeNode *edgeold = NULL;
    while (edge) { 
        edgeold = edge;
        edge = edge->next;
        delete edgeold;
    }
}
void GraphStack::DeleteAllOutEdge(GraphStackNode *node) {
    EdgeNode *edge = node->outfirstedge;
    EdgeNode *edgeold = NULL;
    while (edge) { 
        edgeold = edge;
        edge = edge->next;
        delete edgeold;
    }
}
void GraphStack::DeleteInEdge(GraphStackNode*node, GraphStackNode*srcnode,GraphStackNode*destnode,int contentid) {
    EdgeNode *edge = node->infirstedge;
    EdgeNode* edgeold=NULL;
    while(edge) {
        if (edge->srcvertex == srcnode && edge->destvertex==destnode && edge->contentid==contentid) {
                if (edgeold == NULL) {
                    node->infirstedge = edge->next;
                }
                else {
                    edgeold->next = edge->next;
                }
                delete edge;
                break;
            }
        edgeold = edge;
        edge=edge->next;
    }
}
void GraphStack::DeleteInEdge(GraphStackNode*node, GraphStackNode*srcnode,GraphStackNode*destnode) {
    EdgeNode *edge = node->infirstedge;
    EdgeNode* edgeold=NULL;
    while(edge) {
        if (edge->srcvertex == srcnode && edge->destvertex==destnode) {
                if (edgeold == NULL) {
                    node->infirstedge = edge->next;
                }
                else {
                    edgeold->next = edge->next;
                }
                delete edge;
                break;
            }
        edgeold = edge;
        edge=edge->next;
    }
}

void GraphStack:: AddInEdge(GraphStackNode* node, GraphStackNode* srcnode,GraphStackNode* destnode, int contentid) {
    EdgeNode *edge = node->infirstedge;
    EdgeNode *p = new EdgeNode;
    p->srcvertex = srcnode;
    p->destvertex = destnode;
    p->contentid = contentid;
    if (edge==NULL) {
        node->infirstedge = p;
    }
    else {
        while (edge->next) {
            edge=edge->next;
        }
        edge->next=p;
    }
}
/**************************************************************** 
 *calc every node in the path's inedgedgree sum and outedgedgree sum
 *
 ****************************************************************/
void GraphStack::CalcInAndOutWeight(PathNode* path) {
    PathNode* p = path;
    PathNode *pold = NULL;
    int inweight = 0;
    int outweight = 0;
    while (p) {
        if (p==path){
            p->inweight = 0;
            inweight=0;
            p = p->next;
            continue;
        }
        int inedgecount = AdjustInedgeCount(p->gnode);
        inweight += inedgecount;
        p->inweight = inweight;
        pold = p;
        p = p->next;
    }
    p=pold;
    pold = NULL;
    while (p) {
        if (pold==NULL){
            p->outweight = 0;
            outweight = 0;
            pold = p;
            p = p->prev;
            continue;
        }
        int outedgecount = AdjustOutedgeCount(p->gnode);
        outweight += outedgecount;
        p->outweight = outweight;
        pold = p;
        p = p->prev;
    }
}
int  GraphStack::AdjustPathNodeEdges(PathNode *path) {
    PathNode* p = path;
    PathNode *pold = NULL;
    int pathlen =0;
    int flag = 0;
    while (p) {
        pathlen++;
        p=p->next;
    }
    p = path;
    int cur = 0;
    while (p) {
         cur++;
         // the node in path should be processed
         if (p->outweight == pathlen - cur && p->inweight == cur - 1){
            p->toprocess=1;    
         }
         pold = p;
        p = p->next;
    }
    p = pold;
    pold = NULL;
    cur = 0;
    while (p) {
        if (p->toprocess==1) {
            GraphStackNode *pnode = p->gnode;
            EdgeNode *edge = pnode->infirstedge;
            while (edge) {
                GraphStackNode *pnode1=edge->srcvertex;
                DeleteOutEdge(pnode1,pnode1,pnode);
                edge = edge->next;
            }
            edge = pnode->outfirstedge;
            while (edge) {
                GraphStackNode *pnode1=edge->destvertex;
                DeleteInEdge(pnode1,pnode,pnode1);
                edge = edge->next;
            }
            DeleteAllInEdge(pnode);
            DeleteAllOutEdge(pnode);
            if (cur == 0) {
              // DeleteFromActive(pnode,0); 
              flag = 1;
            }
            delete pnode;
            p->gnode = NULL;
        }
        cur++;
        pold = p;
        p = p->prev;
    }
    return flag;
}
/*******************************************************
 * when actionstr is empty 
 ******************************************************/
void GraphStack::AdjustFailureNodeEdges(GraphStackNode* node) {
    EdgeNode *edge = node->infirstedge;
    while (edge) {
        GraphStackNode *pnode1=edge->srcvertex;
        DeleteOutEdge(pnode1,pnode1,node);
        edge = edge->next;
    }
    DeleteAllInEdge(node);
    DeleteAllOutEdge(node);
    if (node!=head) {
        delete node;
        node = NULL;
    }
}

void GraphStack:: AddToActive(GraphStackNode* node) {
    ActiveNode *p = new ActiveNode;
    p->gnode = node;
    atail->next = p;
    atail = p;
}
void GraphStack::DeleteFromActive() {
     if (ahead==NULL) {
         atail = NULL;
         return;
     }
     ActiveNode *p = ahead->next;
     ActiveNode *pold = NULL;
     ActiveNode *prev = ahead;
     while (p) {
            pold = p;
            p=p->next;
         if (pold->visited == 1) {
            prev->next = p;
            delete pold; 
         }
         else {
             prev = prev->next;
         }
     }
     p=ahead;
     while (p->next) {
         p = p->next;
     }
     atail = p;
}
void GraphStack::ClearPath(PathNode *path) {
    PathNode *p = path;
    PathNode *pold = NULL;
    while (p) {
        pold = p;
        p = p->next;
        delete pold;
    }
}
//void GraphStack::AdjustEdge
void GraphStack:: AddOutEdge(GraphStackNode* node, GraphStackNode* srcnode,GraphStackNode* destnode, int contentid) {
    EdgeNode* edge = node->outfirstedge;
    EdgeNode *p = new EdgeNode;
    p->srcvertex = srcnode;
    p->destvertex = destnode;
    p->contentid = contentid;
    if (edge==NULL) {
        node->outfirstedge = p;
    }
    else {
        while (edge->next) {
            edge=edge->next;
        }
        edge->next=p;
    }
}
int GraphStack::AdjustInedgeCount(GraphStackNode *node) {
    int inedgecount = 0;
    EdgeNode * edge =  node->infirstedge;
    while (edge) {
        inedgecount++;
        edge=edge->next;
    }
    node->inedgecount = inedgecount;
    //fprintf(stdout,"stateid=%d\tinedgecount=%d\n",node->stateId,node->inedgecount);
    return inedgecount;

}

int GraphStack::AdjustOutedgeCount(GraphStackNode *node) {
    int outedgecount = 0;
    EdgeNode * edge =  node->outfirstedge;
    while (edge) {
        outedgecount++;
        edge=edge->next;
    }
    node->outedgecount = outedgecount;
    //fprintf(stdout,"stateid=%d\toutedgecount=%d\n",node->stateId,node->outedgecount);
    return outedgecount;
}
int GraphStack::GetActiveSetCount() {
    ActiveNode *p = ahead;
    int count = 0;
    while (p != NULL) {
        if (p->visited==0) {
            count++;
        }
        p=p->next;
    }
    return count;
}
int GraphStack::GetContentId(GraphStackNode *node, const char *realsymbol, const char *grammarsymbol) {
    int contentid = -1;
    EdgeNode *edge =node->infirstedge;
    while (edge) {
        int contentidtemp = edge->contentid;
        if (strcmp(contentlist[contentidtemp].realsymbol,realsymbol)==0 && 
            strcmp(contentlist[contentidtemp].grammarsymbol,grammarsymbol)==0) {
            contentid=contentidtemp;
            break;
        }
        edge=edge->next;
    }
    return contentid;
}
int GraphStack::GLRParse(SimpleLR* slr, int* input ,char** eleminput, char**geleminput,int n, char* path) {
    int ip = 0;
    while (ip <= n) {
//        fprintf(stderr,"eleminput=%s,geleminput=%s\n",eleminput[ip],geleminput[ip]);
        int gid = -1;
        int activecount = GetActiveSetCount();
        ActiveNode* pawork = ahead;
        ActiveNode* paold = NULL;
        string elem;
        string gelem;
        int acount = 0;
        if (ip < n) {
            gid=input[ip];
            elem=eleminput[ip];
            gelem=geleminput[ip];
        }
        else {
            gelem="$";
            elem="$";
            gid = slr->m_terms_count;
        }
        if (ahead==NULL) {
            clear();
            return 1;
        }
        if (gid == -1 || gid >= (slr->m_terms_count+1)) {
            clear();
         return 1;
        }
        DeleteFromActive();
        pawork = ahead;
        paold = NULL;
        activecount = GetActiveSetCount();
        if (activecount==0) {
            clear();
            return 1;
        }
        acount = 0;
        //for reduce
        while (pawork) {
             if(pawork->visited==1) {
                 paold = pawork;
                 pawork=pawork->next;
                 continue;
             }
            int state_id = (pawork->gnode)->stateId;
            char actionstr[ACTION_TABLE_LEN] = {0};
            strcpy(actionstr,slr->ActionTable[state_id][gid]);
            vector<string> actionlist;
            if (0!=strcmp(actionstr,"")) {
                SplitString(actionstr,actionlist,",");
                for (vector<string>::iterator it = actionlist.begin(); it != actionlist.end(); it++) {
                    if ((*it).at(0) == 'r') {
                        //fprintf(stdout," for reduce state_id=%d,gid=%d,gelem=%s, actionstr=%s, str[%d]=%d\n",state_id,gid,gelem.c_str(),actionstr,ip,input[ip]);
                      string ruleidstr = (*it).substr(1);
                      int rule_id = atoi(ruleidstr.c_str());
                      string ruletext = (slr->m_id2rules)[rule_id];
                      vector<string> rules_vec;
                      SplitString(ruletext,rules_vec,":,");
                      int popnum =  rules_vec.size() - 1;
                      int headid = (slr->m_nonterminals[rules_vec[0]]);
                      vector<PathNode*> node_vec;
                      node_vec = GetPathByLength(pawork->gnode,popnum);
                      for (vector<PathNode* >::iterator subit = node_vec.begin(); subit != node_vec.end(); subit++) {
                        EdgeNode *edge = ((*subit)->gnode)->infirstedge;
                        bool createnewContent = false;
                        while (edge) {
                            //pnode is the father of the path's begin node;
                            GraphStackNode* pnode = edge->srcvertex;
                            //edge2is the last outedge of the father;
                            EdgeNode *edge2 = pnode->outfirstedge;
                            while (edge2->next) {
                                edge2 = edge2->next;
                            }
                            GraphStackNode* gnewnode = new GraphStackNode;
                            gnewnode->stateId = (slr->GoToTable)[pnode->stateId][headid];
                            //fprintf(stdout," for reduce node->state_id=%d,headid=%d,headstr=%s,rule=%s,newsateid=%d,str[%d]=%d\n",pnode->stateId,headid,rules_vec[0].c_str(),ruletext.c_str(),gnewnode->stateId,ip,input[ip]);
                            int contentid = 0;
                            if (!createnewContent) {
                                contentid = contentoffset;
                                contentoffset++;
                                strcpy(contentlist[contentid].grammarsymbol, rules_vec[0].c_str());
                                string temp =GetContentString(pnode,(*subit));
                                strcpy(contentlist[contentid].paths,temp.c_str()); 
                                createnewContent = true;
                            }
                            else {
                                contentid=contentoffset-1;
                            }
                            // if there's no such a state in the active set,do 
                            ActiveNode *aptemp = InActiveSet(gnewnode->stateId);
                            if (aptemp==NULL) {
                               AddInEdge(gnewnode,pnode,gnewnode,contentid);
                               AddOutEdge(pnode,pnode,gnewnode,contentid);
                               AddToActive(gnewnode);
                            }
                            else {
                                    EdgeNode *edgetemp = pnode->outfirstedge;
                                    bool isfind = false;
                                    while (edgetemp) {
                                        if ((edgetemp->destvertex)==(aptemp->gnode)) {
                                            isfind = true;
                                            strcat(contentlist[(edgetemp->contentid)].paths,":");
                                            strcat(contentlist[(edgetemp->contentid)].paths,contentlist[contentid].paths);
                                            break;
                                        }
                                        edgetemp=edgetemp->next;
                                    }
                                    /*
                                    edgetemp = (aptemp->gnode)->infirstedge;
                                    while(edgetemp) {
                                        if ((edgetemp->destvertex)->stateId==gnewnode->stateId) {
                                            isfind = true;
                                            break;
                                        }
                                        edgetemp = edgetemp->next;
                                    }
                                    */
                                    if (isfind) {
                                        delete gnewnode;
                                        if (createnewContent) {
                                            memset(contentlist[contentid].paths,0,LINE_LEN_3);
                                            contentoffset--;
                                        }
                                    }
                                    else {
                                        AddInEdge((aptemp->gnode),pnode,(aptemp->gnode),contentid);
                                        AddOutEdge(pnode,pnode,(aptemp->gnode),contentid);
                                        delete gnewnode;
                                    }
                            }
                            edge = edge ->next;
                       // end of traverse edge;
                        }
                     // end of traverse path
                      }
                      (pawork->actioncount)++;
                      //delete some edge from the path
                      if (pawork->actioncount==int(actionlist.size())) {
                        for (vector<PathNode*>::iterator it = node_vec.begin();it!=node_vec.end();
                        it++) {
                            CalcInAndOutWeight((*it)); 
                            int flag=AdjustPathNodeEdges((*it));
                            if (flag==1) {
                                pawork->visited=1;
                                pawork->gnode=NULL;
                            }
                        }
                      }
                      // free pathnode;
                      for (vector<PathNode*>::iterator it = node_vec.begin();it!=node_vec.end();it++) {
                          ClearPath((*it));
                      }
                    //end of reduce;
                    }
                // end of traverse actionlist;
                }
            // end of check actionstr
            }
            else {
                //clear();
                //return 1;
                //continue;
                pawork->visited=1;
                AdjustFailureNodeEdges(pawork->gnode);
                if (pawork->gnode!=head) {
                    pawork->gnode = NULL;
                }
            }
            paold = pawork;
            pawork = pawork->next;
            acount++;
        //end of traverse actives;
        }
        // for shift
        DeleteFromActive();
        pawork = ahead;
        paold = NULL;
        activecount = GetActiveSetCount();
        acount = 0;
        while (pawork&&acount<activecount) {
             if(pawork->visited==1) {
                 paold = pawork;
                 pawork=pawork->next;
                 continue;
             }
            int state_id = (pawork->gnode)->stateId;
            char actionstr[ACTION_TABLE_LEN] = {0};
            strcpy(actionstr,slr->ActionTable[state_id][gid]);
            //fprintf(stdout,"state_id=%d,gid=%d\n",state_id,gid);
            vector<string> actionlist;
            if (0!=strcmp(actionstr,"")) {
                SplitString(actionstr,actionlist,",");
                for (vector<string>::iterator it = actionlist.begin(); it != actionlist.end(); it++) {
                    if ((*it).at(0) == 's') {
                     //fprintf(stdout,"for shift state_id=%d,gid=%d,gelem=%s,actionstr=%s,str[%d]=%d\n",state_id,gid,gelem.c_str(),actionstr,ip,input[ip]);
                      string desstateidstr = (*it).substr(1);
                      int desstate_id = atoi(desstateidstr.c_str());
                      GraphStackNode *gnewnode = new GraphStackNode;
                      gnewnode->stateId = desstate_id;
                      ActiveNode *atemp = InActiveSet(desstate_id);
                      //not in activeset;
                      if (atemp==NULL) {
                         int contentid = contentoffset;
                         contentoffset++;
                         strcpy(contentlist[contentid].realsymbol,elem.c_str());
                         strcpy(contentlist[contentid].grammarsymbol,gelem.c_str());                
                         AddInEdge(gnewnode,(pawork->gnode),gnewnode,contentid);
                         AddOutEdge(pawork->gnode,pawork->gnode,gnewnode,contentid);
                         AddToActive(gnewnode);
                      }
                      else {
                         GraphStackNode* pnode = pawork->gnode;
                         EdgeNode *edge = pnode->outfirstedge;
                         bool isfind = false;
                         while (edge) {
                           if ((edge->destvertex)==atemp->gnode) {
                               isfind = true;
                               if (strcmp(contentlist[edge->contentid].realsymbol,elem.c_str())!=0 || strcmp(contentlist[edge->contentid].grammarsymbol,gelem.c_str())!=0) {
                                   strcat(contentlist[(edge->contentid)].realsymbol, ":");
                                   strcat(contentlist[(edge->contentid)].realsymbol, elem.c_str());
                                   strcat(contentlist[(edge->contentid)].grammarsymbol,":");
                                   strcat(contentlist[(edge->contentid)].grammarsymbol,gelem.c_str());
                               }
                               break;
                           }
                           edge=edge->next;
                         }
                         if (isfind) {
                             delete gnewnode;
                         }
                         else {
                             int contentid=GetContentId(atemp->gnode,elem.c_str(),gelem.c_str());
                             if (contentid==-1) {
                                contentid = contentoffset;
                                contentoffset++;
                                strcpy(contentlist[contentid].realsymbol,elem.c_str());
                                strcpy(contentlist[contentid].grammarsymbol,gelem.c_str());                
                             }
                            AddInEdge(atemp->gnode,(pawork->gnode),atemp->gnode,contentid);
                            AddOutEdge(pawork->gnode,pawork->gnode,atemp->gnode,contentid);
                            delete gnewnode;
                         }
                     // in Active Set 
                      }
                    // end of shift
                        (pawork->actioncount)++;
                        if (pawork->actioncount==int(actionlist.size())) {
                            pawork->visited=1;
                        }
                    }
                // end of traverse actionlist
                }
            // end of check actionstr
            }
            else {
                 //continue;
                //clear();
                //return 1;
                pawork->visited=1;
                AdjustFailureNodeEdges(pawork->gnode);
                if (pawork->gnode!=head) {
                    pawork->gnode = NULL;
                }
            }
            paold = pawork;
            pawork = pawork->next;
            acount++;
        // end of shift
        }
        // read end of str, to check if all the heads in actives is ready to act "acc"; 
        if (ip == n) {
            int flagerr = 1;
            pawork = ahead;
            paold = NULL;
            activecount = GetActiveSetCount();
            acount = 0;
            while (pawork && acount<activecount) {
                if (pawork->visited==1) {
                    paold = pawork;
                    pawork = pawork->next;
                    continue;
                }
                int state_id = (pawork->gnode)->stateId;
                if (state_id < (slr->m_states_count) && gid <(slr->m_terms_count+1)) {
                    char actionstr[ACTION_TABLE_LEN] = {0};
                    strcpy(actionstr,slr->ActionTable[state_id][gid]);
                    //fprintf(stdout,"state_id=%d,gid=%d\n",state_id,gid);
                    vector<string> actionlist;
                    if (0!=strcmp(actionstr,"")) {
                        SplitString(actionstr,actionlist,",");
                        for (vector<string>::iterator it = actionlist.begin(); it != actionlist.end(); it++) {
                            if (*it=="acc") {
                                flagerr = 0;
                                break;
                            }
                        }
                    }
                }   
                else {
                    flagerr+=1;
                }
                paold = pawork;
                acount++;
                pawork = pawork->next;
            // end of traverse actives;
            }
            if (0 == flagerr) {
                for (int i=0; i<contentoffset;i++) {
 //                   fprintf(stdout,"%d:%s\t%s\t%s\n",i,contentlist[i].realsymbol,contentlist[i].grammarsymbol,contentlist[i].paths);
                }
    
                clear();
                return 0;
              
            }
            else {
                clear();
                return 1;
            }
        // end of ip==n
        }
       /* 
        for (int i=0; i<contentoffset;i++) {
            fprintf(stdout,"%d:%s\t%s\t%s\n",i,contentlist[i].realsymbol,contentlist[i].grammarsymbol,contentlist[i].paths);
        }
        */
       ip++;
    //end of ip<=n
    }
    return 0;
}
