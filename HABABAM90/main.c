#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define SETOFF 0xffff
#define MAXMATCH  ((1<<4) - 1)
#define MAXDISTANT ((SETOFF)-MAXMATCH)/(MAXMATCH+1)
#define DISTANT(d) (d >> 4)
#define LENGTH(d) (d & MAXMATCH)
#define KEEP(d,r) (d << 4 | r)
struct Node{

    unsigned short sset;
    char ch;

};
struct lzss{

    uint8_t pg;
    uint8_t g;

};
struct deflate{

    uint8_t binary;

};
struct huffman
{
    struct lzss *data;
    int freq;
    struct huffman *left;
    struct huffman *right;
};

struct huffman_array {

    int cap;
    int size;
    struct huffman **items;

};
char *file_read(FILE *f, int *size)
{

    char *content;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    content = malloc(*size);
    fseek(f, 0, SEEK_SET);
    fread(content, 1, *size, f);
    *(content+*size)='\0';
    return content;

}
struct lzss *deflate(char *kaynak_metin,int metin_boyutu,int *size,int *location,int freq[]){

    struct lzss *temp=malloc(sizeof(struct lzss));
    int i;
    int j;
    int match;
    int count;
    *size=0;
    for(i=*location;i<metin_boyutu;i++){
        int keep=i;
        int keep2=0;
        match=0;
        for(j=0;j<i;j++){
            if(i-j<=255){
            int p=0;
            while(*(kaynak_metin+i+p)==*(kaynak_metin+j+p)){
                p++;
                if(p+j>=keep) break;
                if(p==15){
                    break;
                }
            }
            if(p>match && p>=3){
                match=p;
                keep2=j;
            }
            }
        }
        if(match>0){
            int flag=0;
        for(count=0;count<*size;count++){
                if(temp[count].pg==keep-keep2 && temp[count].g==match){
                    freq[count]=freq[count]+1;
                    flag=1;
                    break;
                }
        }
        if(flag==0){
            *size=*size+1;
            temp=realloc(temp,(*size)*sizeof(struct lzss));
            temp[*size-1].pg=keep-keep2;
            temp[*size-1].g=match;
            freq[*size-1]=freq[*size-1]+1;
        }
        }
        else{
        int flag=0;
        for(count=0;count<*size;count++){
                if(temp[count].pg==0 && temp[count].g==*(kaynak_metin+i+match)){
                    freq[count]=freq[count]+1;
                    flag=1;
                    break;
                }
        }
        if(flag==0){
            *size=*size+1;
            temp=realloc(temp,(*size)*sizeof(struct lzss));
            temp[*size-1].pg=0;
            temp[*size-1].g=*(kaynak_metin+i+match);
            freq[*size-1]=freq[*size-1]+1;
        }
        }
        if(*size==45){
            break;
        }
        if(*(kaynak_metin+i+match)=='\0'){
                break;
        }
        i=i+match;
    }
    *location=i+match;

    return temp;

};
struct lzss *base(char *kaynak_metin,int metin_boyutu,int *size,int location,int start,int freq[]){

    struct lzss *temp=malloc(sizeof(struct lzss));
    int i;
    int j;
    *size=0;
    for(i=start;i<location;i++){
        *size=*size+1;
        temp=realloc(temp,(*size)*sizeof(struct lzss));
        int match=0;
        int keep=i;
        int keep2=0;
        for(j=0;j<i;j++){
            if(i-j<=255){
            int p=0;
            while(*(kaynak_metin+i+p)==*(kaynak_metin+j+p)){
                p++;
                if(p+j>=keep) break;
                if(p==15){
                    break;
                }
            }
            if(p>match && p>=3){
                match=p;
                keep2=j;
            }
            }
        }
        if(match>0){
        temp[*size-1].pg=keep-keep2;
        temp[*size-1].g=match;
        }
        else{
        temp[*size-1].pg=0;
        temp[*size-1].g=*(kaynak_metin+i+match);
        }
        if(*(kaynak_metin+i+match)=='\0'){
                break;
        }
        i=i+match;
    }

