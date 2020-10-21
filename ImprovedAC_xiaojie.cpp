//============================================================================
// Name        : improvedAC.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include<string.h>
#include <set>
#include <time.h>
#define MAX_CHILD_LEN 128
#define MAX_OUTPUT    128
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <map>
using namespace std;
#define MAX_FILE_CONTENT  1024*1024*20
class Node
{
public:
    Node(char ele){
        element = ele;
        parent = NULL;
        failNode = NULL;
        isMatchNode=false;
        memset(nodeList,0,MAX_CHILD_LEN);
        childNum=0;
        high = 0;
        memset(outPut,0,MAX_OUTPUT);
        outPutNum = 0;
    }
    ~Node(){
        printf("node release,high:%d value:%c\n",high,element);
        for(int i=0;i<outPutNum;i++){
            char * ele = outPut[i];
            printf("release output str:%s\n",ele);
            delete ele;
            ele = NULL;
        }
    }
    char element;
    Node * parent;
    Node * nodeList[MAX_CHILD_LEN];
    Node * failNode;
    bool isMatchNode;
    int childNum;
    int high;
    char * outPut[MAX_OUTPUT];
    int outPutNum;
};
typedef void(*Func)(char * matchedStr,int post);
class AC
{
public:
    AC() {root = new Node(NULL);root->failNode = root;}
    ~AC(){
        delete root;
        root = NULL;
    }
    void initTree(char * patterns[],int patLen){
        for(int i=0;i<patLen;i++){
            char * element = patterns[i];
            int eleLen = strlen(element);
            for(int j=0;j<eleLen;j++){
                bool isMatch = false;
                if(j == eleLen-1)
                    isMatch = true;
                char * p = new char[j+2];
                memset(p,'\0',j+2);//memset(void *s, int ch, size_t n) 将s中当前位置后面的n个字节用 ch 替换并返回 s
                strncpy(p,element,j+1);//strncpy(char *dest, const char *src, int n)，表示把src所指向的字符串中以src地址开始的前n个字节复制到dest
                insert(p,isMatch);
            }
        }
    }
    void buildFailNode(){
        traceAllNodes(root);
    }
    void printSet(set<Node *> xx){
		//调试

		set<Node *>::iterator iterxx;
		int index=0;
		printf("输出集合元素{\n");
		for(iterxx = xx.begin();iterxx!=xx.end();++iterxx){
			Node * node = *iterxx;
			printf("第%d个节点：",index);
			printf("%d,%d,%c\n",node->isMatchNode,node->childNum,node->element);
			index++;
		}
		//调试
		printf("}输出集合元素\n");

    }
    void match_improved_ac(char * srcTxt,int txtLen,Func f){//改进的AC算法，用于匹配非连续子序列
        Node * startNode = root;
        //定义一个存储节点和要匹配边的集合。并初始化。
		set<Node *> A;
		set<Node *> B;//要匹配的指向节点，然后节点的element就是要匹配的元素

		set<Node *>::iterator iter;
		A.insert(startNode);




		for (int j=0;j<startNode->childNum;j++){
			Node * node = startNode->nodeList[j];
			B.insert(node);
		}
		for(int i=0;i<txtLen;i++){
			char e = srcTxt[i];
			if(B.empty()){
				break;
			}
			else {
				set<Node *> addedB;
				addedB.clear();
				set<Node *> deletedB;
				deletedB.clear();
				for(iter = B.begin();iter!=B.end();)
				{
					Node * nodeInB= *iter;//取出待匹配边。然后比较
					if(nodeInB->element == e){ //匹配上，更新A和B. //注意element是指向nodeInB的
						//如果当前节点是模式成功匹配节点
						if(nodeInB->isMatchNode){
							for(int k=0;k<nodeInB->outPutNum;k++){
								f(nodeInB->outPut[k],i); //i为匹配的位置。
							}
						}

						//更新A
						A.insert(nodeInB);
						//准备更新B
						for(int j=0;j<nodeInB->childNum;j++){
						    Node * nodeNew = nodeInB->nodeList[j];
						    addedB.insert(nodeNew);
						}
						//同时准备删除B中匹配成功的元素。
						std::set<Node *>::iterator itmp = iter;
						//更新迭代器
						++iter;//先嗯行迭代器，再执行删除操作.
						B.erase(itmp);//再删除之前的迭代器指向元素。


					}
					else{
						++iter;
					}

				}

				//将addedB添加到B中。
				for(iter = addedB.begin();iter!=addedB.end();iter++){
					B.insert(*iter);
				}
			}
		}
		if(B.empty()){//说明匹配到了所有模式
			printf("匹配到了所有模式\n");
		}
		//无论是否匹配到所有模式

		for(iter = A.begin();iter!=A.end();++iter){
			Node * nodeInA= *iter;//取出待匹配边。然后比较
            if(nodeInA->isMatchNode){
                for(int k=0;k<nodeInA->outPutNum;k++){
                	printf("匹配到的模式串:%s\n",nodeInA->outPut[k]);
                }
            }
		}

    }
    void match(char * srcTxt,int txtLen,Func f){
        Node * startNode = root;
        for(int i=0;i<txtLen;i++){
            char e = srcTxt[i];
            bool isOk = false;
            for(int j=0;j<startNode->childNum;j++){
                Node * node = startNode->nodeList[j];
                if(node->element == e){
                    isOk = true;
                    startNode = node;
                    Node * failNode = node->failNode;
                    while(failNode!=root){
                        if(failNode->isMatchNode){
                            for(int k=0;k<failNode->outPutNum;k++)
                                f(failNode->outPut[k],i);
                        }
                        failNode = failNode->failNode;
                    }
                    if(node->isMatchNode){
                        for(int k=0;k<node->outPutNum;k++)
                            f(node->outPut[k],i);
                    }
                    break;
                }
            }
            if(!isOk){
                startNode = startNode->failNode;
                if(startNode!=root)
                    i--;
            }
        }
    }
    void deleteTree(){
        printf("delete tree--------------------------------\n");
        traceDelNodes(root);
    }
    void printACTree(){
        printf("tree structure-----------------------------\n");
        printf("high  value  match   failNode  childNum  children  outPutStr   \n");
        tracePrintNodes(root);
    }
private:
    void insert(char * ele,bool isMatch){
        int eleLen = strlen(ele);
        //搜索ele最后一个元素节点是否存在，不存在则返回父节点
        int startH = 1;
        Node * pnode = NULL;
        if(!search(root,startH,ele,pnode)){
            Node * cnode = new Node(ele[eleLen-1]);
            cnode->high = eleLen;
            cnode->parent = pnode;
            cnode->isMatchNode = isMatch;
            pnode->nodeList[pnode->childNum]=cnode;
            pnode->childNum++;
            if(isMatch)
                cnode->outPut[cnode->outPutNum++]=ele;
        }
    }
    bool search(Node * pnode,int & index,char * ele,Node * &retNode){
        for(int i=0;i<pnode->childNum;i++){
            Node * node = pnode->nodeList[i];
            if(node->element == ele[index-1]){
                if(index == strlen(ele))
                    return true;
                index++;
                return search(node,index,ele,retNode);
            }
        }
        retNode = pnode;
        return false;
    }
    void initFailNode(Node * node){
        if(node->high == 1){
            node->failNode = root;
        }else{
            //以父节点的失败函数作为起点，node的element作为触发边得到node的失败函数
            Node * failNode = NULL;
            searchFailNode(node->parent,node,failNode);
            node->failNode = failNode;
        }
    }
    void traceAllNodes(Node * node){
        for(int i=0;i<node->childNum;i++){
            Node * cnode = node->nodeList[i];
            initFailNode(cnode);
            traceAllNodes(cnode);
        }
    }
    void searchFailNode(Node * pnode,Node * cnode,Node * & retNode){
        Node * failNode = pnode->failNode;
        for(int i=0;i<failNode->childNum;i++){
            Node * node = failNode->nodeList[i];
            if(node->element == cnode->element){
                retNode = node;
                return;
            }
        }
        //循环已经走完，说明没找到节点，如果已经搜索了根节点没找到，则返回根节点作为失败节点，否则继续搜索
        if(pnode->failNode == root)
            retNode = root;
        else
            searchFailNode(pnode->parent,pnode,retNode);
    }
    void tracePrintNodes(Node *node){
        for(int i=0;i<node->childNum;i++){
            Node * cnode = node->nodeList[i];
            printf("  %d     %c     %d       %d           %d        ",cnode->high,cnode->element,cnode->isMatchNode,cnode->failNode->high,cnode->childNum);
            for(int j=0;j<cnode->childNum;j++){
                Node * lnode = cnode->nodeList[j];
                printf("%c ",lnode->element);
            }
            printf("         ");
            for(int j=0;j<cnode->outPutNum;j++)
                printf("%s ",cnode->outPut[j]);
            printf("\n");
            tracePrintNodes(cnode);
        }
    }
    void traceDelNodes(Node * node){
        for(int i=0;i<node->childNum;i++){
            Node * cnode = node->nodeList[i];
            traceDelNodes(cnode);
        }
        if(node!=root){
            delete node;
            node = NULL;
        }
    }
private:
    Node * root;
};

