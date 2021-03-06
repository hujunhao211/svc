//
//  svcAsm.c
//  Test
//
//  Created by junhao hu on 2020/4/15.
//  Copyright © 2020 junhao hu. All rights reserved.
//

#include "svc.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
char* get_file_name(int hash);
int convert_dec(char* hexa);
typedef struct commit{
    struct commit* prev;
    struct commit** parent;
//    struct commit ** next;
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
    int commit_tag;
}commit_t;
typedef struct helper{
//    int commit_length;
//    commit_t** commit_array;
    commit_t* head;
    struct branch* branch_p;
    struct branch** branches;
    int branch_length;
    int file_length;
    int capacity;
    struct files **file_array;
    int add_length;
    int remove_length;
    struct files** array_add;
    char** array_remove;
    
} helper;
typedef struct branch{
    char* name;
    int length;
    int tag;
    commit_t** branch_commit;
    commit_t*precommit;
}branch;
typedef struct files{
    int old_hash;
    int hash_id;
    char* file_name;
}files_t;
void *svc_init(void) {
    // TODO: Implement
//    printf("svc_init\n");
    helper* help = malloc(sizeof(helper));
    help->branch_length = 1;
    help->branches = malloc(sizeof(branch *));
    help->head = NULL;
//    help->commit_length = 0;
    help->branches[0] = malloc(sizeof(branch));
    help->branches[0]->branch_commit = NULL;
    help->branches[0]->length = 0;
    help->branches[0]->tag = 0;
    help->branches[0]->precommit = NULL;
    help->branch_p = help->branches[0];
    help->file_array = malloc(sizeof(struct files*));
    help->file_length = 0;
    help->capacity = 1;
    help->branches[0]->name = strdup("master");
    help->add_length = 0;
    help->remove_length = 0;
    help->array_add = NULL;
    help->array_remove = NULL;
    return (void*)help;
}

