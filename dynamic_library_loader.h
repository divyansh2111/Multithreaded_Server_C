#ifndef dynamic_library_loader

#include <dlfcn.h>

char* check_validity_and_run(char *func, char** arg, void* handle){
    char *error;
    char *buff = malloc(64);
    if(strcmp(func, "sin")==0){
        double (*sin)(double);
        sin = dlsym(handle, "sin");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*sin)(atof(arg[0])), 5, buff);
        return buff;
    }
    else if(strcmp(func, "cos")==0){
        double (*cos)(double);
        cos = dlsym(handle, "cos");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*cos)(atof(arg[0])), 5, buff);
        return buff;
    }
    else if( strcmp(func, "tan")==0){
        double (*tan)(double);
        tan = dlsym(handle, "tan");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*tan)(atof(arg[0])), 5, buff);
        return buff;
    }  
    else if(strcmp(func, "pow")==0){
        double (*pow)(double, double);
        pow = dlsym(handle, "pow");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        // printf("%s %s\n", arg[0], arg[1]);
        // printf("RESULT => %f\n", (*pow)(atof(arg[0]), atof(arg[1])));
        gcvt((*pow)(atof(arg[0]), atof(arg[1])), 5, buff);
        return buff;
    }
    else if(strcmp(func, "sinh")==0){
        double (*sinh)(double);
        sinh = dlsym(handle, "sinh");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*sinh)(atof(arg[0])), 5, buff);
        return buff;
    }
    else if( strcmp(func, "cosh")==0){
        double (*cosh)(double);
        cosh = dlsym(handle, "cosh");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*cosh)(atof(arg[0])), 5, buff);
        return buff;
    }
    else if(strcmp(func, "tanh")==0){
        double (*tanh)(double);
        tanh = dlsym(handle, "tanh");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*tanh)(atof(arg[0])), 5, buff);
        return buff;
    }
    else if(strcmp(func, "log")==0){
        double (*log)(double);
        log = dlsym(handle, "log");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*log)(atof(arg[0])), 5, buff);
        return buff;
    }
    else if(strcmp(func, "exp")==0){
        double (*exp)(double);
        exp = dlsym(handle, "exp");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*exp)(atof(arg[0])), 5, buff);
        return buff;
    }
    else if(strcmp(func, "sqrt")==0){
        double (*sqrt)(double);
        sqrt = dlsym(handle, "sqrt");
        if ((error = dlerror()) != NULL)  {
            strcat(buff, "Error while loading function from DLL");
            return buff;
        }
        gcvt((*sqrt)(atof(arg[0])), 5, buff);
        return buff;
    }
    
    memset(buff, 0, sizeof(buff));
    strcat(buff, "Incorrect dll name or function name");
    return buff;
}

char* dll_func(char* dll, char *func, char **args) {
    void *handle;

    char dll_name[50] = "/lib/x86_64-linux-gnu/libm.so.6";
    if(strcmp(dll_name, dll)!=0)
        return "DLL not supported\0";

    handle = dlopen (dll, RTLD_LAZY);
    if (!handle) 
        return "Failed to load DLL\0";

    // printf("%s\n", func);
    char *ans = check_validity_and_run(func, args, handle);
    dlclose(handle);
    free(dll);
    free(func);
    free(args);
    // printf("Ans => %s\n", ans);
    return ans;
}
#define dynamic_library_loader
#endif