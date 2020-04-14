#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SIZE 15
typedef struct Item{
    int key;//ключ элемента
    int release;//номер версии элемента
    char *info;
    struct Item *next;
}Item;
Item *table[SIZE];
int N=0;
const char *msgs[]={"0.Quit","1.Add","2.Delete  by key","3.Delete by version ","4.Find by key","5.Find by version","6.Show table"};//альтернативы
const int NMsgs=sizeof(msgs)/sizeof(msgs[0]);
const char *errmsgs[]={"Ok","Overflow","Key not found","Unable to delete!"};
void delTable(Item *[]);
int dialog(const char *msgs[],int N);
int T_Show(Item *[]);
int DeleteByKey(Item *[]);//удаление всех версий элемента, заданного ключом
int DeleteByVersion(Item *[]);//удаление конкретной версии элемента, заданного ключом
int T_Add(Item *[]);
int FindByKey(Item *[]);
int FindLastRel(int key);
int FindByVers(Item *[]);
int (*fptr[])(Item *[]) = {NULL, T_Add,DeleteByKey, DeleteByVersion, FindByKey,FindByVers,T_Show};
int hashFunction(int );
int getInt(int *);
char *getStr();
int main() {
    int rc;
    while((rc=dialog(msgs,NMsgs))){
        if(!fptr[rc](table))
            break;
    }
    printf("That's all.Bye!\n");
    delTable(table);
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
int hashFunction(int key){
    return key % 10;
}
int FindLastRel(int key){
    int rel=0;
    Item *tmpItem;
    int position=hashFunction(key);
    if(table[position]==NULL){
        rel=0;
        return rel;

    }
    else{
        tmpItem=table[position];
        while(tmpItem){
            if(tmpItem->key==key){
                rel=tmpItem->release;
                return rel;
            }
            else
                tmpItem=tmpItem->next;
        }
    }
    return -1;
}
int T_Add(Item *table[]){
    int key,n,release;

    printf("Enter key and info :--->");
    n=getInt(&key);
    if(n==0)
        return 0;
    char *info =NULL;
    info=getStr();
    if (info==NULL)
        return 0;
    int position=hashFunction(key);
    Item *tmpItem;
    Item *pItem=(Item*)malloc(sizeof(Item));
    pItem->key=key;
    pItem->info=info;
    if(FindLastRel(key)==0) {
        if(N>=SIZE){
            puts(errmsgs[1]);
            return 0;
        }
        pItem->release=1;
        pItem->next=NULL;
        table[position] = pItem;
        N++;
    }
    else{
        tmpItem=table[position];
        if(FindLastRel(key)>0)
            pItem->release=FindLastRel(key)+1;
        else
            pItem->release=1;
        pItem->next=tmpItem;
        table[position]=pItem;
    }
    puts(errmsgs[0]);
    return 1;
}
int FindByKey(Item *table[]) {
    int key, n, release;
    int flag = 0;
    printf("Enter key:--->\n");
    n = getInt(&key);
    if (n == 0)
        return 0;
    int position = hashFunction(key);
    Item *tmpItem;
    if (table[position] == NULL)
    {
        puts(errmsgs[2]);
        return 1;
    }
    else{
        tmpItem=table[position];
        while(tmpItem){
            for(int i=0;tmpItem;i++) {
                if (tmpItem->key == key) {
                    printf("[%d]  [key:%d  info:%s]\n",i, tmpItem->release - 1, tmpItem->info);
                    flag = 1;
                }
                tmpItem = tmpItem->next;
            }
        }
        if(!flag) {
            puts(errmsgs[2]);
            return 0;
        }
    }
    printf("\n");
    puts(errmsgs[0]);
    return 1;
}
int FindByVers(Item *table[]){
    int key,release,n;
    int flag = 0;
    printf("Enter key--->\n");
    n = getInt(&key);
    if (n == 0)
        return 0;
    int position = hashFunction(key);
    printf("Enter release--->\n");
    n=getInt(&release);
    if(n==0)
        return 0;
    Item *tmpItem;
    if(table[position]==NULL)
    {
        puts(errmsgs[2]);
        return 1;
    }
    else{
        tmpItem=table[position];
        while(tmpItem){
            if(tmpItem->key==key&&tmpItem->release==release)
            {
                printf("[key:%d  release:%d  info:%s ]",tmpItem->key,tmpItem->release,tmpItem->info);
                flag=1;
            }
            tmpItem=tmpItem->next;
        }
        if(!flag) {
            puts(errmsgs[2]);
            return 1;
        }
    }
    printf("\n");
    puts(errmsgs[0]);
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
            return NULL;
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
int DeleteByKey(Item *table[]) {
    int key, n;
    printf("Enter key--->\n");
    n = getInt(&key);
    if (n == 0)
        return 0;
    int position = hashFunction(key);
    if (table[position]== NULL) {
        puts(errmsgs[3]);
        return 0;
    }
    int flag=0;
    Item *prevItem = NULL;
    Item *tmpItem = table[position];
    while (tmpItem) {
        if(tmpItem->key==key){
            if(prevItem==NULL){//первый элемент в списке
                if(table[position]->next!=NULL) {
                    table[position] = tmpItem->next;
                }
                else
                    free(table[position]);
                N--;
            }
            else
                prevItem->next=tmpItem->next;
            free(tmpItem->info);
            free(tmpItem);
            flag++;
            tmpItem=table[position];
        }
        prevItem=tmpItem;
        tmpItem=tmpItem->next;
    }
    if(!flag)
        puts(errmsgs[3]);
    else
        puts(errmsgs[0]);
    return 1;
}
int DeleteByVersion(Item *table[]){
    int key, n,release;
    printf("Enter key--->\n");
    n = getInt(&key);
    if (n == 0)
        return 0;
    printf("Enter release--->\n");
    n = getInt(&release);
    if (n == 0)
        return 0;
    int position=hashFunction(key);
    if(table[position]==NULL){
        puts(errmsgs[3]);
        return 0;
    }
    Item *tmpItem,*prevItem=NULL;
    tmpItem=table[position];
    while(tmpItem){
        if ((tmpItem->key == key) && (tmpItem->release == release)){
            if(prevItem==NULL)
                table[position]=table[position]->next;
            else
                prevItem->next=tmpItem->next;
            free(tmpItem->info);
            free(tmpItem);
            puts(errmsgs[0]);
            return 1;
        }
        prevItem=tmpItem;
        tmpItem=tmpItem->next;
    }
    puts(errmsgs[3]);
    return 1;
}

int T_Show(Item *table[]){

    Item *tmpItem;
    for(int i=0; i<SIZE; i++){
        if((table[i])!=NULL){
            printf("[%d]",i);
            tmpItem = table[i];

            while(tmpItem){
                printf( "[key: %d  release: %d   info:%s  ]",tmpItem->key,tmpItem->release,tmpItem->info);
                tmpItem = tmpItem->next;

            }
            printf("\n");

        }
        else
            printf("[%d] ...\n", i);
    }
    return 1;
}
void delTable(Item *table[]){
    Item *tmpItem,*prevItem=NULL, *trash;
    for(int i=0; i<SIZE; i++){
        if(table[i]){
            tmpItem = table[i];
            prevItem = NULL;
            while(tmpItem){
                if(tmpItem->next != NULL){
                    if(prevItem == NULL){
                        table[i] = tmpItem->next;
                        trash = tmpItem;
                        tmpItem = tmpItem->next;
                        free(trash);

                    }
                    else{
                        trash = tmpItem;
                        prevItem->next = tmpItem->next;
                        tmpItem = tmpItem->next;
                        free(trash);
                    }

                }
                prevItem = tmpItem;
                tmpItem = tmpItem->next;

            }

        }

    }
}
