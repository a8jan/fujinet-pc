#ifndef COMPAT_GETTIMEOFDAY_H
#define COMPAT_GETTIMEOFDAY_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

int compat_gettimeofday(struct timeval* tp, struct timezone* tzp);

// void test_gettimeofday(int iter);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_GETTIMEOFDAY_H