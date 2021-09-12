#ifndef dynamic_library_loader

#include <dlfcn.h>
float dll_func() {
    void *handle;
    double (*cosine)(double);
    char *error;

    handle = dlopen ("/lib/x86_64-linux-gnu/libm.so.6", RTLD_LAZY);
    if (!handle) {
        return -12;
    }

    cosine = dlsym(handle, "cos");
    if ((error = dlerror()) != NULL)  {
        return -11;
    }

    // printf ("%f\n", (*cosine)(2.0));
    // dlclose(handle);
    return (*cosine)(2.0);
}
#define dynamic_library_loader
#endif