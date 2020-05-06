#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>
using namespace  std;
typedef struct Node{
    int balance;
    int key;
    char *info;
    struct  Node *left, *right,*parent;
}Node;
const char* msgs[] = { "0. Quit", "1. Add", "2. Find", "3. Find max key", "4. Delete",
                       "5. Show", "6. Timing", "7. Show as tree"};
const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);
Node *NIL=(Node*)malloc(sizeof(Node));//Elist
Node *AvlTable=NIL;
int counter=0;
//basic
int dialog(const char *msgs[],int N);
int getInt(int *);
char *getStr();
//for tree
int D_Add(Node*);
int InsertTree(Node *);
//finds
Node *FindByKey( int );
Node *FindMax(Node *);
Node *Find(int);
Node *Tree_Successor(Node *);
Node *Tree_Min(Node *);
//deletes
Node *DeleteByKey(Node *);
int Delete_Balance(Node *,Node *);
int Delete_Fix(Node *);
//shows
int Show(Node *);
int Show_AsTree(Node *p,int count);
int D_ShowAs(Node *p);

int timing(Node *);
Node *newNode( int,char *);
int Balance(Node *,Node *);

//rotations
Node *Left_Rotate(Node *x);
Node *Right_Rotate(Node *y);


int D_Find(Node *);
int D_Delete(Node *);
int D_FindMax(Node *);
//for file
int Load(Node *);


int (*fptr[])(Node *)={nullptr,D_Add,D_Find,D_FindMax,D_Delete,Show,timing,D_ShowAs};
int main() {
    NIL->left=NIL;
    NIL->right=NIL;
    NIL->key=0;
    NIL->balance=0;
    NIL->parent=NIL;
    AvlTable->parent=NIL;
    Load(AvlTable);
    int rc;
    while((rc=dialog(msgs,NMsgs))){
        if(!fptr[rc](AvlTable))
            break;
    }
    printf("\nThat's all, bye!\n");
    free(AvlTable);
    return 0;
}

int dialog(const char *msgs[],int N){//выбор альтернативы(что сделать с таблицей?), возвращает номер альтернативы
    int rc;
    char *errmsg="";
    int i,n;
    do{
        puts(errmsg);
        errmsg="You are wrong,repeat please!";
        for (i=0;i<N;i++)
            puts(msgs[i]);
        puts("Make your choice");
        n=getInt(&rc);
        if(n==0)
            rc=0;
    }while(rc<0||rc>=N);
    return rc;
}
int getInt(int *a){
    int n;
    do{
        n=scanf("%d",a);
        if (n<0)
            return 0;
        if(n==0||*a<0){
            printf("%s\n", "Error! Please, repeat your input: ");
            scanf("%*[^\n]");
        }
    }while(n==0||*a<0);
    return 1;
}
char *getStr() {
    char *ptr = (char *)malloc(1);
    char buf[81];
    int n, len = 0;
    *ptr = '\0';
    do {
        n = scanf("%80[^\n]", buf);
        if (n < 0) {
            free(ptr);
            return nullptr;
        }
        else
        if (n) {
            len += strlen(buf);
            ptr = (char *)realloc(ptr, len + 1);
            int k = strcspn(buf, "\0"); // длина слова
            strncat(ptr, buf, k);
        }
    } while (n > 0);
    scanf("%*c");
    return ptr;
}
Node *Left_Rotate(Node *x){
    Node *y=x->right;
    x->right=y->left;
    if(y->left!=NIL)
        y->left->parent=x;//делаем х родителем левого ребенка у
    y->parent=x->parent; //родитель х-родитель и у
    if(y->parent==NIL){
        AvlTable=y;
    }
    else if(x==x->parent->left){
        x->parent->left=y;//если х-левый потомок
    }
    else
        x->parent->right=y;
    y->left=x;//теперь х-левый потомок y
    x->parent=y;
    return y;
}
Node *Right_Rotate(Node *y){
    Node *x=y->left;
    y->left=x->right;
    if(x->right!=NIL)
        x->right->parent=y;
    x->parent=y->parent;
    if(x->parent==NIL)
        AvlTable=x;
    else if(y==y->parent->right)
        y->parent->right=x;
    else
        y->parent->left=x;
    x->right=y;
    y->parent=x;
    return x;
}
//Find
Node *Tree_Min(Node *p){
    return(p->left!=NIL)?Tree_Min(p->left):p;
}
Node *FindMax(Node *p){
    return (p->right!=NIL)?FindMax(p->right):p;
}

