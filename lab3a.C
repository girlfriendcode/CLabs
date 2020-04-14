//
// Created by mi on 21.03.2020.
//
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

char *copyNum(char *from,char *to);
char *reorganize (char *s);
char *skipSpace(char *s);
char *skipNumber(char *s);
char *getStr();
int check (char *buf);

int main() {
    char *s=NULL;
    char *p=NULL;
    while(puts("enter string"),s=getStr()){
        printf("Source string: \"%s\"\n",s);
        p=reorganize(s);
        printf("Result string: \"%s\"\n",p);
        free(s);
        free(p);
    }
    puts("That's all.BYE!");
    return 0;
}
char *getStr(){
    char buf[81];
    char *ptr=(char*)malloc(1);
    int n,len=0;
    *ptr='\0';
    do{
        n=scanf("%80[^\n]",buf);
        if(n<0){
            free(ptr);
            ptr=NULL;
            continue;
        }
        if(n==0)
            scanf("%*c");
        else{
            len+=strlen(buf);//увеличиваем длину
            ptr=(char*)realloc(ptr,len+1);//изменяем длину массива
            strncat(ptr,buf,len);
        }
    }while(n>0);
    return ptr;
}
char *skipSpace(char *s){//пропуск пробела
    int k=strspn(s," \t");
    return s+k;
}
char *skipNumber(char *s){//пропуск числа
    int k=strcspn(s," \t");
    return s+k;
}

char *reorganize (char *s){
    int len=0;
   //буферная строка
    char *out=(char*)malloc(strlen(s));
    *out='\0';
    while(*(s=skipSpace(s))) {
        char *buf=(char*)malloc(strlen(s));
        *buf='\0';
        buf=copyNum(s,buf);
        if(check(buf)){
            if(*out=='\0'){
                out=copyNum(s,out);
            }
            else {
                strncat(out, " ", 1);
                out = copyNum(s, out);
            }
        }
        free(buf);
        s=skipNumber(s);
    }
    out=(char*)realloc(out,strlen(out));
    return out;
}
int check(char *buf){//проверка, состоит ли число из четных цифр только
    int n;
    n=atoi(buf);
    int i;
    while(n!=0){
        if((n%10)%2!=0){
            return 0;
        }
        n/=10;
    }
    return 1;
}
char *copyNum(char *from,char *to){
    int k=strcspn(from," \t");//длина числа
    strncat(to,from,k);
    return to;
}
