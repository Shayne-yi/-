#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX_STACK 100
#define MAXNAME 50   // 地点名称最大长度
#define MAXINFO 200  // 地点介绍最大长度
#define MAXNAME 50  //地点名称最大长度
#define MAXINFO 200 //地点介绍最大长度
#define MAX_CHILD 10    //每个分类节点最多10个子分类
#define MAX_CLASS_NAME 30 //分类名称字符长度

//分类枚举：区分一级大类
typedef enum {
    TEACH_BUILD,    //教学楼
    LIVE_AREA,      //生活区
    SPORT_AREA,     //运动区
    OTHER_AREA      //其他区域
}AreaType;

//地点信息结构体
typedef struct {
    int id;             // 地点编号
    char name[50];      // 地点名称
    char desc[100];     // 地点描述
    float x, y;         // 地点坐标
    char type[30];      //新增地点分类
} CampusSpot;

//链表节点结构体
typedef struct Node {
    CampusSpot data;    // 节点存储的地点数据
    struct Node *next;  // 指向下一个节点的指针
} ListNode;

// 操作撤销栈元素：记录增删改操作
typedef struct {
    int opType;     // 1-新增 2-删除 3-修改
    CampusSpot oldSite;   // 操作前原始数据
    char opLog[100];     //新增：操作文字记录
} OpStackElem;

// 顺序栈结构
typedef struct {
    OpStackElem data[MAX_STACK];
    int top;
} OpStack;

// 浏览历史栈元素
typedef struct {
    CampusSpot site;
} ViewStackElem;

typedef struct {
    ViewStackElem data[MAX_STACK];
    int top;
} ViewStack;

//链表头结构体
typedef struct {
    ListNode *head;     // 链表头指针
    int length;         // 链表当前元素个数
} SpotLinkList;

//分类树节点：存储分类名称+挂载该分类下所有地点链表
typedef struct ClassTreeNode{
    char className[MAX_CLASS_NAME]; //分类名：教学楼/弘义楼/3楼
    AreaType type;                 //所属大区域类型
    struct ClassTreeNode *child[MAX_CHILD]; //子分类数组
    int childCnt;                  //当前子节点数量
    ListNode *spotListHead;        //挂载：该分类下所有地点
}ClassTreeNode,*ClassTree;

// 全局定义两个栈
OpStack g_opStack;
ViewStack g_viewStack;
SpotLinkList g_SpotList;//全局地点主链表
ClassTree g_ClassRoot; //V4新增：全局分类树根节点

// 初始化链表
void InitList(SpotLinkList *L) {
    L->head = NULL;
    L->length = 0;
}
// 释放
void DestroyList(SpotLinkList *L) {
    ListNode *p, *q;
    p = L->head;
    while (p != NULL) {
        q = p->next;
        free(p);
        p = q;
    }
    L->head = NULL;
    L->length = 0;
}

// 初始化操作栈
void InitOpStack(OpStack *s) {
    s->top = -1;
}

// 操作栈入栈
int PushOpStack(OpStack *s, OpStackElem e) {
    if (s->top == MAX_STACK - 1) return 0;
    s->data[++s->top] = e;
    return 1;
}

// 操作栈出栈
int PopOpStack(OpStack *s, OpStackElem *e) {
    if (s->top == -1) return 0;
    *e = s->data[s->top--];
    return 1;
}

// 判断操作栈是否为空
int IsOpStackEmpty(OpStack *s) {
    return s->top == -1;
}

// 清空操作栈
void ClearOpStack(OpStack *s) {
    s->top = -1;
}

// 初始化浏览栈
void InitViewStack(ViewStack *s) {
    s->top = -1;
}

// 浏览栈入栈
int PushViewStack(ViewStack *s, ViewStackElem e) {
    if (s->top == MAX_STACK - 1) return 0;
    s->data[++s->top] = e;
    return 1;
}

// 清空浏览栈
void ClearViewStack(ViewStack *s) {
    s->top = -1;
}

//初始化空分类树根节点
ClassTree InitClassTree(char rootName[]){
    ClassTree root = (ClassTree)malloc(sizeof(ClassTreeNode));
    strcpy(root->className,rootName);
    root->childCnt=0;
    root->spotListHead=NULL;
    for(int i=0;i<MAX_CHILD;i++) root->child[i]=NULL;
    return root;
}

//向父节点添加子分类
int AddClassNode(ClassTree parent,char subName[],AreaType t){
    if(parent->childCnt >= MAX_CHILD){
        printf("该分类子分类已满！\n");
        return 0;
    }
    ClassTree newNode = InitClassTree(subName);
    newNode->type = t;
    parent->child[parent->childCnt++] = newNode;
    return 1;
}

