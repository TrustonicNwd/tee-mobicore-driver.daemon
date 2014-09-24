#ifndef WRAPPER_H_
#define WRAPPER_H_

#ifdef WIN32
    #ifdef __cplusplus
    extern "C" {
    #endif
    #include <stdint.h>
    #include <inttypes.h>

	#define snprintf _snprintf

    typedef int pid_t;

    #define bool int
    #define TRUE 1
    #define FALSE 0

    #define false FALSE
    #define true TRUE

    #define sleep(x) Sleep(x)

    #ifdef __cplusplus
    }
    #endif // __cplusplus
#else
	#include <stdbool.h>
#endif // WINDOWS_BUILD
#endif //WRAPPER_H_
