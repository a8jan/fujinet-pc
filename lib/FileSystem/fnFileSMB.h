#ifndef _FN_FILESMB_
#define _FN_FILESMB_

#include <stdint.h>
#include <cstddef>
#include <smb2/libsmb2.h>

#include "fnFile.h"


class FileHandlerSMB : public FileHandler
{
protected:
    struct smb2_context *_smb;
    struct smb2fh *_handle;
public:
    FileHandlerSMB(struct smb2_context *smb, struct smb2fh *handle);
    virtual ~FileHandlerSMB() override;

    virtual int close(bool destroy=true) override;
    virtual int seek(long int off, int whence) override;
    virtual long int tell() override;
    virtual size_t read(void *ptr, size_t size, size_t count) override;
    virtual size_t write(const void *ptr, size_t size, size_t count) override;
    virtual int flush() override;
};


#endif //_FN_FILESMB_
