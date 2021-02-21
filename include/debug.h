#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef __OPEN_BUILD_DEBUG__
#define DEBUG

#ifdef UNIT_TESTS
#undef DEBUG
#endif

// #include "../lib/hardware/fnUART.h"

#include <cstdio>
/*
  Debugging Macros
*/
    // Use standard printf
    #define Debug_print(...) {printf( "%s", __VA_ARGS__ ); fflush( stdout );}
    #define Debug_printf(...) {printf( __VA_ARGS__ ); fflush( stdout );}
    #define Debug_println(...) {printf( "%s\n", __VA_ARGS__ ); fflush( stdout );}

    #define HEAP_CHECK(x) Debug_printf("HEAP CHECK %s " x "\n", true ? "PASSED":"FAILED")
#endif

#ifndef DEBUG
    #define Debug_print(...)
    #define Debug_printf(...)
    #define Debug_println(...)

    #define HEAP_CHECK(x)
#endif

#endif // _DEBUG_H_
