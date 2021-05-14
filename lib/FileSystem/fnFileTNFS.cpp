#include <unistd.h>  // for fsync
#include <errno.h>

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
    if (_handle != -1) close(false);
}


int FileHandlerTNFS::close(bool destroy)
{
    Debug_println("FileHandlerTNFS::close");
    int result = 0;
    if (_handle != -1) 
    {
        result = tnfs_close(_mountinfo, _handle);
        _handle = -1;
        _mountinfo = nullptr;
    }
    if (destroy) delete this;
    return result;
}


int FileHandlerTNFS::seek(long int off, int whence)
{
    Debug_println("FileHandlerTNFS::seek");
    uint32_t new_pos;
    int result = tnfs_lseek(_mountinfo, _handle, off, whence, &new_pos);

    if (result == TNFS_RESULT_BAD_FILENUM && _bad_fd_recovery() == TNFS_RESULT_SUCCESS)
    {
        // retry seek command
        result = tnfs_lseek(_mountinfo, _handle, off, whence, &new_pos);
    }

    if (result != TNFS_RESULT_SUCCESS)
    {
        errno = tnfs_code_to_errno(result);
        return -1;
    }
    errno = 0;
    Debug_printf("new pos is %u\n", new_pos);
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
    Debug_printf("returning %u\n", pos);
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
    if (result == TNFS_RESULT_BAD_FILENUM && _bad_fd_recovery() == TNFS_RESULT_SUCCESS)
    {
        // retry read command
        result = tnfs_read(_mountinfo, _handle, (uint8_t *)ptr, bytes_requested, &bytes_read);
    }
    errno = (result == TNFS_RESULT_SUCCESS || (result == TNFS_RESULT_END_OF_FILE && bytes_read > 0)) ? 0 : tnfs_code_to_errno(result);

    return (size_t)(bytes_requested == bytes_read ? count : bytes_read / size);
}


size_t FileHandlerTNFS::write(const void *ptr, size_t size, size_t count)
{
    Debug_println("FileHandlerTNFS::write");
    // return fwrite(ptr, size, n, _fh);

    uint16_t bytes_requested = size * count;
    if (bytes_requested == 0)
        return 0;

    uint16_t bytes_written;
    int result = tnfs_write(_mountinfo, _handle, (uint8_t *)ptr, bytes_requested, &bytes_written);
    errno = (result == TNFS_RESULT_SUCCESS) ? 0 : tnfs_code_to_errno(result);

    return (size_t)(bytes_requested == bytes_written ? count : bytes_written / size);
}


int FileHandlerTNFS::flush()
{
    Debug_println("FileHandlerTNFS::flush");
    // int ret = fflush(_fh);    // This doesn't seem to be connected to anything in ESP-IDF VF, so it may not do anything
    // ret = fsync(fileno(_fh)); // Since we might get reset at any moment, go ahead and sync the file (not clear if fflush does this)
    // return ret;
    return -1;
}

// reopen the file and seek to last known position
uint8_t FileHandlerTNFS::_bad_fd_recovery()
{
    Debug_println("FileHandlerTNFS - Invalid file ID");
    tnfsFileHandleInfo *pFileInf = _mountinfo->get_filehandleinfo(_handle);
    if (pFileInf == nullptr) 
    {
        Debug_printf("\tbad handle\n");
        return TNFS_RESULT_BAD_FILENUM;
    }

    int16_t handle;

    int result = tnfs_open(_mountinfo, pFileInf->filename, TNFS_OPENMODE_READ, 0, &handle);
    if(result != TNFS_RESULT_SUCCESS)
        return result;

    // last known position
    uint32_t pos = pFileInf->cached_pos;
    uint32_t new_pos;

    // update file handle / fd
    _handle = handle;
    // delete bad fd filehandleinfo
    _mountinfo->delete_filehandleinfo(pFileInf);

    // seek to last known position
    return tnfs_lseek(_mountinfo, _handle, pos, SEEK_SET, &new_pos);
}
