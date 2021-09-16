#ifndef resource_check

int get_curr_memory(int* Virtual_peak_mem){
    FILE* file = fopen("/proc/self/status", "r");
    char buffer[1024];
    if(file){
        while(fscanf(file, " %1023s", buffer)==1){
            if(strcmp(buffer, "VmPeak:")==0){
                fscanf(file, " %d", Virtual_peak_mem);
                break;
            }
        }
    }
    else
        return 0;

    fclose(file);
    return 1;
}

int get_curr_files(int *res){

}

#define resource_check
#endif