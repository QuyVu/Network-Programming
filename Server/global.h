#ifndef GLOBAL_H
#define	GLOBAL_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>

#define TEXT_NORMAL      "\x1B[0m"
#define TEXT_ERROR       "\x1B[31m"
#define TEXT_SUCCESS     "\x1B[32m"
#define TEXT_WARNING     "\x1B[33m"
#define TEXT_INFO        "\x1B[34m"

    typedef int BOOLEAN;
#define TRUE        1
#define FALSE       0

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_H */

