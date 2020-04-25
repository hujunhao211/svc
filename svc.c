#include "svc.h"
#include <stdio.h>
#include <string.h>
char* get_file_name(int hash);
typedef struct commit{
    struct commit* prev;
    struct commit** parent;
    struct commit ** next;
    char* commit_id;
    char* message;
    int next_size;
    struct branch* branch_p;
    struct files** files_array;
    int file_length;
    char** addition;
    char** deletion;
    char** modification;
    int add_length;
    int rm_length;
    int mod_lenth;
}commit_t;
typedef struct helper{
    int commit_length;
//    commit_t** commit_array;
    commit_t* head;
    struct branch** branches;
    int branch_length;
} helper;
typedef struct branch{
    int length;
    int tag;
    commit_t** branch_commit;
    commit_t*precommit;
}branch;
typedef struct files{
    int hash_id;
    char* file_name;
}files_t;
int add_length;
int remove_length;
char** array_add;
char** array_remove;
void *svc_init(void) {
    // TODO: Implement
    helper* help = malloc(sizeof(helper));
    help->branch_length = 1;
    help->branches = malloc(sizeof(branch *));
    help->head = NULL;
    help->commit_length = 0;
    help->branches[0] = malloc(sizeof(branch));
    help->branches[0]->branch_commit = NULL;
    help->branches[0]->length = 0;
    help->branches[0]->tag = 0;
    help->branches[0]->precommit = NULL;
    return (void*)help;
}