//递归查找分类节点
static void FindClass(ClassTree p, const char *name, ClassTree *resNode)
{
    if (!p) return;
    if (strcmp(p->className, name) == 0)
    {
        *resNode = p;
        return;
    }
    for (int i = 0; i < p->childCnt; i++)
    {
        FindClass(p->child[i], name, resNode);
    }
}

//前缀搜索DFS
static void DfsSearch(ClassTree p, char *k)
{
    if (!p) return;
    //分类名前缀匹配
    if (strstr(p->className, k) != NULL)
    {
        printf("匹配分类：%s\n", p->className);
        //顺带输出该分类所有地点
        ListNode *tmp = p->spotListHead;
        while (tmp)
        {
            printf("→地点：%s\n", tmp->data.name);
            tmp = tmp->next;
        }
    }
    for (int i = 0; i < p->childCnt; i++)
    {
        DfsSearch(p->child[i], k);
    }
}

//初始化空分类树根节点
ClassTree InitTree(const char rootName[])
{
    ClassTree root = (ClassTree)malloc(sizeof(ClassTreeNode));
    strcpy(root->className, rootName);
    root->childCnt = 0;
    root->spotListHead = NULL;
    for (int i = 0; i < MAX_CHILD; i++)
        root->child[i] = NULL;
    return root;
}

//向父节点添加子分类
ClassTree AddNode(ClassTree parent, const char subName[], AreaType t)
{
    if (parent->childCnt >= MAX_CHILD)
    {
        printf("该分类子分类已满！\n");
        return 0;
    }
    ClassTree newNode = InitTree(subName);
    newNode->type = t;
    parent->child[parent->childCnt++] = newNode;
    return 0;
}

//将一个地点绑定到指定分类
static void FindClass(ClassTree p, const char *name, ClassTree *resNode);
int BindSpotToClass(ClassTree root, const char className[], CampusSpot spot)
{
    ClassTree findNode = NULL;
    FindClass(root, className, &findNode); //调用外部查找函数
    if (!findNode)
    {
        printf("无此分类，绑定失败！\n");
        return 0;
    }
    //复用原有AddSpot逻辑，插入该分类专属链表
    ListNode *newN = (ListNode*)malloc(sizeof(ListNode));
    newN->data = spot;
    newN->next = findNode->spotListHead;
    findNode->spotListHead = newN;
    return 1;
}

//【菜单调用】按分类名称遍历输出该分类所有地点
void QueryByClass(ClassTree root, char className[])
{
    ClassTree findNode = NULL;
    FindClass(root, className, &findNode); //复用外部查找
    if (!findNode || !findNode->spotListHead)
    {
        printf("该分类下暂无地点数据\n");
        return;
    }
    printf("====【%s】分类下所有地点====\n", className);
    ListNode *p = findNode->spotListHead;
    while (p)
    {
        printf("编号:%d 名称:%s 坐标(%.2f,%.2f)\n", p->data.id, p->data.name, p->data.x, p->data.y);
        p = p->next;
    }
}

//前缀快速匹配（
void PrefixSearchTree(ClassTree root, char key[])
{
    DfsSearch(root, key); //调用外部DFS
}

// 新增地点
int AddSpot(SpotLinkList *L, CampusSpot spot) {
    // 1. 创建新节点
    ListNode *newNode = (ListNode *)malloc(sizeof(ListNode));
    if (newNode == NULL) {
        printf("内存分配失败，无法新增地点！\n");
        return 0;
    }
    newNode->data = spot;
    newNode->next = NULL;

    // 2. 尾插
    if (L->head == NULL) {
        L->head = newNode;
    } else {
        ListNode *p = L->head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = newNode;
    }

    L->length++;
    printf("地点【%s】新增成功！\n", spot.name);
    //记录操作
    OpStackElem e;
    e.opType = 1;
    e.oldSite = newNode->data;
	strcpy(e.opLog,"新增校园地点");
    PushOpStack(&g_opStack, e);
    printf("操作已记录，可撤销！\n");
    return 1;
}