    return temp;


}
struct huffman_array *huffman_array_new()
{
    struct huffman_array *arr = malloc(sizeof(struct huffman_array));
    arr->cap = 8;
    arr->size = 0;
    arr->items = malloc(arr->cap * sizeof(struct huffman ));
    return arr;
}
struct huffman *huffman_array_pop(struct huffman_array *arr)
{
    return arr->items[--arr->size];
}
struct Node *compress(char *kaynak_metin,int metin_boyutu,int *size){

    struct Node *temp=malloc(sizeof(struct Node));
    int i;
    int j;
    *size=0;
    for(i=0;i<metin_boyutu;i++){
        *size=*size+1;
        temp=realloc(temp,(*size)*sizeof(struct Node));
        int match=0;
        int keep=i;
        int keep2=0;
        for(j=0;j<i;j++){
            if(i-j<=MAXDISTANT){
            int p=0;
            while(*(kaynak_metin+i+p)==*(kaynak_metin+j+p)){
                p++;
                if(p+j>=keep) break;
                if(p==MAXMATCH){
                    break;
                }
            }
            if(p>match){
                match=p;
                keep2=j;
            }
            }
        }
        if(match>0){
        temp[*size-1].sset=KEEP((keep-keep2),match);
        temp[*size-1].ch=*(kaynak_metin+i+match);
        }
        else{
        temp[*size-1].sset=0;
        temp[*size-1].ch=*(kaynak_metin+i+match);
        }
        if(temp[*size-1].ch=='\0'){
                break;
        }
        i=i+match;
    }

