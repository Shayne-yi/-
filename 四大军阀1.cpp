#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//地点信息结构体
typedef struct {
    int id;             // 地点编号
    char name[50];      // 地点名称
    char desc[100];     // 地点描述
    float x, y;         // 地点坐标
} CampusSpot;

//链表节点结构体
typedef struct Node {
    CampusSpot data;    // 节点存储的地点数据
    struct Node *next;  // 指向下一个节点的指针
} ListNode;

//链表头结构体
typedef struct {
    ListNode *head;     // 链表头指针
    int length;         // 链表当前元素个数
} SpotLinkList;
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
    return 1;
}
// 根据编号修改地点信息
int UpdateSpot(SpotLinkList *L, int id, CampusSpot newSpot) {
    ListNode *p = L->head;
    while (p != NULL) {
        if (p->data.id == id) {
            p->data = newSpot;
            printf("编号%d的地点【%s】修改成功！\n", id, newSpot.name);
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
    while (p != NULL) {
        printf("%d\t%s\t\t%s\t(%.2f, %.2f)\n",
               p->data.id, p->data.name,
               p->data.desc, p->data.x, p->data.y);
        p = p->next;
    }
    printf("================================\n\n");
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
        printf("\n===== 校园导航系统 V1.0（链表版）=====\n");
        printf("1. 新增地点\n");
        printf("2. 删除地点\n");
        printf("3. 修改地点\n");
        printf("4. 查询地点（按编号）\n");
        printf("5. 查询地点（按名称）\n");
        printf("6. 展示所有地点\n");
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
            case 6:
                ShowAllSpots(&spotList);
                break;
            case 0:
                DestroyList(&spotList);  // 退出前释放内存
                printf("感谢使用校园导航系统V1.0（链表版），再见！\n");
                return 0;
            default:
                printf("输入错误，请重新选择！\n");
        }
    }
}
