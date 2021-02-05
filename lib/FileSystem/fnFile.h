#ifndef _FN_FILE_
#define _FN_FILE_

#include <stdio.h>


/* 
FileHandler - stdlib's FILE abstraction to allow implement other file protocols in application (no need for kernel/FUSE drivers)
*/


class FileHandler
{

public:
    virtual ~FileHandler() {};

    virtual int close(bool destroy=true) = 0;
    virtual int seek(long int off, int whence) = 0;
    virtual long int tell() = 0;
    virtual size_t read(void *ptr, size_t size, size_t n) = 0;
    virtual size_t write(const void *ptr, size_t size, size_t n) = 0;
    virtual int flush() = 0;
};


#endif //_FN_FILE_
