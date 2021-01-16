#ifndef _FN_FILELOCAL_
#define _FN_FILELOCAL_

#include <stdlib.h>
#include "fnFile.h"


class FileHandlerLocal : public FileHandler
{
protected:
    FILE *_fh = nullptr;

public:
    FileHandlerLocal(FILE *fh);
    virtual ~FileHandlerLocal() override;

    virtual int close() override;
    virtual int seek(long int off, int whence) override;
    virtual long int tell() override;
    virtual size_t read(void *ptr, size_t size, size_t n) override;
    virtual size_t write(const void *ptr, size_t size, size_t n) override;
    virtual int flush() override;
};


#endif //_FN_FILELOCAL_