void cleanup(void *helper) {
//    printf("clean up\n");
    int i;
    struct helper* help = (struct helper*)helper;
    for (i = 0; i < help->branch_length; i++){
        int j;
//        printf("length: %d\n",help->branches[i]->length);
        for (j = 0; j < help->branches[i]->length; j++){
            free(help->branches[i]->branch_commit[j]->commit_id);
//            free(help->branches[i]->branch_commit[j]->branch_p);
//            printf("commit id in the function3: \n");
            int z;
            for (z = 0; z < help->branches[i]->branch_commit[j]->file_length;z++){
                free(help->branches[i]->branch_commit[j]->files_array[z]->file_name);
                free(help->branches[i]->branch_commit[j]->files_array[z]);
            }
//            printf("commit id in the function\n");
            free(help->branches[i]->branch_commit[j]->files_array);
            free(help->branches[i]->branch_commit[j]->message);
//            free(help->branches[i]->branch_commit[j]->next);
            for (z = 0; z < help->branches[i]->branch_commit[j]->add_length;z++){
                free(help->branches[i]->branch_commit[j]->addition[z]);
            }
            free(help->branches[i]->branch_commit[j]->addition);
            for (z = 0; z < help->branches[i]->branch_commit[j]->rm_length;z++){
                free(help->branches[i]->branch_commit[j]->deletion[z]);
            }
            free(help->branches[i]->branch_commit[j]->deletion);
            for (z = 0; z < help->branches[i]->branch_commit[j]->mod_lenth;z++){
                free(help->branches[i]->branch_commit[j]->modification[z]);
            }
            free(help->branches[i]->branch_commit[j]->modification);
            free(help->branches[i]->branch_commit[j]->parent);
            free(help->branches[i]->branch_commit[j]);
        }
        free(help->branches[i]->branch_commit);
        free(help->branches[i]->name);
        free(help->branches[i]);
    }
    for (int k = 0; k < help->file_length; k++){
        free(help->file_array[k]->file_name);
        free(help->file_array[k]);
    }
    free(help->file_array);
    free(help->branches);
    for (i = 0; i < help->add_length; i++){
        free(help->array_add[i]->file_name);
        free(help->array_add[i]);
    }
    free(help->array_add);
    for (i = 0; i < help->remove_length; i++){
        free(help->array_remove[i]);
    }
    free(help->array_remove);
    help->add_length = 0;
    help->remove_length = 0;
    help->array_add = NULL;
    help->array_remove = NULL;
//    free(help->head);
    free(help);
    // TODO: Implement
}
int do_count(FILE* f ,int hash){
    char c = 0;
    while (fscanf(f, "%c",&c) != EOF) {
//        printf("%c",c);
        hash = (hash + (unsigned char)c) % 2000000000;
    }
    return hash;
}
int do_hash(char* str){
    int i;
    int result = 0;
    for (i = 0; i < (strlen(str)); i++){
        result = (result + (unsigned char)str[i]) % 1000;
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
//    printf("commit.message in fun%s\n",commit->message);
//    printf("commit_id: %d\n",id);
    return id;
}
int string_compare(const char* w1, const char* w2) {
    return strcmp(w1, w2);
}
int file_compare(const void* f1, const void* f2){
    const char* file1 = *((const char**)f1);
    const char* file2 = *((const char**)f2);
    char* s1 = strdup(file1);
    char* s2 = strdup(file2);
    int i = 0;
    char c;
    while (s1[i]) {
        c = s1[i];
        s1[i] = tolower(c);
        i++;
    }
    i = 0;
    while (s2[i]) {
        c = s2[i];
        s2[i] = tolower(c);
        i++;
    }
    int result = string_compare(s1, s2);
    free(s1);
    free(s2);
    return result;
}
int detect_mod(struct files* f, char* path){
    int mod = 0;
    int id = hash_file(NULL, path);
//    printf("file %d\n",f->hash_id);
//    printf("path %d\n",id);
//    printf("path %s\n",path);
//    printf("%s\n",f->file_name);
//    printf("%s\n",path);
//    printf("%d\n",f->hash_id);
//    printf("%d\n",id);
    if (f->hash_id != id){
        mod = 1;
    }
//    puts(path);
//    printf("mod is: %d\n",mod);
    return mod;
}
int detect_add(char* file,struct helper * help){
    int add = 0;
    int i;
    for (i = 0; i < help->add_length; i++){
        if (strcmp(file, help->array_add[i]->file_name) == 0){
            add = 1;
        }
    }
    return add;
}
int detect_del(char* file, struct helper* help){
    int i;
    int del = 0;
    for (i = 0; i < help->remove_length; i++){
        if (strcmp(file, help->array_remove[i]) == 0){
            del = 1;
        }
    }
    return del;
}
int calculate_change(char* file_name,size_t size,int id){
    size_t i;
    for (i = 0; i < size; i++) {
        id = (id * (((unsigned char)file_name[i]) % 37))% 15485863 + 1;
    }
    return id;
}
void copy_file(char* name_1, char *  name_2){
    unsigned int c;
    FILE* file_1 = fopen(name_1, "r");
    FILE* file_2 = fopen(name_2, "w");
    while ((c = fgetc(file_1)) != EOF) {
        fputc(c, file_2);
    }
//    printf("name1 :     %s\n",name_1);
//    printf("name2:       %s\n",name_2);
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

void allocate_file(struct commit* commit,struct helper* help){
    int i;
    commit->files_array = malloc(sizeof(struct files*));
    commit->file_length = 0;
    for (i = 0; i < help->add_length; i++){
        FILE* file = fopen(help->array_add[i]->file_name, "r");
        if (file != NULL){
            commit->files_array = realloc(commit->files_array, (++commit->file_length) * sizeof(struct files*));
            commit->files_array[commit->file_length - 1] = malloc(sizeof(struct files));
            commit->files_array[commit->file_length - 1]->file_name = strdup(help->array_add[i]->file_name);
            commit->files_array[commit->file_length - 1]->hash_id = hash_file(NULL, help->array_add[i]->file_name);
        }
    }
//    printf("here\n");
    struct commit* pre = commit->prev;
    if (pre != NULL){
        for(i = 0; i < pre->file_length; i++){
//            printf("here1\n");
            if (!detect_del(pre->files_array[i]->file_name,help)){
                FILE* file = fopen(pre->files_array[i]->file_name, "r");
                if (file != NULL){
                    commit->files_array = realloc(commit->files_array, (++commit->file_length) * sizeof(struct files*));
                    commit->files_array[commit->file_length - 1] = malloc(sizeof(struct files));
                    commit->files_array[commit->file_length - 1]->file_name = strdup(pre->files_array[i]->file_name);
                    commit->files_array[commit->file_length - 1]->hash_id = hash_file(NULL, pre->files_array[i]->file_name);
                }
            }
        }
    }
}
int file_struct_compare(const void* f1, const void* f2){
    const struct files* file1 = *((const struct files**)f1);
    const struct files* file2 = *((const struct files**)f2);
    char* s1 = strdup(file1->file_name);
    char* s2 = strdup(file2->file_name);
    int i = 0;
    char c;
    while (s1[i]) {
        c = s1[i];
        s1[i] = tolower(c);
        i++;
    }
    i = 0;
    while (s2[i]) {
        c = s2[i];
        s2[i] = tolower(c);
        i++;
    }
    int result = string_compare(s1, s2);
    free(s1);
    free(s2);
    return result;
}
int cal_commit(struct commit* commit,struct helper* help){
    allocate_file(commit,help);
    int i;
    int commit_id = 0;
    commit_id = get_commit_id(commit);
//    printf("message: %s\n",commit->message);
//    printf("commid %d\n",commit_id);
    if (commit->prev == NULL){
        qsort(help->array_add, help->add_length, sizeof(help->array_add[0]), file_struct_compare);
        if (help->add_length != 0){
            commit->addition = malloc(sizeof(char*));
            commit->deletion = malloc(sizeof(char*));
            commit->rm_length = 0;
            commit->modification = malloc(sizeof(char*));
            commit->mod_lenth = 0;
            int size_file = 0;
            for (i = 0; i < help->add_length; i++){
                FILE* file = fopen(help->array_add[i]->file_name, "r");
                if (file != NULL){
                    commit_id += 376591;
                    commit_id = calculate_change(help->array_add[i]->file_name, strlen(help->array_add[i]->file_name), commit_id);
                    commit->addition = realloc(commit->addition,( ++size_file) * sizeof(char*));
                    commit->addition[size_file - 1] = strdup(help->array_add[i]->file_name);
                }
             }
            commit->add_length = size_file;
        }
    } else if (commit->file_length > 0){
        char** array = malloc(sizeof(char*));
        int i;
        int size = 0;
        commit->addition = malloc(sizeof(char*));
        commit->deletion = malloc(sizeof(char*));
//        printf("add length: %d\n",add_length);
        for (i = 0; i < help->add_length; i++){
            FILE* file = fopen(help->array_add[i]->file_name, "r");
            if (file != NULL){
//                printf("fuck fuck fuck\n");
                array = realloc(array, (++size) * sizeof(char*));
                array[size - 1] = help->array_add[i]->file_name;
            }
        }
//        printf("after size: %d\n",size);
        int j;
        for(j = 0; j < help->remove_length; j++){
            array = realloc(array, (++size) * sizeof(char*));
            array[size - 1] = help->array_remove[j];
        }
        for (i = 0; i < commit->prev->file_length; i++){
            FILE* file = fopen(commit->prev->files_array[i]->file_name, "r");
            if (file == NULL){
                help->array_remove = realloc(help->array_remove, ++(help->remove_length) * sizeof(char*));
                help->array_remove[help->remove_length - 1] = strdup(commit->prev->files_array[i]->file_name);
                array = realloc(array, (++size)*sizeof(char*));
                array[size - 1] = commit->prev->files_array[i]->file_name;;
            }
//            fclose(file);
        }
        int mod_size = 0;
        char** mod_array = malloc(sizeof(char*));
        commit->modification = malloc(sizeof(char*));
//        printf("mod is in wrnnn nnnlnlnttpijt\n");
//        printf("before size = %d\n",size);
        for (i = 0; i < commit->prev->file_length; i++){
            if (!detect_add(commit->prev->files_array[i]->file_name,help) && !detect_del(commit->prev->files_array[i]->file_name,help)){
                if (detect_mod(commit->prev->files_array[i], commit->prev->files_array[i]->file_name)){
//                    printf("mod is in wrnnn nnnlnlnttpijt\n");
                    array = realloc(array, (++size) * sizeof(char*));
                    mod_array = realloc(mod_array, (++mod_size) * sizeof(char*));
                    array[size - 1] = commit->prev->files_array[i]->file_name;
                    mod_array[mod_size - 1] = commit->prev->files_array[i]->file_name;
                    for (j = 0; j < commit->file_length; j++){
                        if (strcmp(commit->prev->files_array[i]->file_name, commit->files_array[j]->file_name) == 0){
                            commit->files_array[j]->old_hash = commit->prev->files_array[i]->hash_id;
                        }
                    }
                }
            }
        }
        
        int size_add = 0;
        int size_rm = 0;
        int size_mod = 0;
        qsort(array, size,sizeof(array[0]) ,file_compare);
//        printf("size = %d\n",size);
        for(i = 0; i < size; i++){
//            printf("array[i] : %s\n",array[i]);
//            int j;
//            printf("jbjbjbjbjjbjjbbjjb\n");
            if (detect_add(array[i],help)){
                commit->addition = realloc(commit->addition, (++size_add) * sizeof(char *));
                commit->addition[size_add-1] = strdup(array[i]);
                commit_id += 376591;
                commit_id = calculate_change(array[i], strlen(array[i]), commit_id);
            }
            if (detect_del(array[i],help)){
//                printf("remove here %s\n",array[i]);
                commit->deletion = realloc(commit->deletion, (++size_rm) * sizeof(char*));
                commit->deletion[size_rm - 1] = strdup(array[i]);
                commit_id += 85973;
                commit_id = calculate_change(array[i], strlen(array[i]), commit_id);
            }
            if (find_mod(array[i], mod_array, mod_size)){
                commit->modification =realloc(commit->modification, (++size_mod)*sizeof(char*));
                commit->modification[size_mod - 1] = strdup(array[i]);
                commit_id += 9573681;
                commit_id = calculate_change(array[i], strlen(array[i]), commit_id);
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
                FILE* file = fopen(commit->prev->files_array[k]->file_name, "r");
                if (file != NULL){
                commit_id += 85973;
//                hash = hash_file(NULL, commit->prev->files_array[k]->file_name);
                char* get_name = commit->prev->files_array[k]->file_name;
                commit_id = calculate_change(get_name, strlen(get_name), commit_id);
                }
            }
            commit->add_length = 0;
            commit->rm_length = 0;
            commit->mod_lenth = 0;
            commit->addition = NULL;
            commit->deletion = NULL;
            commit->modification = NULL;
        }
    }
    for (i = 0; i < help->add_length; i++){
        free(help->array_add[i]->file_name);
        free(help->array_add[i]);
    }
    free(help->array_add);
    for (i = 0; i < help->remove_length; i++){
        free(help->array_remove[i]);
    }
    free(help->array_remove);
    help->add_length = 0;
    help->remove_length = 0;
    help->array_add = NULL;
    help->array_remove = NULL;
    return commit_id;
}
char* concat(char *s1, char *s2,char* s3)
{
//    printf("%d %d %d\n",strlen(s1),strlen(s2),strlen(s3));
    size_t value = strlen(s1) + strlen(s2) + strlen(s3) + 1;
//    printf("%zu\n",value);
    char *result = malloc(value); // +1 for the null-terminator
    result[0] = s1[0];
    for (int i = 0; i < strlen(s2); i++){
        result[i + 1] = s2[i];
    }
    for(int j = 0; j < strlen(s3); j++){
        result[strlen(s2) + 1 + j] = s3[j];
    }
    result[strlen(s1) + strlen(s2) +  + strlen(s3)] = '\0';
    return result;
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
char* con_hexa(int decimalNumber){
    char* array = malloc(20);
    sprintf(array,"%.6x", decimalNumber);
    return array;
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

int detect_change(struct commit* pre,struct helper* help){
    int i;
    int have_mod = 0;
    for (i = 0; i < pre->file_length; i++){
//        printf("%s\n",pre->files_array[i]->file_name);
        if (detect_mod(pre->files_array[i], pre->files_array[i]->file_name)){
            have_mod = 1;
        }
    }
//    printf("%d\n",remove_length);
//    printf("%d\n",remove_length);
    int remove_file = 0;
    for (i = 0; i < pre->file_length; i++){
        FILE* file = fopen(pre->files_array[i]->file_name, "r");
        if (file == NULL){
            remove_file = 1;
        }
    }
    int change = 0;
    for (i = 0; i < help->add_length; i++){
        FILE* f = fopen(help->array_add[i]->file_name, "r");
        if (f != NULL){
            change = 1;
        }
    }
    return (help->remove_length || have_mod || remove_file || change);
}
int detec_self(struct commit* commit){
    int i,change;
    change = 0;
    for (i = 0; i < commit->file_length; i++){
        if (detect_mod(commit->files_array[i], commit->files_array[i]->file_name)){
            change = 1;
        }
    }
    return change;
}
void update_help(struct helper* help){
    int i,index,find,iteration;
    for (iteration = 0; iteration < help->file_length; iteration++){
        index = 0;
        find = 0;
        for (i = 0; i < help->file_length; i++){
            FILE* file = fopen(help->file_array[i]->file_name, "r");
            if (file == NULL){
                find = 1;
                index = i;
                break;
            }
        }
        if (find){
            free(help->file_array[index]->file_name);
            free(help->file_array[index]);
            for (i = index; i < help->file_length - 1; i++){
                help->file_array[i] = help->file_array[i + 1];
            }
            help->file_array[help->file_length -1] = NULL;
            help->file_length--;
        }
    }
}
char *svc_commit(void *helper, char *message) {
//    printf("svc_commit %s\n",message);
    // TODO: Implement
    int i;
    struct helper* help = helper;
    if (message == NULL){
        return NULL;
    }
    for (i = 0; i < help->file_length; i++){
        help->file_array[i]->hash_id = hash_file(NULL, help->file_array[i]->file_name);
    }
    if (help->head == NULL){
        if (help->add_length != 0){
            struct helper* help = helper;
            help->branches[0]->branch_commit = malloc(sizeof(commit_t*));
            help->branches[0]->branch_commit[0]= malloc(sizeof(commit_t));
            help->branches[0]->length = 1;
            help->branches[0]->branch_commit[0]->next_size = 0;
            help->branches[0]->branch_commit[0]->prev = NULL;
//            help->branches[0]->branch_commit[0]->next = NULL;
            help->branches[0]->branch_commit[0]->file_length = 0;
            help->branches[0]->branch_commit[0]->message = strdup(message);
            help->branches[0]->branch_commit[0]->parent = NULL;
            help->branches[0]->branch_commit[0]->branch_p = help->branches[0];
            int commit_id = cal_commit(help->branches[0]->branch_commit[0],help);
            help->head = help->branches[0]->branch_commit[0];
            help->branches[0]->branch_commit[0]->commit_tag = 0;
            for (i = 0; i < help->file_length; i++){
                FILE* file = fopen(help->file_array[i]->file_name,"r");
                if (file != NULL){
                    fclose(file);
                    char* get_name = get_file_name(commit_id);
                    char* file_name = get_file_name(hash_file(NULL, help->file_array[i]->file_name));
                    char* free_file = concat("A",file_name , get_name);
                    copy_file(help->file_array[i]->file_name, free_file);
                    free(get_name);
                    free(file_name);
                    free(free_file);
                }
            }
            help->branches[0]->branch_commit[0]->commit_id = con_hexa(commit_id);
            help->branches[0]->precommit = NULL;
//            printf("commit id in the function: %s\n",help->branches[0]->branch_commit[0]->commit_id);
            update_help(help);
            return help->branches[0]->branch_commit[0]->commit_id;
        } else {
            return NULL;
        }
    } else if(help->branch_p->length == 0 || strcmp(help->branch_p->name,help->head->branch_p->name ) != 0){
//        printf("in here1 merge?\n");
        if (detect_change(help->branch_p->precommit,help)){
//            printf("in here1 merge\n");
            help->branch_p->branch_commit = realloc(help->branch_p->branch_commit, (++help->branch_p->length)* sizeof(struct commit*));
            help->branch_p->branch_commit[help->branch_p->length - 1] = malloc(sizeof(commit_t));
            help->branch_p->branch_commit[help->branch_p->length - 1]->commit_tag = 0;
            help->branch_p->branch_commit[help->branch_p->length - 1]->file_length = 0;
            help->branch_p->branch_commit[help->branch_p->length - 1]->prev = help->branch_p->precommit;
            help->branch_p->branch_commit[help->branch_p->length - 1]->parent = malloc(sizeof(struct commit*) * 2);
            help->branch_p->branch_commit[help->branch_p->length - 1]->message = strdup(message);
            help->branch_p->branch_commit[help->branch_p->length - 1]->parent[0] = help->branch_p->precommit;
            help->branch_p->branch_commit[help->branch_p->length - 1]->branch_p = help->branch_p;
            help->branch_p->branch_commit[help->branch_p->length - 1]->parent[1] = NULL;
            int commit_id = cal_commit(help->branch_p->branch_commit[help->branch_p->length - 1],help);
            help->branch_p->branch_commit[help->branch_p->length - 1]->commit_id = con_hexa(commit_id);
//            help->branch_p->branch_commit[help->branch_p->length - 1]->next_size = 0;
//            help->branch_p->branch_commit[help->branch_p->length - 1]->next = NULL;
            for (i = 0; i < help->file_length; i++){
                FILE* file = fopen(help->file_array[i]->file_name,"r");
                if (file != NULL){
                    fclose(file);
                    char* get_name = get_file_name(commit_id);
                    char* file_name = get_file_name(hash_file(NULL, help->file_array[i]->file_name));
                    char* free_file = concat("A", file_name, get_name);
                    copy_file(help->file_array[i]->file_name,free_file);
                    free(get_name);
                    free(file_name);
                    free(free_file);
                } else{
                }
            }
//            struct commit* pre = help->branch_p->precommit;
//            if (pre->next_size == 0){
//                pre->next = malloc(sizeof(struct commit*));
//                pre->next[0] = help->branch_p->branch_commit[help->branch_p->length - 1];
//            } else{
//                pre->next = realloc(pre->next, (sizeof(struct commit*)) * ++pre->next_size);
//                pre->next[pre->next_size - 1] = help->branch_p->branch_commit[help->branch_p->length - 1];
//            }
            
//            printf("?????????????????????   here    %d\n",help->branch_p->length);
            help->head = help->branch_p->branch_commit[0];
//            help->branch_p->branch_commit[help->branch_p->length - 1]->commit_id = con_hexa(commit_id);
//            printf("commit id in the function1: %s\n",help->branch_p->branch_commit[help->branch_p->length - 1]->commit_id);
//            help->branch_p->branch_commit[help->branch_p->length - 1]->commit_id = con_hexa(commit_id);
            update_help(help);
            return help->branch_p->branch_commit[help->branch_p->length - 1]->commit_id;
    }else {
        return NULL;
        }
    } else{
//        printf("in here2 merge?\n");
//        for(i = 0; i < help->head->file_length; i++){
//            FILE* f = fopen(help->head->files_array[i]->file_name,"r");
//            if (f != NULL){
//                printf("%s\n",help->head->files_array[i]->file_name);
//            }
//        }
//        for(i = 0 ; i < add_length; i++){
//            printf("add: %s\n",array_add[i]->file_name);
//        }
        if (detect_change(help->head,help)){
//            printf("in here2 merge\n");
            struct commit* commit = malloc(sizeof(struct commit));
            commit->prev = help->head;
            commit->file_length = 0;
//            if (help->head->next_size == 0){
//                help->head->next = malloc(sizeof(struct commit*));
//                help->head->next[0] = commit;
//            } else{
//                help->head->next = realloc(help->head->next, (sizeof(struct commit*)) * (++help->head->next_size));
//                help->head->next[help->head->next_size - 1] = commit;
//            }
            commit->message = strdup(message);
            help->head->branch_p->branch_commit = realloc(help->head->branch_p->branch_commit, (++help->head->branch_p->length)* sizeof(struct commit*));
            help->head->branch_p->branch_commit[help->head->branch_p->length - 1] = commit;
            commit->branch_p = help->head->branch_p;
            int commit_id = cal_commit(commit,help);
            commit->commit_id = con_hexa(commit_id);
//            commit->next_size = 0;
//            commit->next = NULL;
            help->head = commit;
            for (i = 0; i < help->file_length; i++){
                FILE* file = fopen(help->file_array[i]->file_name,"r");
                if (file != NULL){
                    fclose(file);
                    char* get_name = get_file_name(commit_id);
                    char* file_name = get_file_name(hash_file(NULL, help->file_array[i]->file_name));
                    char* free_file = concat("A", file_name, get_name);
                    copy_file(help->file_array[i]->file_name,free_file);
                    free(get_name);
                    free(file_name);
                    free(free_file);
                } else{
                }
            }
            commit->parent = malloc(sizeof(struct commit*) * 2);
            commit->parent[0] = commit->prev;
            commit->parent[1] = NULL;
            commit->commit_tag = 0;
//            printf("help %d\n",help->branch_length);
//            printf("help commit: %d\n",help->branches[help->branch_length-1]->length);
//            printf("commit id in the function2: %d\n",help->branches[0]->branch_commit[1] == NULL);
            update_help(help);
            return commit->commit_id;
        }
        return NULL;
    }
    return NULL;
}

void *get_commit(void *helper, char *commit_id) {
    // TODO: Implement
    int i,j;
    struct commit* com = NULL;
    struct helper* help = (struct helper*)helper;
    for (i = 0; i < help->branch_length; i++){
        for (j = 0; j < help->branches[i]->length; j++){
            if (strcmp(help->branches[i]->branch_commit[j]->commit_id, commit_id) == 0){
                com = help->branches[i]->branch_commit[j];
            }
        }
    }
    return com;
}

char **get_prev_commits(void *helper, void *commit, int *n_prev) {
    // TODO: Implement
    if (n_prev == NULL){
        return NULL;
    }
    if (commit == NULL){
        *(n_prev) = 0;
        return NULL;
    }
//    struct helper* help = (struct helper*)helper;
    struct commit* com = (struct commit*)commit;
    if (com->parent == NULL){
        *(n_prev) = 0;
        char** array = NULL;
//        array[0] = NULL;
        return array;
    } else if (com->parent[1] == NULL){
        *(n_prev) = 1;
        char** array = malloc(sizeof(char*));
        array[0] = com->parent[0]->commit_id;
        return array;
    } else{
        *(n_prev) = 2;
        char** array = malloc(sizeof(char*) * 2);
        array[0] = com->parent[0]->commit_id;
        array[1] = com->parent[1]->commit_id;
        return array;
    }
//    struct commit** array = malloc(sizeof(struct commit*));
}
void find_file_hash(struct commit* com,char* file_name ,int* hash, int* old){
    int i;
    for (i = 0; i < com->file_length; i++){
        if (strcmp(com->files_array[i]->file_name, file_name) == 0){
            *(hash) = com->files_array[i]->hash_id;
            *(old) = com->files_array[i]->old_hash;
        }
    }
}

void print_c(struct commit* com){
    printf("%s [%s]: %s\n",com->commit_id, com->branch_p->name,com->message);
    int i;
    for (i = 0; i < com->add_length; i++){
        printf("    + %s\n",com->addition[i]);
    }
    for (i = 0; i < com->rm_length; i++){
        printf("    - %s\n",com->deletion[i]);
    }
    int old = 0;
    int hash = 0;
    for(i = 0; i < com->mod_lenth;i++){
        find_file_hash(com, com->modification[i], &hash, &old);
        printf("    / %s [%d --> %d]",com->modification[i],old,hash);
    }
    printf("\n");
    printf("    Tracked files (%d):\n",com->file_length);
    for (i = 0; i < com->file_length; i++){
        printf("    [%*d] %s\n",10,com->files_array[i]->hash_id,com->files_array[i]->file_name);
    }
}
void print_commit(void *helper, char *commit_id) {
    // TODO: Implement
    if (commit_id == NULL){
        printf("Invalid commit id\n");
        return;
    }
    struct helper* help = (struct helper*)helper;
    int i,j;
    for (i = 0; i < help->branch_length; i++){
        for (j = 0; j < help->branches[i]->length; j++){
            if (strcmp(help->branches[i]->branch_commit[j]->commit_id, commit_id) == 0){
                print_c(help->branches[i]->branch_commit[j]);
                return;
            }
        }
    }
    printf("Invalid commit id\n");
}
int detect_no_change(struct commit* pre,struct helper* help){
    int i;
    int have_mod = 0;
    for (i = 0; i < pre->file_length; i++){
        if (detect_mod(pre->files_array[i], pre->files_array[i]->file_name)){
            have_mod = 1;
        }
    }
    int remove_file = 0;
    for (i = 0; i < pre->file_length; i++){
        FILE* file = fopen(pre->files_array[i]->file_name, "r");
        if (file == NULL){
            remove_file = 1;
        }
    }
    int change = 0;
    for (i = 0; i < help->add_length; i++){
        FILE* f = fopen(help->array_add[i]->file_name, "r");
        if (f != NULL){
            change = 1;
        }
    }
    return (!(help->remove_length || have_mod || remove_file || change));
}
int check_name(char* name){
    int i;
    int valid = 1;
    for(i = 0; i < strlen(name); i++){
        if ((name[i] <= 57 && name[i] >= 48) || (name[i] >= 65 && name[i] <= 90) || (name[i] >= 97 && name[i] <= 122) || (name[i] == '-') || (name[i] == '/') || (name[i] == '_')){
            
        } else {
            valid = 0;
        }
    }
//    printf("valid :    ????????       %d\n",valid);
    return valid;
}
int svc_branch(void *helper, char *branch_name) {
    // TODO: Implement
    struct helper* help = helper;
    if (branch_name == NULL || !check_name(branch_name)){
        return -1;
    }
//    printf("_______________________________no change\n");
    int i;
    for ( i = 0; i < help->branch_length; i++){
        if (strcmp(help->branches[i]->name, branch_name) == 0){
            return -2;
        }
    }
//    printf("create a new branch %s\n",branch_name);
    if (detect_no_change(help->head,help)){
//        printf("_______________________________no change\n");
        help->branches = realloc(help->branches, (++help->branch_length)*sizeof(struct branch*));
        help->branches[help->branch_length - 1] = malloc(sizeof(struct branch));
        help->branches[help->branch_length - 1]->length = 0;
        help->branches[help->branch_length - 1]->precommit = help->head;
        help->branches[help->branch_length - 1]->name = strdup(branch_name);
        help->branches[help->branch_length - 1]->branch_commit = NULL;
//        help->branches[help->branch_length - 1]->
        return 0;
    } else {
        return -3;
    }
    
    return 0;
}

int svc_checkout(void *helper, char *branch_name) {
    int find = 0;
    int i;
    if (branch_name == NULL){
        return -1;
    }
//    printf("check out %s\n",branch_name);
    int index = 0;
//    printf("here____________________________________Egrg%s\n",branch_name);
    struct helper* help = helper;
    for (i = 0; i < help->branch_length; i++){
        if(strcmp(help->branches[i]->name, branch_name) == 0){
            find = 1;
            index = i;
        }
    }
    if (!find){
        return -1;
    }
    if (detect_no_change(help->head,help)){
        help->branch_p = help->branches[index];
//        printf("here____________________________________Egrg\n");
    } else {
        return -2;
    }
    for (i = 0; i < help->file_length; i++){
        free(help->file_array[i]->file_name);
        free(help->file_array[i]);
    }
    help->file_length = 0;
    if (help->branches[index]->length == 0){
        for (i = 0; i < help->branches[index]->precommit->file_length; i++){
            help->file_array[help->file_length] = malloc(sizeof(struct files));
            help->file_array[help->file_length ++ ]->file_name = strdup(help->branches[index]->precommit->files_array[i]->file_name);
            help->file_array[help->file_length - 1]->hash_id = help->branches[index]->precommit->files_array[i]->hash_id;
        }
        help->head = help->branches[index]->precommit;
        struct commit* com = help->branches[index]->precommit;
        for(i = 0; i < com->file_length; i++){
            int value = convert_dec(com->commit_id);
            char* get_name = get_file_name(value);
            char* file_name = get_file_name(com->files_array[i]->hash_id);
            char* free_file = concat("A",file_name , get_name);
            copy_file(free_file, com->files_array[i]->file_name);
            //        printf("free_file :::::::::::::::  %s\n",free_file);
            //        printf("file_name :::::::::::::::  %s\n",file_name);
            free(get_name);
            free(file_name);
            free(free_file);
        }
    } else{
        struct commit* commit_head = help->branches[index]->branch_commit[help->branches[index]->length - 1];
        for (i = help->branches[index]->length - 1; i >= 0; i--){
            commit_head = help->branches[index]->branch_commit[i];
            if (commit_head->commit_tag == 0){
                break;
            } else {
                commit_head = help->branches[index]->branch_commit[i];
            }
        }
        for (i = 0; i < commit_head->file_length; i++){
            help->file_array[help->file_length] = malloc(sizeof(struct files));
            help->file_array[help->file_length ++]->file_name = strdup(commit_head->files_array[i]->file_name);
            help->file_array[help->file_length - 1]->hash_id = commit_head->files_array[i]->hash_id;
        }
//        struct commit* com = help->branches[index]->branch_commit[help->branches[index]->length - 1];
        for(i = 0; i < commit_head->file_length; i++){
            int value = convert_dec(commit_head->commit_id);
            char* get_name = get_file_name(value);
            char* file_name = get_file_name(commit_head->files_array[i]->hash_id);
            char* free_file = concat("A",file_name , get_name);
            copy_file(free_file, commit_head->files_array[i]->file_name);
            //        printf("free_file :::::::::::::::  %s\n",free_file);
            //        printf("file_name :::::::::::::::  %s\n",file_name);
            free(get_name);
            free(file_name);
            free(free_file);
        }
        help->head = commit_head;
    }
    // TODO: Implement
    return 0;
}

char **list_branches(void *helper, int *n_branches) {
    // TODO: Implement
//    printf("list_branches\n");
    int i;
    if (n_branches == NULL){
        return NULL;
    }
    struct helper* help = (struct helper*)helper;
    int size = 0;
    char** array = malloc(sizeof(char*));
    for (i = 0 ;i < help->branch_length; i++){
        printf("%s\n",help->branches[i]->name);
        array = realloc(array, sizeof(char*) * (++size));
        array[size - 1] = help->branches[i]->name;
    }
    *(n_branches) = size;
    return array;
}

int svc_add(void *helper, char *file_name) {
    // TODO: Implement
    if (file_name == NULL){
        return -1;
    }
    struct helper* help = helper;
    if (help->array_add == NULL){
        help->array_add = malloc(sizeof(struct files*));
    }
//    printf("add %s\n",file_name);
    int i;
    for(i = 0; i < help->file_length;i++){
        if (strcmp(file_name, help->file_array[i]->file_name) == 0 ){
            return -2;
        }
    }
    FILE* file = fopen(file_name, "r");
    if (file == NULL){
        return -3;
    }
    int find_remove = 0;
    int remove_index = 0;
    for (i = 0; i < help->remove_length; i++){
        if (strcmp(help->array_remove[i], file_name) == 0){
            find_remove = 1;
            remove_index = i;
        }
    }
    
    if (find_remove){
        char* filename = help->array_remove[remove_index];
        for (i = remove_index; i < help->remove_length - 1; i++){
            help->array_remove[i] = help->array_remove[i + 1];
        }
        free(filename);
        help->remove_length--;
    } else {
        if (help->file_length == help->capacity){
            help->file_array = realloc(help->file_array, help->capacity*2 * sizeof(char *));
            help->capacity *= 2;
        }
//        printf("%d\n",help->file_length);
//        printf("%d\n",help->capacity);
//        help->file_array[help->file_length++]->file_name = malloc(sizeof(char*));
//        printf("%p\n",help->file_array[help->file_length]);
//        help->file_array[help->file_length]->file_name;
//        help->file_array = realloc(help->file_array, sizeof(struct files*));
        help->file_array[help->file_length] = malloc(sizeof(struct files));
        help->file_array[help->file_length++]->file_name = strdup(file_name);
        help->file_array[help->file_length - 1]->hash_id = hash_file(NULL, file_name);
        
        
        help->array_add = realloc(help->array_add, sizeof(struct file*) * (++(help->add_length)));
        help->array_add[help->add_length - 1] = malloc(sizeof(struct files));
//        printf("lenth%d\n",add_length);
        help->array_add[help->add_length - 1]->file_name = strdup(file_name);
        help->array_add[help->add_length - 1]->hash_id = hash_file(NULL, file_name);
    }
//    else if(help->commit_array[help->commit_length - 1]->message != NULL){
//
//    }
    return hash_file(NULL, file_name);
}

int svc_rm(void *helper, char *file_name) {
    // TODO: Implement
    if (file_name == NULL){
        return -1;
    }
    struct helper* help = helper;
    if (help->array_remove == NULL){
        help->array_remove = malloc(sizeof(char*));
    }
//    printf("remove %s\n",file_name);
    int find = 0;
    int i,j,index = -1;
    int id = -2;
    for(i = 0; i < help->file_length;i++){
        if (strcmp(file_name, help->file_array[i]->file_name) == 0 ){
            index = i;
            id = help->file_array[i]->hash_id;
            find = 1;
        }
    }
    if (!find){
        return -2;
    }
    char* file_temp = help->file_array[index]->file_name;
    free(help->file_array[index]);
    for (j = index; j < help->file_length - 1; j++){
        help->file_array[j] = help->file_array[j + 1];
    }
    find = 0;
    help->file_array[j] = NULL;
    help->file_length--;
    for (i = 0; i < help->add_length; i++){
        if (strcmp(help->array_add[i]->file_name,file_name) == 0){
            index = i;
            find = 1;
        }
    }

    if (find){
        free(help->array_add[index]->file_name);
        free(help->array_add[index]);
        for (j = index; j < help->add_length - 1; j++){
            help->array_add[j] = help->array_add[j + 1];
        }
        help->array_add[j] = NULL;
        help->add_length--;
    }
    free(file_temp);
//    if (help->file_length == help->capacity){
//        help->file_array = realloc(help->file_array, help->capacity*2 * sizeof(char *));
//        help->capacity *= 2;
//    }
//    help->file_array[help->file_length] = malloc(sizeof(struct files));
//    help->file_array[help->file_length++]->file_name = strdup(file_name);
//    help->file_array[help->file_length - 1]->hash_id = hash_file(NULL, help->file_array[help->file_length - 1]->file_name);
    if (!find){
        help->array_remove = realloc(help->array_remove, (++(help->remove_length))*sizeof(char*));
        help->array_remove[help->remove_length - 1] = strdup(file_name);
    }
    return id;
}
int convert_dec(char* hexa){
    int value = 0;
    sscanf(hexa, "%x",&value);
    return value;
}
void recover_file(struct commit* com){
//    char* file =
    int i;
    for (i = 0; i < com->file_length; i++){
        int value = convert_dec(com->commit_id);
        char* get_name = get_file_name(value);
        char* file_name = get_file_name(com->files_array[i]->hash_id);
        char* free_file = concat("A",file_name , get_name);
        copy_file(free_file, com->files_array[i]->file_name);
//        printf("free_file :::::::::::::::  %s\n",free_file);
//        printf("file_name :::::::::::::::  %s\n",file_name);
        free(get_name);
        free(file_name);
        free(free_file);
    }
}
int svc_reset(void *helper, char *commit_id) {
    if (commit_id == NULL){
        return -1;
    }
//    printf("reset %s\n",commit_id);
    int find = 0;
    int index_branch = 0;
    int index_commit = 0;
    struct helper* help = (struct helper*)helper;
    int i,j;
    for(i = 0; i < help->branch_length;i++){
        for(j = 0; j < help->branches[i]->length; j++){
            if (help->branches[i]->branch_commit[j]->commit_tag == 0){
                if (strcmp(help->branches[i]->branch_commit[j]->commit_id, commit_id) == 0){
                    find = 1;
                    index_branch = i;
                    index_commit = j;
                }
            }
        }
    }
    if (!find){
        return -2;
    }
//    printf("reset?__________________________________\n");
    for (i = 0; i < help->file_length; i++){
        free(help->file_array[i]->file_name);
        free(help->file_array[i]);
    }
    help->file_length = 0;
    struct commit* com_p = help->branches[index_branch]->branch_commit[help->branches[index_branch]->length - 1];
    while (1) {
        if (strcmp(com_p->commit_id, help->branches[index_branch]->branch_commit[index_commit]->commit_id) == 0){
            break;
        }
        com_p->commit_tag = 1;
        com_p = com_p->prev;
    }
    for(i = 0; i < help->branches[index_branch]->branch_commit[index_commit]->file_length; i++){
        if (help->capacity == help->file_length){
            help->file_array = realloc(help->file_array, help->capacity*2 * sizeof(char *));
            help->capacity *= 2;
        }
        help->file_array[help->file_length] = malloc(sizeof(struct files));
        help->file_array[help->file_length++]->file_name = strdup(com_p->files_array[i]->file_name);
        help->file_array[help->file_length - 1]->hash_id = com_p->files_array[i]->hash_id;
    }
    for(i = 0; i < help->add_length; i++){
        free(help->array_add[i]->file_name);
        free(help->array_add[i]);
    }
    free(help->array_add);
    help->array_add = 0;
    for(i = 0; i < help->remove_length; i++){
        free(help->array_remove[i]);
    }
    free(help->array_remove);
    help->remove_length = 0;
    help->head = com_p;
    recover_file(com_p);
    // TODO: Implement
    return 0;
}
int detect_head_change(struct commit* head,struct helper* help){
//    int i,uncom;
//    uncom = 0;
//    for(i = 0 ; i < add_length; i++){
//        FILE* file = fopen(array_add[i]->file_name, "r");
//        if (file != NULL){
//            uncom = 1;
//        }
//    }
//    for(i = 0; i < head->file_length; i++){
//        if (detect_mod(head->files_array[i], head->files_array[i]->file_name)){
//            uncom = 1;
//        }
//    }
    return detect_change(head,help);
//    return uncom;
}
//int find_file(char* str, struct files** file_array, int f_size){
//    int find = 0;
//    int i;
//    for (i = 0; i < f_size; i++) {
//        if (strcmp(str, file_array[i]->file_name) == 0){
//            find = 1;
//        }
//    }
//    return find;
//}
int check_remove(char* str, char** remove,int r_size){
    int i;
    int find = 0;
    for (i = 0; i < r_size; i++) {
        if (strcmp(remove[i], str) == 0) {
            find = 1;
        }
    }
    return find;
}
char* get_mess(char *branch_name){
    char* pre = "Merged branch ";
    char* mess = malloc(strlen(pre) + strlen(branch_name) + 1);
    strcpy(mess, pre);
    strcat(mess, branch_name);
    return mess;
}
int svc_add_copy(void *helper, char *file_name) {
    // TODO: Implement
    if (file_name == NULL){
        return -1;
    }
    struct helper* help = helper;
    if (help->array_add == NULL){
        help->array_add = malloc(sizeof(struct files *));
    }
//    printf("add %s\n",file_name);
    int i;
    for(i = 0; i < help->file_length;i++){
        if (strcmp(file_name, help->file_array[i]->file_name) == 0 ){
            return -2;
        }
    }
    FILE* file = fopen(file_name, "r");
    if (file == NULL){
        return -3;
    }
    int find_remove = 0;
    int remove_index = 0;
    for (i = 0; i < help->remove_length; i++){
        if (strcmp(help->array_remove[i], file_name) == 0){
            find_remove = 1;
            remove_index = i;
        }
    }
    
    if (find_remove){
        char* filename = help->array_remove[remove_index];
        for (i = remove_index; i < help->remove_length - 1; i++){
            help->array_remove[i] = help->array_remove[i + 1];
        }
        free(filename);
        help->remove_length--;
    } else {
        if (help->file_length == help->capacity){
            help->file_array = realloc(help->file_array, help->capacity*2 * sizeof(char *));
            help->capacity *= 2;
        }
//        printf("%d\n",help->file_length);
//        printf("%d\n",help->capacity);
//        help->file_array[help->file_length++]->file_name = malloc(sizeof(char*));
//        printf("%p\n",help->file_array[help->file_length]);
//        help->file_array[help->file_length]->file_name;
//        help->file_array = realloc(help->file_array, sizeof(struct files*));
        help->file_array[help->file_length] = malloc(sizeof(struct files));
        help->file_array[help->file_length++]->file_name = strdup(file_name);
        help->file_array[help->file_length - 1]->hash_id = hash_file(NULL, file_name);
        
        
        help->array_add = realloc(help->array_add, sizeof(struct file*) * (++(help->add_length)));
        help->array_add[help->add_length - 1] = malloc(sizeof(struct files));
//        printf("lenth%d\n",add_length);
        help->array_add[help->add_length - 1]->file_name = strdup(file_name);
        help->array_add[help->add_length - 1]->hash_id = hash_file(NULL, file_name);
    }
//    else if(help->commit_array[help->commit_length - 1]->message != NULL){
//
//    }
    return hash_file(NULL, file_name);
}

char *svc_merge(void *helper, char *branch_name, struct resolution *resolutions, int n_resolutions) {
    // TODO: Implement
    int i,find;
    struct helper* help = (struct helper*)helper;
    if (branch_name == NULL){
        printf("Invalid branch name\n");
        return NULL;
    }
    if(strcmp(help->branch_p->name, branch_name) == 0){
        printf("Cannot merge a branch with itself\n");
        return NULL;
    }
    find = 0;
    int index = 0;
    for (i = 0; i < help->branch_length; i++){
        if (strcmp(help->branches[i]->name, branch_name) == 0){
            find = 1;
            index = i;
        }
    }
    if (!find){
        printf("Branch not found\n");
        return NULL;
    }
    if(help->head->prev == NULL){
        if(detect_head_change(help->head,help)){
            printf("Changes must be committed\n");
            return NULL;
        }
    } else if(detect_change(help->head->prev,help)){
        printf("Changes must be committed\n");
        return NULL;
    }
    
//    printf("merge %s %s\n",help->branch_p->name,branch_name);
//    printf("indexd: %d igonore\n",index);
////    struct files** array = malloc(sizeof(struct files*));
////    int size = 0;
    struct commit* com_p = help->branches[index]->branch_commit[help->branches[index]->length - 1];;
    for (i = help->branches[index]->length - 1; i >= 0 ; i--){
        com_p = help->branches[index]->branch_commit[i];
        if (com_p->commit_tag == 0){
            break;
        }
        com_p = help->branches[index]->branch_commit[i];
    }
    int j;
    if(n_resolutions > 0){
        for (i = 0; i < help->head->file_length; i++){
            for(j = 0; j < n_resolutions; j++){
                if(strcmp(help->head->files_array[i]->file_name, resolutions[j].file_name) == 0){
                    if (resolutions[j].resolved_file == NULL){
                        svc_rm(help, resolutions[j].file_name);
                    } else{
                        copy_file(resolutions[j].resolved_file, resolutions[j].file_name);
                    }
                } else {
//                    svc_add(helper,help->head->files_array[i]->file_name);
                }
            }
        }
        if (com_p->commit_tag == 0){

        }
        for (i = 0; i < com_p->file_length; i++) {
            for (j = 0; j < n_resolutions; j++) {
                if(strcmp(com_p->files_array[i]->file_name, resolutions[j].file_name) == 0){
                    if (resolutions[j].resolved_file == NULL){
                        if (!check_remove(resolutions[j].file_name, help->array_remove, help->remove_length)){
                            svc_rm(help, resolutions[j].file_name);
                        }
                    } else{
                        copy_file(resolutions[j].resolved_file, resolutions[j].file_name);
                    }
                } else {
                    svc_add_copy(helper,com_p->files_array[i]->file_name);
                }
            }
        }
    } else {
//        for (i = 0; i < help->head->file_length; i++){
//            svc_add(helper, help->head->files_array[i]->file_name);
//        }
        for (i = 0; i < com_p->file_length; i++){
            svc_add_copy(helper, com_p->files_array[i]->file_name);
        }
    }

//    file_res->add_length = 0;
//    file_res->rm_length = 0;
//    file_res->mod_lenth = 0;
//    file_res->addition = malloc(sizeof(char*));
//    file_res->deletion = malloc(sizeof(char*));
//    file_res->modification = malloc(sizeof(char*));
//    file_res->file_length = 0;
//    file_res->files_array = malloc(sizeof(struct files*));
//    file_res->branch_p = help->branch_p;
    char* name = get_mess(branch_name);
    svc_commit(helper, name);
    free(name);
    help->head->parent[1] = com_p;
    printf("Merge successful\n");
    return help->head->commit_id;
//    return NULL;
}