// 根据地点编号删除节点
int DeleteSpot(SpotLinkList *L, int id) {
    if (L->head == NULL) {
        printf("链表为空，无地点可删除！\n");
        return 0;
    }

    ListNode *p = L->head, *q = NULL;
    // 1. 查找要删除的节点
    while (p != NULL && p->data.id != id) {
        q = p;
        p = p->next;
    }

    // 2. 未找到节点
    if (p == NULL) {
        printf("未找到编号为%d的地点！\n", id);
        return 0;
    }

    // 3. 删除节点
    if (q == NULL) {
        // 删除的是头节点
        L->head = p->next;
    } else {
        // 删除的是中间/尾节点
        q->next = p->next;
    }

    free(p);
    L->length--;
    printf("编号%d的地点删除成功！\n", id);
    OpStackElem e;
    e.opType = 2;
    e.oldSite = q->data;
	strcpy(e.opLog,"删除校园地点");
    PushOpStack(&g_opStack, e);
    printf("操作已记录，可撤销！\n");
    return 1;
}

// 根据编号修改地点信息
int UpdateSpot(SpotLinkList *L, int id, CampusSpot newSpot) {
    ListNode *p = L->head;
    while (p != NULL) {
        if (p->data.id == id) {
			CampusSpot oldData = p->data;
            p->data = newSpot;
            printf("编号%d的地点【%s】修改成功！\n", id, newSpot.name);
			OpStackElem e;
			e.opType = 3;
			e.oldSite = p->data;
			strcpy(e.opLog,"修改校园地点");
			PushOpStack(&g_opStack, e);
			printf("修改已记录，可撤销！\n");
            return 1;
        }
        p = p->next;
    }
    printf("未找到编号为%d的地点！\n", id);
    return 0;
}

// 按编号查询
CampusSpot* SearchById(SpotLinkList *L, int id) {
    ListNode *p = L->head;
    while (p != NULL) {
        if (p->data.id == id) {
            return &(p->data);
        }
        p = p->next;
    }
    return NULL;
}

// 按名称查询：遍历打印匹配结果
void SearchByName(SpotLinkList *L, char *name) {
    ListNode *p = L->head;
    int flag = 0;
    printf("\n查询结果：\n");
    while (p != NULL) {
        if (strstr(p->data.name, name) != NULL) {
            printf("编号：%d  名称：%s  描述：%s  坐标：(%.2f, %.2f)\n",
                   p->data.id, p->data.name, p->data.desc,
                   p->data.x, p->data.y);
            flag = 1;
        }
        p = p->next;
    }
    if (!flag) {
        printf("未找到包含【%s】的地点！\n", name);
    }
}
// 遍历链表，打印所有地点信息
void ShowAllSpots(SpotLinkList *L) {
    if (L->head == NULL) {
        printf("当前无存储的校园地点！\n");
        return;
    }

    ListNode *p = L->head;
    printf("\n===== 校园地点列表（共%d个）=====\n", L->length);
    printf("编号\t名称\t\t描述\t\t坐标\n");
	ListNode *last = NULL;
    while (p != NULL) {
        printf("%d\t%s\t\t%s\t(%.2f, %.2f)\n",
        p->data.id, p->data.name,
        p->data.desc, p->data.x, p->data.y);
        p = p->next;
    }
    if(last != NULL){
	     ViewStackElem ve;
	     ve.site = last->data;
	     PushViewStack(&g_viewStack, ve);
	 }    
    printf("\n请按任意键返回...");
    fflush(stdin);
    getchar();
}

// 撤销上一步增删改操作
void UndoOperate(SpotLinkList *L) {
    if (IsOpStackEmpty(&g_opStack)) {
        printf("暂无可撤销操作！\n");
        return;
    }
    OpStackElem e;
    PopOpStack(&g_opStack, &e);

    Node *p;
    if (e.opType == 1) {
        // 撤销新增
        DeleteSpot(L, e.oldSite.id);
        printf("撤销新增成功！\n");
    } else if (e.opType == 2) {
        // 撤销删除
        AddSpot(L, e.oldSite);
        printf("撤销删除成功！\n");
    } else if (e.opType == 3) {
        // 撤销修改
        p = L->head;
        while (p && p->data.id != e.oldSite.id)
            p = p->next;
        if (p) {
            p->data = e.oldSite;
            printf("撤销修改成功！\n");
        }
    }
}

// 查看地点浏览历史
void ShowViewHistory() {
    if (g_viewStack.top == -1) {
        printf("暂无浏览历史！\n");
        return;
    }
    printf("===== 浏览历史记录 =====\n");
    for (int i = 0; i <= g_viewStack.top; i++) {
        ViewStackElem e = g_viewStack.data[i];
        printf("编号：%d  名称：%s\n", e.site.id, e.site.name);
    }
}

// 清空所有操作+浏览历史
void ClearAllHistory() {
    ClearOpStack(&g_opStack);
    ClearViewStack(&g_viewStack);
    printf("所有历史记录已清空！\n");
}

