#ifndef UTSNAME_H
#define UTSNAME_H

#if defined(_WIN32)

#define UTSNAME_MAXLENGTH 256

struct utsname {
	char sysname [UTSNAME_MAXLENGTH]; // name of this implementation of the operating system
	char nodename[UTSNAME_MAXLENGTH]; // name of this node within an implementation - dependent communications network
	char release [UTSNAME_MAXLENGTH]; //  current release level of this implementation
	char version [UTSNAME_MAXLENGTH]; //  current version level of this release
	char machine [UTSNAME_MAXLENGTH]; //  name of the hardware type on which the system is running
};

int uname(struct utsname *name);

#else

#include <sys/utsname.h>

#endif

#endif // UTSNAME_H