void cleanup(void *helper) {
    int i;
    struct helper* help = (struct helper*)helper;
    for (i = 0; i < help->branch_length; i++){
        int j;
        for (j = 0; j < help->branches[i]->length; j++){
            free(help->branches[i]->branch_commit[j]->commit_id);
//            free(help->branches[i]->branch_commit[j]->branch_p);
//            int z;
//            for (z = 0; z < help->branches[i]->branch_commit[j]->file_length;z++){
//                free(help->branches[i]->branch_commit[j]->files_array[z]->file_name);
//                free(help->branches[i]->branch_commit[j]->files_array[z]);
//            }
//            free(help->branches[i]->branch_commit[j]->files_array);
//            free(help->branches[i]->branch_commit[j]->message);
//            free(help->branches[i]->branch_commit[j]->next);
//            for (z = 0; z < help->branches[i]->branch_commit[j]->add_length;z++){
//                free(help->branches[i]->branch_commit[j]->addition[z]);
//            }
//            free(help->branches[i]->branch_commit[j]->addition);
//            for (z = 0; z < help->branches[i]->branch_commit[j]->rm_length;z++){
//                free(help->branches[i]->branch_commit[j]->deletion[z]);
//            }
//            free(help->branches[i]->branch_commit[j]->deletion);
//            for (z = 0; z < help->branches[i]->branch_commit[j]->mod_lenth;z++){
//                free(help->branches[i]->branch_commit[j]->modification[z]);
//            }
//            free(help->branches[i]->branch_commit[j]->modification);
            if (help->branches[i]->branch_commit[j] != NULL){
                free(help->branches[i]->branch_commit[j]->parent[0]);
                free(help->branches[i]->branch_commit[j]->parent[1]);
            }
            free(help->branches[i]->branch_commit[j]);
        }
        free(help->branches[i]->branch_commit);
        free(help->branches[i]);
    }
    free(help->branches);
    free(help->head);
    free(help);
    // TODO: Implement
}
int do_count(FILE* f ,int hash){
    char *count = NULL;
    count = malloc(sizeof(char));
    int i = 0;
    while (fscanf(f, " %c",count)) {
        count = realloc(count, sizeof(char) * (++i));
    }
    for (int j = 0; j < i; j++){
        hash = (hash + count[j]) % 2000000000;
    }
    free(count);
    return hash;
}
int do_hash(char* str){
    int i;
    int result = 0;
    for (i = 0; i < (strlen(str)); i++){
        result = (result + str[i]) % 1000;
    }
    return result;
}
int hash_file(void *helper, char *file_path) {
    // TODO: Implement
    if (file_path == NULL){
        return -1;
    }
    FILE* file = fopen(file_path, "r");
    if (file == NULL){
        return -2;
    }
    int result = do_hash(file_path);
    result = do_count(file, result);
    return result;
}
int get_commit_id(struct commit* commit){
    int id = 0;
    id = do_hash(commit->message);
    return id;
}
int string_compare(const char* w1, const char* w2) {
    return strcmp(w1, w2);
}
int file_compare(const void* f1, const void* f2){
    const char* file1 = *((const char**)f1);
    const char* file2 = *((const char**)f2);
    return string_compare(file1, file2);
}
int detect_mod(struct files* f, char* path){
    int mod = 0;
    int id = hash_file(NULL, path);
    if (f->hash_id != id){
        mod = 1;
    }
    return mod;
}
int detect_add(char* file){
    int add = 0;
    int i;
    for (i = 0; i < add_length; i++){
        if (strcmp(file, array_add[i]) == 0){
            add = 1;
        }
    }
    return add;
}
int detect_del(char* file){
    int i;
    int del = 0;
    for (i = 0; i < remove_length; i++){
        if (strcmp(file, array_remove[i]) == 0){
            del = 1;
        }
    }
    return del;
}
int calculate_change(char* file_name,size_t size,int id){
    size_t i;
    for (i = 0; i < size; i++) {
        id = (id * (file_name[i] % 37))% 15485863 + 1;
    }
    return id;
}
void copy_file(char* name_1, char *  name_2){
    char c;
    FILE* file_1 = fopen(name_1, "r");
    FILE* file_2 = fopen(name_2, "w");
    while ((c = fgetc(file_1)) != EOF) {
        fputc(c, file_2);
    }
    fclose(file_1);
    fclose(file_2);
}
int find_mod(char* file_name, char** file_array,int size){
    int i;
    int find = 0;
    for (i = 0; i < size;i++){
        if (strcmp(file_name,file_array[i]) == 0){
            find = 1;
        }
    }
    return find;
}
int cal_commit(struct commit* commit){
    int hash;
    int i;
    int commit_id = 0;
    commit_id = get_commit_id(commit);
    if (commit->prev == NULL){
        if (add_length != 0){
            commit->addition = malloc(sizeof(char*));
            int size_file = 0;
            for (i = 0; i < add_length; i++){
                commit_id += 376591;
                commit_id = calculate_change(array_add[i], strlen(array_add[i]), commit_id);
                commit->addition = realloc(commit->addition,( ++size_file) * sizeof(char*));
                commit->addition[size_file - 1] = strdup(array_add[i]);
             }
            commit->add_length = size_file;
        }
    } else if (commit->file_length > 0){
        char** array = malloc(sizeof(char*) * (add_length + remove_length));
        int i;
        commit->addition = malloc(sizeof(char*));
        for (i = 0; i < add_length; i++){
            array[i] = array_add[i];
        }
        int j;
        for(j = 0; j < remove_length; j++){
            array[j + add_length] = array_remove[j];
        }
        int size = add_length + remove_length;
        for (i = 0; i < commit->file_length; i++){
            FILE* file = fopen(commit->files_array[i]->file_name, "r");
            if (file == NULL){
                array_remove = realloc(array_remove, ++remove_length * sizeof(char*));
                array_remove[remove_length - 1] = commit->files_array[i]->file_name;
            }
            fclose(file);
        }
        int mod_size = 0;
        char** mod_array = malloc(sizeof(char*));
        size = 0;
        commit->modification = malloc(sizeof(char*));
        for (i = 0; i < commit->prev->file_length; i++){
            if (!detect_add(commit->prev->files_array[i]->file_name) && !detect_add(commit->prev->files_array[i]->file_name)){
                if (detect_mod(commit->prev->files_array[i], commit->prev->files_array[i]->file_name)){
                    array = realloc(array, (++size) * sizeof(char*));
                    mod_array = realloc(mod_array, (++mod_size) * sizeof(char*));
                    array[size - 1] = commit->prev->files_array[i]->file_name;
                    mod_array[mod_size - 1] = commit->prev->files_array[i]->file_name;
                }
            }
        }
        int size_add = 0;
        int size_rm = 0;
        int size_mod = 0;
        qsort(array, size,sizeof(array[0]) ,file_compare);
        for(i = 0; i < size; i++){
//            int j;
            if (detect_add(array[i])){
                commit->addition = realloc(commit->addition, (++size_add) * sizeof(char *));
                commit->addition[size_add-1] = strdup(array[i]);
                commit_id += 376591;
                commit_id = calculate_change(array[i], strlen(array[i]), commit_id);
            }
            if (detect_del(array[i])){
                commit->deletion = realloc(commit->deletion, (++size_rm) * sizeof(char*));
                commit->deletion[size_rm - 1] = strdup(array[i]);
                commit_id += 85973;
                commit_id = calculate_change(array[i], strlen(array[i]), commit_id);
            }
            if (find_mod(array[i], mod_array, mod_size)){
                commit->modification =realloc(commit->modification, (++size_mod)*sizeof(char*));
                commit->modification[size_mod - 1] = strdup(array[i]);
                commit_id += 9573681;
            }
        }
        commit->add_length = size_add;
        commit->rm_length = size_rm;
        commit->mod_lenth = size_mod;
        free(mod_array);
        free(array);
    }  else {
        if (commit->prev->file_length > 0){
            int k;
            for (k = 0; k < commit->prev->file_length; k++){
                commit_id += 85973;
                hash = hash_file(NULL, commit->prev->files_array[k]->file_name);
                commit_id = calculate_change(get_file_name(hash), strlen(get_file_name(hash)), commit_id);
            }
        }
    }
    return commit_id;
}
char* convert_hexa(int decimalNumber){
    long int quotient;
    int i = 1,j,temp,size;
    char *hexadecimalNumber = malloc(sizeof(char));
    quotient = decimalNumber;
    while(quotient != 0) {
        temp = quotient % 16;
        //To convert integer into character
        if( temp < 10)
            temp =temp + 48;
        else
            temp = temp + 87;
        hexadecimalNumber[i - 1]= temp;
        hexadecimalNumber = realloc(hexadecimalNumber,++i);
        quotient = quotient / 16;
    }
    hexadecimalNumber[i - 1] = '\0';
    char* answer = malloc(sizeof(char) * i);
    size = i - 2;
    for (j = 0; j < i - 1; j++){
        answer[j] = hexadecimalNumber[size--];
    }
    answer[j] = '\0';
    free(hexadecimalNumber);
    return answer;
}
char* get_file_name(int hash){
//    printf("hash: %d\n",hash);
    char* array = malloc(sizeof(char));
    int value = 0;
    int index = 1;
    do{
        value = hash % 10;
        hash = hash / 10;
//        printf("char: %c\n",(char)(value + 48));
        array[index - 1] = (char)(value + 48);
//        printf("index: %d\n",index);
        index++;
        array = realloc(array, index);
    } while (hash != 0);
    array[index - 1] = '\0';
    char* name = malloc(sizeof(char) * index);
    int j;
    int temp = index - 2;
    for (j = 0; j < index - 1; j++){
        name[j] = array[temp--];
    }
    name[j] = '\0';
    free(array);
    return name;
}