Node *FindByKey(int key){
    Node *x = AvlTable;
    while (x != NIL && x->key != key) {
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    return x;
}
int D_Find(Node *p){
    //char *key=nullptr;
    int key;
    char *skey=nullptr;
    Node *tmp=nullptr;
    printf("\nInput key:\n");
    //getInt(&key);
    getStr();
    if(FindByKey(key)==NIL){
        printf("\n Element with this key wasn't found\n");
        return 1;
    }
    else{
        tmp=FindByKey(key);
        printf("\nInfo of element with key %d is :%s\n",tmp->key,tmp->info);
        return 1;
    }
}
Node *Find(int key) {
    Node *cur = AvlTable;
    while (cur != NIL) {
        counter++;
        if (cur->key == key) return cur;
        if (key < cur->key) cur = cur->left;
        else cur = cur->right;
    }
    return nullptr;
}

int D_FindMax(Node *p) {
    Node *tmp = FindMax(p);
    if (p==NIL) {
        printf("\nTree is empty\n");
        return 1;
    } else
        printf("\nThe max element has key %d and info :%s\n", tmp->key, tmp->info);
    return 1;
}
Node *newNode(int key,char *info){
    Node *node= (Node*)malloc(sizeof(Node));
    node->key=key;
    node->left=NIL;
    node->right=NIL;
    node->info=info;//лист,но не Elist
    return node;
}
int InsertTree( Node *z){
    Node *x, *y;

    x = AvlTable;
    y = NIL;

    // Просматриваем дерево до тех пор, пока не достигнем листа (NIL)

    while (x != NIL) {
        y = x;
        if (z->key <= x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent=y;
    if (y == NIL) {
        AvlTable = z;
        AvlTable->balance=0;
    } else if (z->key <= y->key) {
        y->left = z;
    } else {
        y->right = z;
    }
    Balance(AvlTable,z);
    return 0;

}
int Balance(Node *root,Node *z){
   Node *p=z->parent;
   while(p!=NIL&&p->balance==0){
       if(z==z->parent->left)//в левом поддереве новый узел
           p->balance=-1;
       else
           p->balance=1;
       z=p;
       p=z->parent;
   }
   if(p==NIL)
       return 1;
   //printf("%d",p->balance);
   if((z==z->parent->left&&p->balance==1)||(z==z->parent->right&&p->balance==-1)){
       p->balance=0;
       return 1;
   }
   else{
       //RIGHT IS LONGER
      //case 1
      if(z==p->right&&z->balance==1&&p->balance==1){
          Left_Rotate(p);
          z->balance=0;
          p->balance=0;
      }
      //case 2a:
      if(p->balance==1&&z->balance==-1&&z==p->right&&z->left->balance==1){
          Right_Rotate(z);
          Left_Rotate(z->parent->parent);
          p->balance=-1;
          z->balance=0;
          z->parent->balance=0;
      }
      //case 2b
      if(p->balance==1&&z->balance==-1&&z==p->right&&z->left->balance==-1){
          Right_Rotate(z);
          Left_Rotate(z->parent->parent);
          p->balance=0;
          z->balance=1;
          z->parent->balance=0;
      }
      //LEFT is longer
      //case 1
       if( p->balance==-1&&z->balance==-1&&z==p->left){
           Right_Rotate(p);
           z->balance=0;
           p->balance=0;
       }
       //case left 2a
       if(p->balance==-1&&z->balance==1&&z==p->left&&z->right->balance==-1){
           Left_Rotate(z);
           Right_Rotate(z->parent->parent);
           p->balance=1;
           z->balance=0;
           z->parent->balance=0;
       }
       //case left 2b
       if(p->balance==-1&&z->balance==1&&z==p->left&&z->right->balance==1){
           Left_Rotate(z);
           Right_Rotate(z->parent->parent);
           p->balance=0;
           z->parent->balance=0;
           z->balance=-1;
       }
   }
    return 1;
}

int D_Add(Node *p){
    Node *z;
    z = (Node *) malloc(sizeof(Node));
    char *skey=nullptr;
    int key;
    printf("Input key:\n");
    getInt(&key);
    if (Find(key) != nullptr) {
        printf("\nElement with this key already exists\n");
        return 1;
    }
    z->key = key;
    z->left = NIL;
    z->right = NIL;
    z->parent=NIL;
    z->balance=0;
    cin.sync(); // очистка буфера

    printf("Input info:\n");
    z->info = getStr();
    InsertTree(z);
    return 1;
}
int Show (Node *root) {
    if (root == NIL)
        return 1;
    Show(root->left);
    Show(root->right);
    printf("[ key : %d  info : %s ]\n", root->key, root->info);
    return 1;
}
//DELETE


int D_Delete(Node *p){
    printf("Input key:\n");
    int key;
    getInt(&key);

    if (FindByKey(key) == NIL) {
        printf("\nThere is no element with this key\n");
        return 1;
    }
    printf("info : %s",FindByKey(key)->info);
    Delete_Balance((DeleteByKey(FindByKey(key))),(FindByKey(key))->parent);
    return 1;
}
Node *Tree_Successor(Node *z){
    if(z->right!=NIL)
        return Tree_Min(z->right);
    Node *y=z->parent;
    while(y!=NIL&&z==y->right){
        z=y;
        y=y->parent;
    }
    return y;
}
Node  *DeleteByKey(Node *z){
    Node *y;
    Node *x;
    Node *tmp;
    if(z->left==NIL||z->right==NIL){
        y=z;
    }
    else
        y=Tree_Successor(z);
    if (y->left!=NIL)
        x=y->left;
    else
        x=y->right;
    if(x!=NIL)
        x->parent=y->parent;
    if(y->parent==NIL)
        AvlTable=x;
    else if(y==y->parent->left)
        y->parent->left=x;
    else
        y->parent->right=x;
    if(y!=z)
        z->key=y->key;
    if(y->right==NIL)
        tmp=y->left;
    else
        tmp=y->right;
    free(y);
    return tmp;
}
int Delete_Balance(Node *x,Node *z){
    //z-узел родителя удалённого элемента.Обработка случая,когда у узла не было потомков
    Node *p=x->parent;//х-дочерний узел удалённого элемента
    if(p==NIL&&x==NIL){
        if(z->left==NIL&&z->balance==0&&z->right!=NIL){//удалили слева
            z->balance=1;
            return 1;
        }
        if(z->left==NIL&&z->balance==-1){//удалили слева
            z->balance=0;
            return 1;
        }
        if(z->left==NIL&&z->balance==1){
            Left_Rotate(z);
            z->balance=0;
            return 1;
        }
        if(z->right==NIL&&z->balance==0&&z->left!=NIL){//удалили справа
            z->balance=-1;
            return 1;
        }
        if(z->right==NIL&&z->balance==1){
            z->balance=0;
            return 1;
        }
        if(z->right==NIL&&z->balance==-1){
            z->balance=0;
            Right_Rotate(z);
            return 1;
        }
    }
    while(p!=NIL){
        if(x==p->left)
            p->balance++;
        else
            p->balance--;
        if(fabs(p->balance)>1){
            Delete_Fix(x);
            return 1;
        } else{
            if(fabs(p->balance)==1)
                return 1;
            else{
                x=p;
                p=x->parent;
            }
        }
    }
    return 1;
}
int Delete_Fix(Node *x){
    Node *p=x->parent;
    //RIGHT IS LONGER
    //case 1 right is longer and after delete it becames longer
    if(p->balance==2&&x==p->left&&p->right->balance==1) {
        Left_Rotate(p);
        p->balance=0;
        p->parent->balance=0;
        printf(" 1 right");
    }
    //2a
    if(p->balance==2&&x==p->left&&p->right->balance==-1&&p->right->left->balance==1){
        Right_Rotate(p->right);
        Left_Rotate(p);
        p->balance=-1;
        p->parent->balance=0;
        p->parent->right->balance=0;
        printf(" \n 2a right");
    }
    //2b
    if(p->balance==2&&x==p->left&&p->right->balance==-1&&p->right->left->balance==-1){
        Right_Rotate(p->right);
        Left_Rotate(p);
        p->balance=0;
        p->parent->balance=0;
        p->parent->right->balance=1;
        printf(" \n 2b right");
    }
    //3
    if(p->balance==2&&x==p->left&&p->right->balance==0){
        Left_Rotate(p);
        p->balance=1;
        p->parent->balance=-1;
        printf("\n3 right");
    }
    //LEFT IS LONGER
    //case 1
    if(p->balance==-2&&x==p->right&&p->left->balance==-1){
        Right_Rotate(p);
        p->balance=0;
        p->parent->balance=0;
        printf("\n 1 left");
    }
    //case 2a
    if(p->balance==-2&&x==p->right&&p->left->balance==1&&p->left->right->balance==-1){
        Left_Rotate(p->left);
        Right_Rotate(p);
        p->balance=1;
        p->parent->left->balance=0;
        p->parent->balance=0;
        printf("\n 2a left");

    }
    //case 2b
    if(p->balance==-2&&x==p->right&&p->left->balance==1&&p->left->right->balance==1){
        Left_Rotate(p->left);
        Right_Rotate(p);
        p->parent->balance=0;
        p->parent->left->balance=-1;
        p->balance=0;
        printf("\n 2b left");
    }
    // case 3
    if(p->balance==-2&&x==p->right&&p->left->balance==0){
        Right_Rotate(p);
        p->balance=-1;
        p->parent->balance=1;
        printf("\n 3 left");
    }
    return 1;
}

int timing(Node *) {
    Node *ptr, *nd, *cur;
    int n = 10, key[10000], k, cnt = 1000000, i, m, j;
    clock_t first, last;
    srand(time(nullptr));
    while (n-- > 0) {
        for (i = 0; i < 10000; ++i)
            key[i] = rand() * rand();
        for (i = 0; i < cnt;) {
            k = rand() * rand();
            ptr = (Node *) malloc(sizeof(Node));
            ptr->right = NIL;
            ptr->left = NIL;
            ptr->key = k;
            ptr->balance=0;
            ptr->parent=NIL;
            nd = Find(k);
            if (!nd) {
                if (!InsertTree(ptr))
                    ++i;
            }
        }
        m = 0;
        first = clock();
        for (i = 0; i < 10000; ++i) {
            if (Find(key[i])) {
                ++m;
            }
        }
        last = clock();
        printf("%d items were found\n", m);
        printf("test #%d, number of nodes = %d, time = %ld\n",
               10 - n, (10 - n) * cnt, (last - first));
        cur = AvlTable;
        for (j = 0; cur != NIL; ++j) {
            cur = cur->left;
        }
        printf("tree height: %d\n", j);
        printf("checked elements: %d\n\n", counter);
    }
    return 1;
}
int D_ShowAs(Node *p){
    puts("TREE");
    Show_AsTree(AvlTable,0);
    return 1;
}
int Show_AsTree(Node *p,int count) {
    int i = 0;

    if (p == NIL)
        return 1;
    Show_AsTree(p->left, count + 1);


    for (; i < count; i++) {
        printf("  ");
    }
    printf("[%d;%s]\n", p->key, p->info);
    Show_AsTree(p->right, count + 1);
    return 1;
}
int Load(Node *root){
    printf("Input file name:\n");
    char *fname = getStr();
    strcat(fname, ".txt");
    cout << "\n" << fname;
    printf("\n  %s",fname);
    FILE *f = nullptr;
    f=fopen(fname, "r");
    if (!f) {
        printf("\nFile does not exist \n");
        return 1;
    }
    rewind(f);

    char *info = (char *) calloc(100, sizeof(char));
    int k;

    while (fscanf(f, "%d%s", &k, info) != EOF) {
        Node *newNode = (Node *) malloc(sizeof(Node));

        char *str = (char *) calloc(100, sizeof(char));

        strcpy(str, info);

        newNode->balance=0;
        newNode->left = NIL;
        newNode->right = NIL;
        newNode->key = k;
        newNode->info = str;

        InsertTree(newNode);
    }
    free(info);
    fclose(f);
    return 1;
}
