#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef __PC_BUILD_DEBUG__
#define DEBUG

#ifdef UNIT_TESTS
#undef DEBUG
#endif

#include <utils.h>
/*
  Debugging Macros
*/
    // Use standard printf
    #define Debug_print(...) util_debug_printf(nullptr, __VA_ARGS__)
    #define Debug_printf(...) util_debug_printf(__VA_ARGS__)
    #define Debug_println(...) util_debug_printf("%s\n", __VA_ARGS__)

    #define HEAP_CHECK(x) Debug_printf("HEAP CHECK %s " x "\n", true ? "PASSED":"FAILED")
#endif

#ifndef DEBUG
    #define Debug_print(...)
    #define Debug_printf(...)
    #define Debug_println(...)

    #define HEAP_CHECK(x)
#endif

#endif // _DEBUG_H_
