/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：GraphStrack.h
*   创 建 者：unicodeproject
*   创建日期：2016年08月09日
*   描    述：
*   备    注: the Graph Structure Stack is a DAG, and you donnot know the edge 
*   更新日志：
*
================================================================*/
#include "common.h"
#include "SimpleLR.h"
#include <queue>
#include <stack>
#ifndef _GRAPHSTRACK_H
#define _GRAPHSTRACK_H
// please add your code here!
typedef struct ContentNode {
    char realsymbol[LINE_LEN_5];//example in
    char grammarsymbol[LINE_LEN_5];//example prep
    char paths[LINE_LEN_3];//semicolon for group segmentation, comma for inner group segmentation; 
    public:
    ContentNode() {
        memset(realsymbol,0,LINE_LEN_5);
        memset(grammarsymbol,0,LINE_LEN_5);
        memset(paths,0,LINE_LEN_3);
    }
}ContentNode;
struct GraphStackNode; 
typedef struct EdgeNode {
    EdgeNode *next;
    GraphStackNode* srcvertex;//边的入点
    GraphStackNode* destvertex;//边的出点
    int contentid;
    public:
    EdgeNode () {
        next = NULL;
        srcvertex = NULL;
        destvertex = NULL;
        contentid = -1;
    }
} EdgeNode;
typedef struct PathNode {
    GraphStackNode *gnode;
    PathNode *next;
    PathNode *prev;
    int inweight;//计算路径的入度和
    int outweight;//计算路径的出度和
    int toprocess;
    public:
    PathNode() {
        gnode = NULL;
        next = NULL;
        prev = NULL;
        inweight = 0;
        outweight = 0;
        toprocess = 0;
    }
} PathNode;
typedef struct ActiveNode {
    GraphStackNode *gnode;
    ActiveNode *next;
    int actioncount;
    int visited;
    public:
    ActiveNode() {
        gnode = NULL;
        next = NULL;
        actioncount = 0;
        visited = 0;
    }
    
} ActiveNode;
typedef struct GraphStackNode {
    int stateId; // stateId;
    int npath;// for calc path
    int id;//assisted for deep first traverse path
    int inedgecount;
    int outedgecount;
    EdgeNode* infirstedge;
    EdgeNode* outfirstedge;
    GraphStackNode* next;
    //ContentNode* contentlist;
    int* visited; //assist used for find paths
    GraphStackNode* parent;// used for deep first traverse to calc path len 
    public:
    GraphStackNode () {
        stateId = -1;
        npath = 0;
        id = 0;
        inedgecount = 0;
        outedgecount = 0;
        infirstedge = NULL;
        outfirstedge = NULL;
        next = NULL;
        visited = NULL;
        parent = NULL;

    }
} GraphStackNode;
class GraphStack {
    public:
        ActiveNode *ahead;
        ActiveNode *atail;
        GraphStack();
        ~GraphStack();
        void clear();
        void push(GraphStackNode* head,GraphStackNode* newnode);
        void pop();
        int size();
        int contentoffset;
        ContentNode* contentlist;
        int GLRParse(SimpleLR* slr, int* input,char** eleminput, char**geleminput, int n ,char* path);
        vector<PathNode*> GetPathByLength(GraphStackNode *node, int nlen);
        string GetContentString(GraphStackNode *node, PathNode* path);
        ActiveNode* InActiveSet(int stateid);
        int AdjustInedgeCount(GraphStackNode* node);
        int AdjustOutedgeCount(GraphStackNode* node);
        void DeleteInEdge(GraphStackNode* node,GraphStackNode* srcnode, GraphStackNode* destnode, int contentid);
        void DeleteOutEdge(GraphStackNode* node, GraphStackNode *srcnode, GraphStackNode* destnode, int contentid);
        void DeleteInEdge(GraphStackNode* node,GraphStackNode* srcnode, GraphStackNode* destnode);
        void DeleteOutEdge(GraphStackNode* node, GraphStackNode *srcnode, GraphStackNode* destnode);
        void AddInEdge(GraphStackNode* node, GraphStackNode* srcnode,GraphStackNode* destnode, int contentid);
        void AddOutEdge(GraphStackNode* node,GraphStackNode* srcnode,GraphStackNode*destnode,int contentid);
        void AddToActive(GraphStackNode* node);
        void DeleteFromActive();
        void ClearPath(PathNode *path);
        void DeleteAllInEdge(GraphStackNode* node);
        void DeleteAllOutEdge(GraphStackNode* node);
        void CalcInAndOutWeight(PathNode* path);
        int AdjustPathNodeEdges(PathNode* path);
        void AdjustFailureNodeEdges(GraphStackNode *pnode);
        int GetActiveSetCount();
        int GetContentId(GraphStackNode *node, const char *realsymbol, const char* grammarsymbol);
    private:
        GraphStackNode* head;
};
#endif
