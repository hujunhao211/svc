#include <assert.h>
#include "svc.h"
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char **argv) {
    void* helper = svc_init();
    svc_add(helper, "a.txt");
    char* start = svc_commit(helper, "Initial commit");
    svc_add(helper,"b.txt");
    svc_add(helper,"c.a");
    FILE* file = fopen("a.txt","w");
    fprintf(file,"this is changed a file, it has been changed\n");
    svc_commit(helper,"Changed a.txt content");
    svc_branch(helper,"features/brand-new-feature1");
    int number = 0;
    scanf("%d",&number);
    svc_add(helper,"d.out");
    svc_commit(helper,"Added a compiled program");
    svc_add(helper,"view.sh");
    svc_commit(helper,"Created icon");
    svc_checkout(helper,"features/brand-new-feature1");
    svc_rm(helper,"b.txt");
    svc_commit(helper,"Removed a useless file");
    svc_branch(helper,"fixes/emergency-fix");
    svc_checkout(helper,"feature/config");
    svc_add(helper,"e.pdf");
    svc_commit(helper,"Copied some random pdf from somewhere");
    svc_checkout(helper,"fixes/emergency-fix");
    svc_add(helper,"resolve file");
    svc_commit(helper,"This somehow made everything work fine");
    svc_checkout(helper,"master");
    struct resolution re = {0};
    re.file_name = "view.sh";
    re.resolved_file = "temp.txt";
    struct resolution res[1];
    res[0] = re;
    int n = 1;
    svc_merge(helper,"features/brand-new-feature1",res,n);
    cleanup(helper);
    
    return 0;
}

