/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*   
*   文件名称：test.cpp
*   创 建 者：unicodeproject
*   创建日期：2016年07月20日
*   描    述：
*   备    注: 
*   更新日志：
*
================================================================*/
 
// please add your code here!
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<time.h>
#include <set>
#include <string>
#include<sys/time.h>
#include "Grammar.h"
#include "SimpleLR.h"
#include "GraphStack.h"
#include "util.h"
#include <queue>
using namespace std;

void PrintUsage()
{
        fprintf( stderr, "prog [IN]hzpylist_file [IN]input_file [OUT]output_file [OUT]errdmp_file\n" );
}
int test(SimpleLR *lr,const char *infilename,const char* ofilename) {
    FILE *fin = fopen(infilename,"r");
    FILE *fout = fopen(ofilename,"w");
    char line[1024] = {0};
    char path[1024] = {0};
    if (NULL == fin) {
        fprintf(stderr,"can not open inputfile %s\n",infilename);
        if (NULL != fout) {
            fclose(fout);
            fout = NULL;
        }
        return 1;
    }
   if (NULL == fout) {
       fprintf(stderr,"can not create outputfile %s\n",ofilename);
       if (NULL != fin) {
           fclose(fin);
           fin = NULL;
       }
       return 1;
   }
   int linecount = 0;
   while (fgets(line,1024,fin)) {
       size_t len = strlen(line);
       line[len-1] = 0;
       len = strlen(line);
       int utf8size = enc_calc_unicode_outsize((uchar*)line);
       char **s = new char *[utf8size];
       int i = 0; 
       int step = 0;
       int count = 0;
       int flag = 0;
       if (linecount%10==0) {
           fprintf(stderr,"linecount=%d\n",linecount);
       }
       linecount++;
       while(i<int(len)) {
           step = enc_get_utf8_size((uchar)(*(line+i)));
           if (0 == step){
               step=1;
           }
           s[count] = new char[step+1];
           strncpy(s[count],line+i,step);
           s[count][step]=0;
           count++;
           i+=step;
       }
       for (i=0;i<utf8size;i++) {
           if (!(lr->m_morphmap).count(s[i])) {
                flag = 1;
                break;
           }
       }
       if (flag == 1) {
           fprintf(fout,"%s\t0\n",line);
           fflush(fout);
           for (i=0;i<utf8size;i++) {
               if (s[i]!=NULL) {
                   delete [] s[i];
                   s[i]=NULL;
               }
           }
           delete [] s;
           continue;
       }
       char **geleminput = new char *[utf8size];
       for (int j = 0; j < utf8size;j++) {
           geleminput[j] = new char [LINE_LEN_5];
           memset(geleminput[j],0,LINE_LEN_5); 
       }
       int *ids = new int[utf8size];
       // to generate all the pathes
       vector<string> tempvec;
       vector<string> vec_path;
       int needtopop = 0;
       int needtopopold = 0;
       i = 0;
       count = 0;
       queue<string> myque;
       tempvec = (lr->m_morphmap)[s[i]];
       needtopop = tempvec.size();
       for (vector<string>::iterator it = tempvec.begin(); it != tempvec.end();it++) {
            myque.push(*it);
       }
       i=1;
       while (i < utf8size) {
           count = 0;
           needtopopold=needtopop;
           tempvec.clear();
           tempvec = (lr->m_morphmap)[s[i]];
           while (needtopop) {
               string s1 = myque.front();
               for (vector<string>::iterator it = tempvec.begin(); it != tempvec.end();it++) {
                   string tempstr=s1+";";
                   tempstr+=*it;
                   myque.push(tempstr);
                   count++;
               }
               myque.pop();
               needtopop--;
           }
           needtopop=count;
           i++;  
       }
       while (!myque.empty()) {
           string temp = myque.front();
           vec_path.push_back(temp);
           myque.pop();
       }
       flag = 0;
       fprintf(stderr,"%d\n",vec_path.size());
        //int  limit = vec_path.size()<6000?vec_path.size():6000;
        //fprintf(stderr,"limit=%d\n",limit);
//       for (vector<string>::iterator it = vec_path.begin(); it != vec_path.end();it++) {
    int limit = vec_path.size();
    for (int i=0; i<limit;i++) {
         tempvec.clear();
         SplitString(vec_path[i],tempvec,";");
         if (int(tempvec.size()) != utf8size) {
             continue;
         }
         memset(ids,0, sizeof(int)*utf8size);
         for (int i = 0; i < utf8size;i++) {
            strcpy(geleminput[i],tempvec[i].c_str());
            ids[i]=(lr->m_terminals)[tempvec[i]];
         }
         GraphStack *gs = new GraphStack();
         int code = gs->GLRParse(lr,ids,s,geleminput,utf8size,path);
         if (code==0) {
            flag = 1;
         }
         delete gs;
         
    }
     fprintf(fout,"%s\t%d\n",line,flag);
     fflush(fout);

        
       for (i=0;i<utf8size;i++) {
         if (s[i]!=NULL) {
             delete [] s[i];
             s[i]=NULL;
         }
       }
       delete [] s;
       for (i=0;i<utf8size;i++) {
         if (geleminput[i] != NULL) {
             delete [] (geleminput[i]);
             geleminput[i] = NULL;
         }
       }
       delete [] geleminput;
       geleminput = NULL;
       if (ids!=NULL) {
           delete [] ids;
       }
       ids = NULL;
// end of while
   }
   if (NULL!=fin) {
       fclose(fin);
       fin = NULL;
   }
   if (NULL != fout) {
       fclose(fout);
       fout = NULL;
   }
    return 0;
}
int main( int argc, char *argv[] ) 
{
    timeval tv1, tv2;
    if ( 4 != argc )
	{
		PrintUsage();
		return 1;
	}
    // test for SimpleLR()
    FILE *fout = fopen(argv[3],"w");
    SimpleLR *lr = new SimpleLR("Stcc");
    fprintf(fout,"项目集开始\n");    
    lr->GenerateItemSets();
    int id = 0;
    for (vector<std::tr1::unordered_set<string> >::iterator it = (lr->m_closures).begin(); it != (lr->m_closures).end(); it++) {
        fprintf(fout,"state_id=%d\n",id);
        for (std::tr1::unordered_set<string>::iterator subit =(*it).begin(); subit != (*it).end(); subit++) {
        fprintf(fout,"%s\n",(*subit).c_str());
        }
        fprintf(fout,"statd_id=%d,finish\n\n",id);
        id++;
    }
    fprintf(fout,"项目集结束开始\n");    
    
   
    fprintf(fout,"first集合开始\n");    
    lr->GetFirstSet();
    for (std::tr1::unordered_map<string,string>::iterator it = (lr->m_firstset).begin(); it != (lr->m_firstset).end();it++) {
        fprintf(fout,"%s\t%s\n",(it->first).c_str(),(it->second).c_str());
    }
    fprintf(fout,"first集合结束\n");    
    
    
    fprintf(fout,"follow集合开始\n");    
    lr->GetFollowSet();
    for (std::tr1::unordered_map<string,string>::iterator it = (lr->m_followset).begin(); it!=(lr->m_followset).end();it++) {
        fprintf(fout,"%s\t%s\n",(it->first).c_str(),(it->second).c_str());
    }
    fprintf(fout,"follow集合结束\n");    
    
    lr->GetCount();
    lr->NextEdge();
    lr->ConstructTables();
    
    
/*
      string elem="PP";
      string firststr="";
      lr->GetSingleFollow(elem,firststr);
      fprintf(fout,"%s\t%s\n",elem.c_str(),firststr.c_str());
  */  
   
    fprintf(fout,"非终结符开始\n");
    for (std::tr1::unordered_map<string,int>::iterator it = lr->m_nonterminals.begin(); it!=lr->m_nonterminals.end();it++) {
        fprintf(fout,"%s=%d\n",(it->first).c_str(),it->second);
    }
    fprintf(fout,"非终结符结束\n");
    fprintf(fout, "GOTO表开始\n");
    for (int i = 0; i < lr->m_states_count;i++) {
        for (int j = 0; j < lr->m_nonterms_count;j++) {
            if (lr->GoToTable[i][j] != -1) {
                fprintf(fout,"GoToTable[%d][%d]=%d",i,j,lr->GoToTable[i][j]);
                if (j < lr->m_nonterms_count - 1) {
                    fprintf(fout," ");
                }
                fprintf(fout,"\n");
            }
        }
    }
    fprintf(fout, "GOTO表结束\n");

    fprintf(fout,"终结符开始\n");
    for (std::tr1::unordered_map<string,int>::iterator it = lr->m_terminals.begin(); it!=lr->m_terminals.end();it++) {
        fprintf(fout,"%s=%d\n",(it->first).c_str(),it->second);
    }
    fprintf(fout,"终结符结束\n");
    fprintf(fout,"规则集开始\n");
    for (std::tr1::unordered_map<string,int>::iterator it = lr->m_rules.begin(); it!=lr->m_rules.end();it++) {
        fprintf(fout,"%s=%d\n",(it->first).c_str(),it->second);
    }
    fprintf(fout,"规则集结束\n");
    fprintf(fout, "ACTION表开始\n");
    for (int i = 0; i < lr->m_states_count;i++) {
        for (int j = 0; j < lr->m_terms_count+1;j++) {
            if (strcmp(lr->ActionTable[i][j],"") != 0) {
                fprintf(fout,"Action[%d][%d]=%s",i,j,lr->ActionTable[i][j]);
                if (j < lr->m_terms_count - 1) {
                    fprintf(fout," ");
                }
                fprintf(fout,"\n");
            }
        }
    }
    fprintf(fout, "ACTION表结束\n");
    fclose(fout);
    gettimeofday(&tv1, NULL); 
    test(lr,argv[1],argv[2]); 
    if (NULL != lr) {
        delete lr;
        lr = NULL;
    }
    gettimeofday(&tv2, NULL);
    fprintf(stderr,"%s has finished congratulations!\n",argv[0]);
    fprintf( stderr,"time elapsed: %.2f ms\n", (float)((tv2.tv_sec - tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec))/1000);
	return 0;
}