//回调函数在外面进行统计，找到一个回调一次
void findCallBack(char * matchedStr,int startPos){
    char curTime[32]={0};
//    currentTime(curTime,sizeof(curTime));
    printf("end time:%s  machedStr:%s  matchedPos:%d\n",curTime,matchedStr,startPos);
}
bool readFile(char * fileName,char * fileContent){
    int fd = open(fileName,O_RDONLY);
    if(-1 == fd){
        printf("open file error:%d\n",errno);
        return false;
    }
    int len = read(fd,fileContent,MAX_FILE_CONTENT);
    if(-1 == len){
        printf("read file error:%d\n",errno);
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
void ac_func(char ** pattern,int patLen,char *txt,int txtLen){
    AC ac;
    ac.initTree(pattern,patLen);
    ac.buildFailNode();
    ac.printACTree();
    char curTime[32]={0};
//    currentTime(curTime,sizeof(curTime));
    printf("searching tree-----------------------------\n");
    printf("start time of search:%s\n",curTime);
    ac.match(txt,txtLen,findCallBack);
    ac.deleteTree();
}
void test1(){
    char *pattern[]={"hee","he", "h","she", "his" ,"her","hers"};
    char *txt = "ushers";
    int patternLen = sizeof(pattern)/sizeof(char*);
    int txtLen = strlen(txt);
    ac_func(pattern,patternLen,txt,txtLen);
}
void test2(){
    //模式串出现的顺序在文件中刚好相反，里面存在一个关键字在日志文件的最后一行
    char * pattern[] = {"39_347990193541512029","39_347739859976612007","13_1002D375","2017-09-29 17:43:37:517"};
    //文件内容太大，使用堆内存
    char * txt = new char[MAX_FILE_CONTENT];
//    bzero(txt,MAX_FILE_CONTENT);
    //test.log为一个10M的日志
    if(!readFile("./test.txt",txt))
        return;
    int patternLen = sizeof(pattern)/sizeof(char*);
    int txtLen = strlen(txt);
    ac_func(pattern,patternLen,txt,txtLen);
    delete txt;
    txt = NULL;
}

void test3(){
    //模式串和内容出现自包含,notepad遇到这样的搜索有bug
    char *pattern[]={"s", "ss","sss","ssss"};
    char *txt = "ssss";
    int patternLen = sizeof(pattern)/sizeof(char*);
    int txtLen = strlen(txt);
    ac_func(pattern,patternLen,txt,txtLen);
}

void ac_func_Discontinuous_subsequence(char ** pattern,int patLen,char *txt,int txtLen){
    AC ac;
    ac.initTree(pattern,patLen);
    ac.buildFailNode();
    ac.printACTree();
    char curTime[32]={0};
//    currentTime(curTime,sizeof(curTime));
    printf("searching tree-----------------------------\n");
    printf("start time of search:%s\n",curTime);
    ac.match_improved_ac(txt,txtLen,findCallBack);
    //ac.match(txt,txtLen,findCallBack);
    ac.deleteTree();
}

void improvedACFor_Discontinuous_subsequence(){//非连续子序列
    char *pattern[]={"it","hit", "ice","itch"};
    char *txt = "Uhiteeceh";
    int patternLen = sizeof(pattern)/sizeof(char*);
    int txtLen = strlen(txt);
    ac_func_Discontinuous_subsequence(pattern,patternLen,txt,txtLen);//非连续子序列
}
void improvedAC_test2(){
    //模式串出现的顺序在文件中刚好相反，里面存在一个关键字在日志文件的最后一行
    char * pattern[] = {"39_347990193541512029","39_347739859976612007","13_1002D375","2017-09-29 17:43:37:517"};
    //文件内容太大，使用堆内存
    char * txt = new char[MAX_FILE_CONTENT];
//    bzero(txt,MAX_FILE_CONTENT);
    //test.log为一个10M的日志
    if(!readFile("./test.txt",txt))
        return;
    int patternLen = sizeof(pattern)/sizeof(char*);
    int txtLen = strlen(txt);
    ac_func(pattern,patternLen,txt,txtLen);
    delete txt;
    txt = NULL;
}
int main()
{
	setbuf(stdout,NULL);//方便调试
//    test1();
    //test2();
	improvedACFor_Discontinuous_subsequence();
    //improvedAC_test2();
//    test3();
    return 0;
}
