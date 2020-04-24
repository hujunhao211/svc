#include "svc.h"
#include <stdio.h>
#include <string.h>
typedef struct commit{
    struct commit* prev;
    struct commit ** next;
    char* commit_id;
    char* message;
    int next_size;
    struct branch* branch_p;
    struct files** files_array;
    int file_length;
}commit_t;
typedef struct helper{
    int commit_length;
    commit_t** commit_array;
    commit_t* head;
    struct branch** branches;
    int branch_length;
} helper;
typedef struct branch{
    int length;
    int tag;
    commit_t** branch_commit;
}branch;
typedef struct files{
    int hash_id;
    char* file_name;
}files_t;
void *svc_init(void) {
    // TODO: Implement
    helper* help = malloc(sizeof(helper));
//    commit_t* commit = malloc(sizeof(commit_t));
//    commit->prev = NULL;
//    commit->commit_id = NULL;
//    commit->message = NULL;
    help->branch_length = 1;
    help->branches = malloc(sizeof(branch *));
    help->head = NULL;
    help->commit_array = NULL;
    help->commit_length = 0;
    help->branches[0] = malloc(sizeof(branch));
    help->branches[0]->branch_commit = NULL;
    help->branches[0]->length = 0;
    help->branches[0]->tag = 0;
    return (void*)help;
}

void cleanup(void *helper) {
    int i;
    struct helper* help = (struct helper*)helper;
    for (i = 0; i < help->branch_length; i++){
        int j;
        for (j = 0; j < help->branches[i]->length; j++){
            free(help->branches[i]->branch_commit[j]->commit_id);
            free(help->branches[i]->branch_commit[j]->branch_p);
            int z;
            for (z = 0; z < help->branches[i]->branch_commit[j]->file_length;z++){
                free(help->branches[i]->branch_commit[j]->files_array[z]->file_name);
                free(help->branches[i]->branch_commit[j]->files_array[z]);
            }
            free(help->branches[i]->branch_commit[j]->files_array);
            free(help->branches[i]->branch_commit[j]->message);
            free(help->branches[i]->branch_commit[j]->next);
            free(help->branches[i]->branch_commit[j]);
        }
        free(help->branches[i]->branch_commit);
        free(help->branches[i]);
    }
    free(help->branches);
    free(help->head);
    free(help->commit_array);
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
char* get_commit_id(struct commit* commit){
    int id = 0;
    id = do_hash(commit->message);
    char* commit_id = NULL;
    
    return commit_id;
}
int string_compare(const char* w1, const char* w2) {
    return strcmp(w1, w2);
}
int file_compare(const void* f1, const void* f2){
    const files_t* file1 = *((const files_t**)f1);
    const files_t* file2 = *((const files_t**)f2);
    return string_compare(file1->file_name, file2->file_name);
}
int detect_mod(FILE* f1, FILE* f2){
    int same = 1;
    char array_f1[255];
    char array_f2[255];
    while (fgets(array_f1, 254, f1)) {
        fgets(array_f2, 254, f2);
        if (strcmp(array_f1, array_f2) != 0){
            same = 0;
        }
    }
    return same;
}
int cal_commit(struct commit* commit){
    int i;
    if (commit->prev != NULL&&commit->prev->file_length > 0){
        qsort(commit->prev->files_array, commit->prev->file_length,sizeof(commit->prev->files_array[0]) ,file_compare);
    }
    return 0;
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