// 查看全部操作历史记录
void showAllOpHistory()
{
    printf("===== 全部操作历史 =====\n");
       if(IsOpStackEmpty(&g_opStack)){
           printf("暂无任何操作记录！\n");
           printf("请按任意键继续...\n");
           int c;
           while( (c = getchar()) != '\n' && c != EOF );
           getchar();
           return;
       }
    // 倒序遍历
    for(int i = g_opStack.top; i >= 0; i--)
   	   {
   	        OpStackElem op = g_opStack.data[i];
   	        int num = g_opStack.top - i + 1;
   	        if(op.opType == 1)
   	        {
   	            printf("%d. 新增校园地点\n", num);
   	        }
   	        else if(op.opType == 2)
   	        {
   	            printf("%d. 删除校园地点\n", num);
   	        }
   			else if(op.opType == 3)
   	        {
   	            printf("%d. 修改校园地点\n", num);
   	        }
   	    }
   	    printf("\n请按任意键返回主菜单...");
       int c;
       while( (c = getchar()) != '\n' && c != EOF );
       getchar();
}

int main() {
    SpotLinkList spotList;
    InitList(&spotList);
    int choice;

    // 预存测试地点
    CampusSpot test1 = {1, "第一教学楼", "主教学楼，校园中心", 100.0, 200.0};
    CampusSpot test2 = {2, "第一食堂", "学生一食堂，靠近宿舍区", 300.0, 400.0};
    AddSpot(&spotList, test1);
    AddSpot(&spotList, test2);

    while (true) {
        printf("\n===== 校园导航系统 V3.0=====\n");
        printf("1. 新增地点\n");
        printf("2. 删除地点\n");
        printf("3. 修改地点\n");
        printf("4. 查询地点（按编号）\n");
        printf("5. 查询地点（按名称）\n");
        printf("6. 撤销上一步操作\n");//V2.0新增
        printf("7. 查看浏览记录\n");//V2.0新增
        printf("8. 清空历史\n");//V2.0新增
		printf("9. 查看全部操作日志\n");//V3.0新增
        printf("10. 展示所有地点\n");
        printf("0. 退出系统\n");
        printf("请输入操作选项：");
        scanf("%d", &choice);
        getchar();  // 清除缓冲区换行符

        switch (choice) {
            case 1: {
                CampusSpot newSpot;
                printf("请输入地点编号：");
                scanf("%d", &newSpot.id);
                getchar();
                printf("请输入地点名称：");
                gets(newSpot.name);
                printf("请输入地点描述：");
                gets(newSpot.desc);
                printf("请输入地点坐标x、y（用空格分隔）：");
                scanf("%f %f", &newSpot.x, &newSpot.y);
                AddSpot(&spotList, newSpot);
                break;
            }
            case 2: {
                int id;
                printf("请输入要删除的地点编号：");
                scanf("%d", &id);
                DeleteSpot(&spotList, id);
                break;
            }
            case 3: {
                int id;
                CampusSpot newSpot;
                printf("请输入要修改的地点编号：");
                scanf("%d", &id);
                getchar();
                printf("请输入新的地点名称：");
                gets(newSpot.name);
                printf("请输入新的地点描述：");
                gets(newSpot.desc);
                printf("请输入新的坐标x、y：");
                scanf("%f %f", &newSpot.x, &newSpot.y);
                newSpot.id = id;  // 保证编号一致
                UpdateSpot(&spotList, id, newSpot);
                break;
            }
            case 4: {
                int id;
                printf("请输入要查询的地点编号：");
                scanf("%d", &id);
                CampusSpot *spot = SearchById(&spotList, id);
                if (spot) {
                    printf("\n查询结果：\n编号：%d  名称：%s  描述：%s  坐标：(%.2f, %.2f)\n",
                           spot->id, spot->name, spot->desc, spot->x, spot->y);
                } else {
                    printf("未找到该地点！\n");
                }
                break;
            }
            case 5: {
                char name[50];
                printf("请输入要查询的地点关键词：");
                gets(name);
                SearchByName(&spotList, name);
                break;
            }
            case 6: {
                UndoOperate(&spotList);
                break;
            }
			case 7: {
                ShowViewHistory();
                break;
            }
			case 8:{
                ClearAllHistory(); 
                break;
            }
			case 9:{
			    showAllOpHistory();
				break;
			}	
            case 10:
                ShowAllSpots(&spotList);
                break;
            case 0:
                DestroyList(&spotList);  // 退出前释放内存
                printf("感谢使用校园导航系统V3.0，再见！\n");
                return 0;
            default:
                printf("输入错误，请重新选择！\n");
        }
    }
}
