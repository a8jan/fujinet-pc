#include <unistd.h>  // for fsync

#include "fnFileTNFS.h"
#include "../../include/debug.h"


FileHandlerTNFS::FileHandlerTNFS(tnfsMountInfo *mountinfo, int handle)
{
    Debug_println("new FileHandlerTNFS");
    _mountinfo = mountinfo;
    _handle = handle;
};


FileHandlerTNFS::~FileHandlerTNFS()
{
    Debug_println("delete FileHandlerTNFS");
    close();
}


int FileHandlerTNFS::close()
{
    Debug_println("FileHandlerTNFS::close");
    int result = 0;
    if (_handle != -1) 
    {
        // TODO call tnfs_close

        _handle = -1;
        _mountinfo = nullptr;
    }
    return result;
}


int FileHandlerTNFS::seek(long int off, int whence)
{
    Debug_println("FileHandlerTNFS::seek");
    uint32_t new_pos;
    int result = tnfs_lseek(_mountinfo, _handle, off, whence, &new_pos);

    if(result != TNFS_RESULT_SUCCESS)
    {
        errno = tnfs_code_to_errno(result);
        return -1;
    }
    errno = 0;
    Debug_printf("\nnew pos is %u\n", new_pos);
    return 0;
}


long int FileHandlerTNFS::tell()
{
    Debug_println("FileHandlerTNFS::tell");
    uint32_t pos;
    tnfsFileHandleInfo *pFileInf = _mountinfo->get_filehandleinfo(_handle);
    if (pFileInf == nullptr) 
    {
        Debug_printf("\tbad handle\n");
        return -1;
    }
    pos = pFileInf->cached_pos;
    Debug_printf("\treturning %u\n", pos);
    return pos;
}


size_t FileHandlerTNFS::read(void *ptr, size_t size, size_t count)
{
    Debug_println("FileHandlerTNFS::read");
    // return fread(ptr, size, n, _fh);

    uint16_t bytes_requested = size * count;
    if (bytes_requested == 0)
        return 0;

    uint16_t bytes_read = 0;
    int result = tnfs_read(_mountinfo, _handle, (uint8_t *)ptr, bytes_requested, &bytes_read);

    if(result == TNFS_RESULT_SUCCESS || (result == TNFS_RESULT_END_OF_FILE && bytes_read > 0))
        errno = 0;
    else
        errno = tnfs_code_to_errno(result);

    return (size_t)(bytes_requested == bytes_read ? count : bytes_read / size);
}


size_t FileHandlerTNFS::write(const void *ptr, size_t size, size_t n)
{
    Debug_println("FileHandlerTNFS::write");
    // return fwrite(ptr, size, n, _fh);
    return 0;
}


int FileHandlerTNFS::flush()
{
    Debug_println("FileHandlerTNFS::flush");
    // int ret = fflush(_fh);    // This doesn't seem to be connected to anything in ESP-IDF VF, so it may not do anything
    // ret = fsync(fileno(_fh)); // Since we might get reset at any moment, go ahead and sync the file (not clear if fflush does this)
    // return ret;
    return -1;
}
