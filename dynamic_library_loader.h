#ifndef dynamic_library_loader

#include <dlfcn.h>
#include <string.h>

/** Check the validity of the given string that whether it can be a valid number of type float
 * @param string
 * @return boolean type
 */

int valid_arg(char* str){
    int n = strlen(str);
    // checking validity of single digit string
    if(n==1 && !(str[0]>='0' && str[0]<='9'))
        return 0;
    
    // checking validity of first character
    if(str[0]!='+' && str[0]!='-' && str[0]!='.' && !(str[0]>='0' && str[0]<='9'))
        return 0;
    
    int flag = 0;
    for(int i=0; i<n; i++){

        // checking whether each char is valid
        if(str[i]!='.' && str[i]!='e' && str[i]!='+' && str[i]!='-' && !(str[i]>='0' && str[i]<='9'))
            return 0;
        
        if(str[i]=='e'){
            flag = 1;

            // if previous of e is not a number then invalid
            if(!(str[i-1]>='0' && str[i-1]<='9'))
                return 0;
            
            // if it is the last char then invalid
            if(i+1==n)
                return 0;
            
            // if it is not followed by +, - or digit then invalid
            if(str[i+1] != '+' && str[i+1]!='-' && !(str[i+1]>='0' && str[i+1]<='9'))
                return 0;
        }

        else if(str[i]=='.'){
            // if . comes after e then invalid
            if(flag)
                return 0;
            
            // if it is the last char then invalid
            if(i+1 > n)
                return 0;
            
            // if . not followed by a digit then invalid
            if(!(str[i+1]>='0' && str[i+1]<='9'))
                return 0;
        }
    }

    return 1;
}

/** chec whether the arguments provided by the user are supported and it yes then return its output
 * @param function_name
 * @param function_arguments
 * @param DL_pointer
 * @return string_output based on functions
 */ 
char* check_validity_and_run(char *func, char** arg, void* handle){
    char *error;
    char *buff = malloc(128);
    if(strcmp(func, "sin")==0){
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0]) || !valid_arg(arg[1])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
        if(!valid_arg(arg[0])){
            strcat(buff, "Invalid Arguments");
            return buff;
        }
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
    strcat(buff, "Unsupported dll name or function name");
    return buff;
}

/** accepts the DL along with other parameters. calls the above function  and return the final result or error message
 * @param Dl_name
 * @param function_name
 * @param function_arguments
 * @param flag (if set then it don't free the pointers)
 * @return string_output
 */ 
char* dll_func(char* dll, char *func, char **args, int flag) {
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
    if(!flag){
        free(dll);
        free(func);
        free(args);
    }
    // printf("Ans => %s\n", ans);
    return ans;
}
#define dynamic_library_loader
#endif