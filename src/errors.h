#ifndef RMF_ERRORS_H
#define RMF_ERRORS_H

#ifdef __cpusplus
extern "C"
{
#endif

#include <stdio.h>

#define handle_err(rescode, ctrlcode, errmsg) \
    {                                         \
        if ((int)rescode != (int)ctrlcode)    \
            raise_err(errmsg);                \
    }

#define handle_null(ptr, errmsg) \
    {                            \
        if ((void *)ptr == NULL) \
            raise_err(errmsg);   \
    }

#define raise_err(e)                                \
    {                                               \
        printf("\033[0;31m%s\033[0m\n", (char *)e); \
        exit(1);                                    \
    }

#ifdef __cpuslpus
}
#endif /* __cpuslpus */

#endif /* RMF_ERRORS_H */