char *svc_commit(void *helper, char *message) {
    // TODO: Implement
    struct helper* help = helper;
    if (message == NULL){
        return NULL;
    }
    if(help->head == NULL){
        struct helper* help = helper;
        if (help->head == NULL){
            struct helper* help = helper;
            if (help->head == NULL){
                help->branches[0]->branch_commit = malloc(sizeof(commit_t*));
                help->branches[0]->branch_commit[0]= malloc(sizeof(commit_t));
                help->branches[0]->length = 1;
                help->branches[0]->branch_commit[0]->prev = NULL;
                help->branches[0]->branch_commit[0]->next = NULL;
                help->branches[0]->branch_commit[0]->file_length = 0;
                help->branches[0]->branch_commit[0]->files_array = NULL;
                help->branches[0]->branch_commit[0]->parent = NULL;
            }
        }
    }
    return NULL;
}

void *get_commit(void *helper, char *commit_id) {
    // TODO: Implement
    return NULL;
}

char **get_prev_commits(void *helper, void *commit, int *n_prev) {
    // TODO: Implement
    return NULL;
}

void print_commit(void *helper, char *commit_id) {
    // TODO: Implement
}

int svc_branch(void *helper, char *branch_name) {
    // TODO: Implement
    return 0;
}

int svc_checkout(void *helper, char *branch_name) {
    // TODO: Implement
    return 0;
}

char **list_branches(void *helper, int *n_branches) {
    // TODO: Implement
    return NULL;
}

int svc_add(void *helper, char *file_name) {
    // TODO: Implement
//    struct helper* help = helper;
//    if (help->head == NULL){
//        struct helper* help = helper;
//        if (help->head == NULL){
//            help->branches[0]->branch_commit = malloc(sizeof(commit_t*));
//            help->branches[0]->branch_commit[0]= malloc(sizeof(commit_t));
//            help->branches[0]->length = 1;
//            help->branches[0]->branch_commit[0]->prev = NULL;
//            help->branches[0]->branch_commit[0]->next = NULL;
//        }
//    }
//    else if(help->commit_array[help->commit_length - 1]->message != NULL){
//
//    }
    return 0;
}

int svc_rm(void *helper, char *file_name) {
    // TODO: Implement
    return 0;
}

int svc_reset(void *helper, char *commit_id) {
    // TODO: Implement
    return 0;
}

char *svc_merge(void *helper, char *branch_name, struct resolution *resolutions, int n_resolutions) {
    // TODO: Implement
    return NULL;
}
