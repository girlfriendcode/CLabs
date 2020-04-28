#include <cstring>
#include <iostream>
#include <string>

using namespace std;
const int SIZE = 15;
struct Item {
    int len;
//    char *info;
    int offset;
    int key;
    int release;
    struct Item *next;
};

struct Table {
    Item *tableitem[SIZE]{};
    FILE *ft{};
    char *fname;
    int n = SIZE;
};


const char *msgs[]={"0.Quit","1.Add","2.Delete  by key","3.Delete by version ","4.Find by key","5.Find by version","6.Show table","7.Save"};//альтернативы
const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);
int N=0;
int getNaturalInt(int *a);
int hashFunction(int key);
char *getStr();
//Функции для работы с таблицей
int dialog(const char *msgs[], int N);
int FindByKey(Table *);
int FindLastRel(Table *,int );
int FindByRel(Table *);
int DeleteByKey(Table *);
int DeleteByRel(Table*);
int T_Show(Table *);
int D_Load(Table *);
int insertInTable(Table *);
Item *getLast(Item *);
void pushBack(Item *,Item *);
void delTable(Table *);
//для работы с файлом
int load(Table *pTable,char *fname);
int save(Table *);
char *findInfo(Table *, int,int  );
Item *findItemByKey(Table *, int, int  );

int (*fptr[])(Table *) ={nullptr, insertInTable, DeleteByKey,DeleteByRel,FindByKey,FindByRel, T_Show, save};

int main() {
    int rc = 0;
    Table *table = new Table;
    if(D_Load(table)==0)
        return 1;

    while ((rc = dialog(msgs, NMsgs))) {
        if (!fptr[rc](table))
            break;
    }
    printf("That's all. Bye!\n");
    fclose(table->ft);
    free(table);
    return 0;
}
int D_Load(Table *ptab){
    int rc=1;
    char *filename=nullptr;
    printf("Input file name:\n");
    filename=getStr();
    rc=load(ptab,filename);
    if(rc==0)
        return 0;
    return rc;
}