    return temp;

};
int binaryToDecimal(int p)
{
    int num = p;
    int dec_value = 0;

    int base = 1;

    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;

        dec_value += last_digit * base;

        base = base * 2;
    }

    return dec_value;
}
void huffmantree_print(struct huffman *t, int llist[],int *deflate_size,char *prefix, int size_prefix)
{

    if (t->left == NULL && t->right == NULL)
    {
        prefix[size_prefix] = 0;
        llist[*deflate_size]=binaryToDecimal(atoi(prefix));
        return;
    }

    if (t->left)
    {
        prefix[size_prefix++] = '0';
        *deflate_size=*deflate_size+1;
        huffmantree_print(t->left, llist,deflate_size,prefix, size_prefix);
        size_prefix--;
    }
    if (t->right)
    {
        prefix[size_prefix++] = '1';
        *deflate_size=*deflate_size+1;
        huffmantree_print(t->right, llist,deflate_size,prefix, size_prefix);
        size_prefix--;
    }


}
void huffman_array_sort(struct huffman_array *arr)
{
    int i, j;
    int max_index, max_value;

    for (i = 0; i < arr->size - 1; i++)
    {
        max_index = i;
        max_value = arr->items[i]->freq;

        for (j = i + 1; j < arr->size; j++)
        {
            if (arr->items[j]->freq > max_value)
            {
                max_value = arr->items[j]->freq;
                max_index = j;
            }
        }

        if (i != max_index)
        {

            struct huffman *temp = arr->items[max_index];
            arr->items[max_index] = arr->items[i];
            arr->items[i] = temp;
        }

    }
}
void huffman_array_add(struct huffman_array *arr, struct huffman *t)
{
    if (arr->size + 1 == arr->cap)
    {
        arr->cap *= 2;
        arr->items = realloc(arr->items, arr->cap * sizeof(struct huffman *));
    }

    arr->items[arr->size++] = t;

}
int main()
{

    FILE *f;
    FILE *file;
    FILE *file_;
    FILE *file__;
    int metin_boyutu;
    char *kaynak_metin;
    int i,j;
    int size;
    int start=0;
    int location=0;
    int result;
    int result_;
    struct lzss *array=malloc(sizeof(struct lzss));
    struct Node *encoded_metin=malloc(sizeof(struct Node));
    if ((f = fopen("source.txt", "r"))!=NULL)
    {
        kaynak_metin=file_read(f,&metin_boyutu);
    }

    if ((file = fopen("deflate.txt", "w"))!=NULL)
    {

    }
    if ((file_ = fopen("key.txt", "w"))!=NULL)
    {

    }

    encoded_metin=compress(kaynak_metin,metin_boyutu,&size);

    if ((file__ = fopen("encoded.txt", "w"))!=NULL)
    {
        fwrite(encoded_metin,sizeof(struct Node),size,file__);
    }

    result=sizeof(struct Node)*size;
    printf("Baslangic boyutu : %d\n",metin_boyutu);
    printf("Lz77 encode boyutu : %d\n",result);

    size=0;
    result=0;
    result_=0;

    while(location<metin_boyutu){
    int freq[45];
    int llist[250];
    struct deflate *deflate_array=malloc(sizeof(struct deflate));
    int deflate_size=0;
    uint8_t deflate_[100];
    memset(&freq[0], 0, sizeof(freq));
    memset(&llist[0], -1, sizeof(llist));
    memset(&deflate_[0], 0, sizeof(deflate_));
    array=deflate(kaynak_metin,metin_boyutu,&size,&location,freq);
    if(size<=1){
        break;
    }
    struct huffman_array *arr = huffman_array_new();
    for (i = 0; i < size; i++)
    {
        if (freq[i] > 0)
        {
            struct huffman *t = malloc(sizeof(struct huffman));
            struct lzss *t2 = malloc(sizeof(struct lzss));
            t2->pg=array[i].pg;
            t2->g=array[i].g;
            t->data=t2;
            t->freq = freq[i];
            t->left = NULL;
            t->right = NULL;
            huffman_array_add(arr, t);
        }
    }
    while (arr->size > 1)
    {
        huffman_array_sort(arr);
        struct huffman *t1 = huffman_array_pop(arr);
        struct huffman *t2 = huffman_array_pop(arr);
        struct huffman *t3 = malloc(sizeof(struct huffman));
        t3->left = t1;
        t3->right = t2;
        t3->freq = t1->freq + t2->freq;
        huffman_array_add(arr, t3);
    }

    char *buffer = malloc(256);
    int count=0;
    huffmantree_print(arr->items[0], llist, &deflate_size, buffer, 0);

    struct lzss *array_=malloc(sizeof(struct lzss));
    int deflate_i=0;
    int boyut=0;
    array_=base(kaynak_metin,metin_boyutu,&boyut,location,start,freq);
    int start_=start;
    start=location;
    deflate_array=realloc(deflate_array,metin_boyutu*sizeof(struct deflate));
    int deflate_llist[100];
    count=0;
    for(j=0;j<250;j++){
        if(llist[j]>-1){
            deflate_llist[count]=llist[j];
            count++;
        }
    }
    for(deflate_i=0;deflate_i<boyut;deflate_i++){
        for(j=0;j<size;j++){
            if(array_[deflate_i].pg==array[j].pg && array_[deflate_i].g==array[j].g){
                deflate_array[deflate_i].binary=deflate_llist[j];
                break;
            }
        }
    }
    for(j=0;j<size;j++){
        deflate_[j]=deflate_llist[j];
    }
    if ((file = fopen("deflate.txt", "a"))!=NULL)
    {
        fwrite(deflate_array,sizeof(struct deflate),boyut,file);
    }
    result=result+sizeof(struct deflate)*boyut;
    if ((file_ = fopen("key.txt", "a"))!=NULL)
    {
        fwrite(array,sizeof( struct lzss),size,file_);
        fwrite(deflate_,sizeof( uint8_t),size,file_);
        fwrite(&start_,sizeof( int),1,file_);
        fwrite(&location,sizeof( int),1,file_);
    }
    result_=result_+sizeof( struct lzss)*size+sizeof( uint8_t)*size+sizeof( int)+sizeof( int);

    }

    printf("Huffman deflate : %d\n",result);
    printf("Huffman key : %d",result_);

    fclose(f);
    fclose(file);
    fclose(file_);
    fclose(file__);

    return 0;
}
