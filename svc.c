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
    char c = 0;
//    printf("%d\n",hash);
    while ((c = fgetc(f)) != EOF) {
        hash = (hash + (uint8_t)c) % 2000000000;
    }
    return hash;
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
    int i;
    int result = 0;
    for (i = 0; i < (strlen(file_path)); i++){
        result = (result + (uint8_t)file_path[i]) % 1000;
    }
    result = do_count(file, result);
//    printf("%d\n",result);
    return result;
}
char *svc_commit(void *helper, char *message) {
    // TODO: Implement
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