// Заносим в таблицу ft данные о смещениях и в таблицу fd саму информацию
int insertInTable(Table *ptab) {
    int key;
    //работа с файлом
    printf(" Input key: ");
    getNaturalInt(&key);
    cin.sync(); // очистка буфера
    char *str = nullptr;
    printf("Input  information: ");
    str = getStr();
    char *ftname = (char *) malloc(sizeof(char) * (strlen(ptab->fname) + 4));
    strcpy(ftname, ptab->fname);
    strcat(ftname, ".tab");
    // создали файл для записи offset
    ptab->ft=fopen( ftname, "w+b");
    if (ptab->ft == nullptr) {
        printf("\nCan't open\n");
        return 0;
    }
    char *fdname = (char *) malloc((strlen(ptab->fname) + 4) * sizeof(char));
    strcpy(fdname, ptab->fname);
    strcat(fdname, ".dat");
    FILE *fd; // Запись в файл со словами
    fd=fopen( fdname, "a+t");
    auto *item = (Item *) malloc(sizeof(Item)); // вставляемый элемент
    item->key = key;
    item->len = (int) strlen(str) + 1;
    fseek(fd, 0, SEEK_END);
    item->offset = ftell(fd);
    printf("Offset is: %d", item->offset);

    //Вставка в таблицу
    int position=hashFunction(key);
    Item *tmpItem = nullptr;
    if (FindLastRel(ptab,item->key) ==0) {
        if(N>=SIZE){
            puts("Overflow");
            return 0;
        }
        item->release=1;
        item->next = nullptr;
        ptab->tableitem[position] = item;
        N++;
    }
    else {
        tmpItem = ptab->tableitem[position];
        if(FindLastRel(ptab,item->key)>0)
            item->release=FindLastRel(ptab,item->key)+1;
        else
            item->release=1;
        item->next=tmpItem;
        ptab->tableitem[position]=item;
    }
    printf("\nitem successfully added!\n");

    fwrite(str, sizeof(char), item->len, fd);

    fclose(fd);
    fclose(ptab->ft);
    return 1;
}
int T_Show(Table *ptab) {
    Item *tmpItem;
    int size =SIZE;
    cout << endl;

    for (int i = 0; i < size; i++) {
        if ((ptab->tableitem[i]) != nullptr) { // Проверка на пустоту списков элементов
            cout << "[" << i << "]";
            tmpItem = ptab->tableitem[i];
            while (tmpItem) {
                printf("\n    [key: %d | release: %d | size of info: %d | info: %s ]", tmpItem->key,tmpItem->release, tmpItem->len - 1,findInfo(ptab, tmpItem->key,tmpItem->release));
//                    cout << "\n    [key: " << tmpItem->key << " | info: " << findInfo(table, tmpItem->key) << "] ";
                tmpItem = tmpItem->next;
            }
            cout << endl;
        } else cout << "[" << i << "] " << "..." << endl;
    }
    return 1;
}
int DeleteByKey(Table * ptab){
    int key, n;
    printf("Enter key--->\n");
    getNaturalInt(&key);
    int position = hashFunction(key);
    if (ptab->tableitem[position]== nullptr) {
        printf("\nUnable to delete!\n");
        return 0;
    }
    int flag=0;
    Item *prevItem = nullptr;
    Item *tmpItem = ptab->tableitem[position];
    while (tmpItem) {
        if(tmpItem->key==key){
            if(prevItem==nullptr){//первый элемент в списке
                if(ptab->tableitem[position]->next!=nullptr) {
                    ptab->tableitem[position] = tmpItem->next;
                }
                else
                    free(ptab->tableitem[position]);
                N--;
            }
            else
                prevItem->next=tmpItem->next;
            free(tmpItem);
            flag++;
            tmpItem=ptab->tableitem[position];
        }
        prevItem=tmpItem;
        tmpItem=tmpItem->next;
    }
    if(!flag)
        puts("Unable to delete!");
    else
        puts("OK");
    return 1;
}
int DeleteByRel(Table *ptab){
    int key, n,release;
    printf("Enter key--->\n");
    n = getNaturalInt(&key);
    if (n == 0)
        return 0;
    printf("Enter release--->\n");
    n = getNaturalInt(&release);
    if (n == 0)
        return 0;
    int position=hashFunction(key);
    if(ptab->tableitem[position]==nullptr){
        puts("Unable to delete!");
        return 0;
    }
    Item *tmpItem,*prevItem=nullptr;
    tmpItem=ptab->tableitem[position];
    while(tmpItem){
        if ((tmpItem->key == key) && (tmpItem->release == release)){
            if(prevItem==nullptr)
                ptab->tableitem[position]=ptab->tableitem[position]->next;
            else
                prevItem->next=tmpItem->next;
            free(tmpItem);
            puts("OK");
            return 1;
        }
        prevItem=tmpItem;
        tmpItem=tmpItem->next;
    }
    puts("Unable to delete!");
    return 1;
}
int FindByKey(Table *ptab) {
    int key, n, release;
    int flag = 0;
    printf("Enter key:--->\n");
    n = getNaturalInt(&key);
    if (n == 0)
        return 0;
    int position = hashFunction(key);
    Item *tmpItem;
    if (ptab->tableitem[position] == NULL)
    {
        puts("Key not found!");
        return 1;
    }
    else{
        tmpItem=ptab->tableitem[position];
        while(tmpItem){
            for(int i=0;tmpItem;i++) {
                if (tmpItem->key == key) {
                    printf("[%d]  [key:%d info: %s]\n",i, tmpItem->release - 1,findInfo(ptab,tmpItem->key,tmpItem->release));
                    flag = 1;
                }
                tmpItem = tmpItem->next;
            }
        }
        if(!flag) {
            puts("Key not found");
            return 0;
        }
    }
    printf("\n");
    puts("OK");
    return 1;
}
int FindByRel(Table *ptab){
    int key,release,n;
    int flag = 0;
    printf("Enter key--->\n");
    n = getNaturalInt(&key);
    if (n == 0)
        return 0;
    int position = hashFunction(key);
    printf("Enter release--->\n");
    n=getNaturalInt(&release);
    if(n==0)
        return 0;
    Item *tmpItem;
    if(ptab->tableitem[position]==NULL)
    {
        puts("Key not found");
        return 1;
    }
    else{
        tmpItem=ptab->tableitem[position];
        while(tmpItem){
            if(tmpItem->key==key&&tmpItem->release==release)
            {
                printf("[key:%d  release:%d  info:%s ]",tmpItem->key,tmpItem->release,findInfo(ptab,tmpItem->key,tmpItem->release));
                flag=1;
            }
            tmpItem=tmpItem->next;
        }
        if(!flag) {
            puts("Key not found");
            return 1;
        }
    }
    printf("\n");
    puts("OK");
    return 1;
}
int FindLastRel(Table *ptab,int key){
    int rel=0;
    Item *tmpItem;
    int position=hashFunction(key);
    if(ptab->tableitem[position]==NULL){
        rel=0;
        return rel;

    }
    else{
        tmpItem=ptab->tableitem[position];
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
void delTable(Table *ptab){
    Item *tmpItem,*prevItem=NULL, *trash;
    for(int i=0; i<SIZE; i++){
        if(ptab->tableitem[i]){
            tmpItem = ptab->tableitem[i];
            prevItem = NULL;
            while(tmpItem){
                if(tmpItem->next != NULL){
                    if(prevItem == NULL){
                        ptab->tableitem[i] = tmpItem->next;
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

//для работы с файлом
int load(Table *pTable, char *fname) {
    FILE *fd = nullptr;
    int len = strlen(fname);  // длина основной части имени файла
    pTable->fname = fname;
    // формируем имена файлов для таблицы
    char *fdname = (char *) malloc((len + 4) * sizeof(char)); // Файл информации таблицы
    strcpy(fdname, pTable->fname);
    strcat(fdname, ".dat");
    char *ftname = (char *) malloc(sizeof(char) * (len + 4));  // Файл структуры таблицы
    strcpy(ftname, pTable->fname);
    strcat(ftname, ".tab");
    fd=fopen(fdname, "rb");
    // Если он существует, то работаем с ним
    if (fd != nullptr) { // считываем данные
        printf("Files have already created: %s %s\n", fdname, ftname);
        // открываем файлы с отступами на чтение и запись
        pTable->ft=fopen(ftname, "r+b");
        if (pTable->ft == nullptr) {
            free(ftname); // если нет, то ошибка
            cout << "\nError: cannot find .tab file \n";
            return 0;
        }
        auto *item = (Item *) malloc(sizeof(Item));
        int position;
        // вставляем элемент из файла в таблицу
        while (fread(&item->len, sizeof(int), 1, pTable->ft) &&
               fread(&item->offset, sizeof(int), 1, pTable->ft) &&
               fread(&item->key, sizeof(int), 1, pTable->ft)&&fread(&item->release,sizeof(int),1,pTable->ft)) {

            printf("%d %d %d\n", item->len, item->offset, item->key);
            position = hashFunction(item->key);

            if (pTable->tableitem[position] == nullptr) { // Если первая ячейка пуста
                item->next = nullptr;
                pTable->tableitem[position] = new Item;
                pTable->tableitem[position]->len = item->len;
                pTable->tableitem[position]->offset = item->offset;
                pTable->tableitem[position]->key = item->key;
                pTable->tableitem[position]->release=item->release;
                pTable->tableitem[position]->next = nullptr;

                cout << "\nOK\n";
            } else { // Если нет
                pushBack(pTable->tableitem[position], item);
            }
            cout << endl << "Elements from your file were added!\n" << endl;
        }
        free(item);
        fclose(pTable->ft);
        fclose(fd); // закрываем файл
        return 1;
    } else { // иначе создаем файл данных
        pTable->ft=fopen( ftname, "w+b");
        if (pTable->ft != nullptr) {
            printf("New files: %s %s were created\n\n", fdname, ftname);
            fclose(pTable->ft);
            fclose(fd);
            return 1;
        }
    }
    free(ftname);
    return 1;
}
int save(Table *ptab) {
    char *ftname = (char *) malloc((strlen(ptab->fname) + 4) * sizeof(char));
    strcpy(ftname, ptab->fname);
    strcat(ftname, ".tab");
    printf("\nSaving to %s \n", ftname);
    ptab->ft=fopen(ftname, "w+b");//открываем файл таблицы для записи структуры
    if (ptab->ft == nullptr) {
        printf("\nIs Null\n");
        return 0; // ошибка при создании файла
    }
    Item *cur = nullptr;
    // запись файла в таблицу
    for (int i = 0; i < SIZE; i++) {
        for (cur = ptab->tableitem[i]; cur; cur = cur->next) {
            fwrite(&(cur->len), sizeof(int), 1, ptab->ft);
            fwrite(&(cur->offset), sizeof(int), 1, ptab->ft);
            fwrite(&(cur->key), sizeof(int), 1, ptab->ft);
            fwrite(&(cur->release),sizeof(int),1,ptab->ft);
        }
    }
    printf("OK");
    printf("\nFile  \"%s\" was saved\n", ftname);
    fclose(ptab->ft);
    return 1;
}

Item *findItemByKey(Table *pTable, int key,int rel) {// Возвращает адрес элемента по ключу
    int position = hashFunction(key);
    Item *cur;
    for (cur = pTable->tableitem[position]; cur; cur = cur->next) {
        if (cur->key == key&&cur->release==rel)
            return cur;
    }
    return nullptr;
}
char *findInfo(Table *ptab, int key,int rel) {//  Возвращает информацию по ключу
    char *info = nullptr;
    Item *tmpItem = findItemByKey(ptab, key,rel);
//    printf("\nSize: %d | Offset: %d | Key: %d: \n", tmpItem->size, tmpItem->offset, tmpItem->key);
    FILE *fd = nullptr;
    char *fdatname = (char *) malloc((strlen(ptab->fname) + 4) * sizeof(char));
    strcpy(fdatname, ptab->fname);
    strcat(fdatname, ".dat");
    fd=fopen(fdatname, "rb");//открываем для чтения
    info = (char *) malloc(tmpItem->len);
    fseek(fd, tmpItem->offset, SEEK_SET);
    fread(info, sizeof(char), tmpItem->len, fd);
    fclose(fd);
    return info;
}
//others
int dialog(const char *msgs[], int Ni) {
     char *errmsg = "";
    int rc, n;

    do {
        puts(errmsg);
        errmsg="You are wrong. Repeat, please";
        for (int j = 0; j < Ni; ++j)
            puts(msgs[j]);
        puts("Make your choice: --> ");

        n = getNaturalInt(&rc);
        if (n == 0)
            rc = 0;
    } while (rc < 0 || rc >= Ni);
    return rc;
}

int getNaturalInt(int *a) {
    int n;
    do {
        n = scanf("%d", a);
        if (n < 0)
            return 0;
        if (n == 0 || *a < 0) {
            printf("%s\n", "Error! Please, repeat your input: ");
            scanf("%*[^\n]");
        }
    } while (n == 0 || *a < 0);
    return 1;
}

char *getStr() {
    char *ptr = (char *) malloc(1);
    char buf[81];
    int n, len = 0;
    *ptr = '\0';
    do {
        n = scanf("%80[^\n]", buf);
        if (n < 0) {
            free(ptr);
            return nullptr;
        } else if (n) {
            len += strlen(buf);
            ptr = (char *) realloc(ptr, len + 1);
            int k = strcspn(buf, "\0"); // длина слова
            strncat(ptr, buf, k);
        }
    } while (n > 0);
    scanf("%*c");
    return ptr;
}

int hashFunction(int key) {
    return key % SIZE;
}
Item *getLast(Item *head) {//получаем указатель на последний элемент списка
    if (head == nullptr) {
        return nullptr;
    }
    while (head->next) {
        head = head->next;
    }
    return head;
}
void pushBack(Item *head,Item *item) {
    Item *last = getLast(head);
    auto *tmp = (Item*) malloc(sizeof(Item));
    tmp->key =item->key;
    tmp->len = item->len;
    tmp->offset = item->offset;
    tmp->next = nullptr;//делаем концом
    last->next = tmp;
}