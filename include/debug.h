#ifndef _DEBUG_H_
#define _DEBUG_H_

// __PLATFORMIO_BUILD_DEBUG__ is set when build_type is set to debug in platformio.ini
#if defined(__PLATFORMIO_BUILD_DEBUG__) || defined(__PC_BUILD_DEBUG__) || defined(DBUG2)
#define DEBUG
#endif

#ifdef UNIT_TESTS
#undef DEBUG
#endif

#if defined(DEBUG) || !defined(NO_DEBUG_PRINT)
#include <utils.h>
/*
  Debugging Macros
*/
    #define Debug_print(...) util_debug_printf(nullptr, __VA_ARGS__)
    #define Debug_printf(...) util_debug_printf(__VA_ARGS__)
    #define Debug_println(...) util_debug_printf("%s\n", __VA_ARGS__)

    #define HEAP_CHECK(x) Debug_printf("HEAP CHECK %s " x "\n", true ? "PASSED":"FAILED")
#else
    #define Debug_print(...)
    #define Debug_printf(...)
    #define Debug_println(...)

    #define HEAP_CHECK(x)
#endif

#endif // _DEBUG_H_
