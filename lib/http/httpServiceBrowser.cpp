#include <bsd/string.h>

#include "httpServiceBrowser.h"
#include "httpService.h"
#include "fuji.h"
#include "fnFsSD.h"
#include "fnFsTNFS.h"

#include "debug.h"



int fnHttpServiceBrowser::browse_url_encode(const char *src, size_t src_len, char *dst, size_t dst_len)
{
    static const char hex[] = "0123456789abcdef";
    size_t i, j;
    for (i = j = 0; i < src_len && j + 1 < dst_len; i++, j++)
    {
        if ((src[i] < 'A' || src[i] > 'Z') &&
            (src[i] < 'a' || src[i] > 'z') && 
            (src[i] < '0' || src[i] > '9') &&
            src[i] != '-' && src[i] != '.' && src[i] != '_' && src[i] != '~')
        {
            if (j + 3 < dst_len)
            {
                dst[j++] = '%';
                dst[j++] = hex[src[i] >> 4];
                dst[j] = hex[src[i] & 0x0f];
            } else
            {
                return -1;
            }
        } else 
        {
            dst[j] = src[i];
        }
    }
    if (j < dst_len) dst[j] = '\0';  // Null-terminate the destination
    return i >= src_len ? (int) j : -1;
}


int fnHttpServiceBrowser::browse_html_escape(const char *src, size_t src_len, char *dst, size_t dst_len)
{
    size_t i, j;
    for (i = j = 0; i < src_len && j + 1 < dst_len; i++, j++)
    {
        if (src[i] == '<' || src[i] == '>' || src[i] == '&' || src[i] == '\'' || src[i] == '"')
        {
            // replace above with &#NN; note: above character codes are < 100
            if (j + 5 < dst_len)
            {
                dst[j++] = '&';
                dst[j++] = '#';
                dst[j++] = '0' + src[i] / 10;
                dst[j++] = '0' + src[i] % 10;
                dst[j] = ';';
            } else
            {
                return -1;
            }
        } else 
        {
            dst[j] = src[i];
        }
    }
    if (j < dst_len) dst[j] = '\0';  // Null-terminate the destination
    return i >= src_len ? (int) j : -1;
}


int fnHttpServiceBrowser::browse_listdir(mg_connection *c, FileSystem *pFS, int slot, const char *host_path, unsigned pathlen)
{
    char path[256];
    char enc_path[256]; // URL encoded path
    char esc_path[256]; // HTML escaped path

    if (pathlen > 0)
    {
        if ((pathlen >= sizeof(enc_path)) || (mg_url_decode(host_path, pathlen, path, sizeof(path), 0) < 0))
        {
            mg_http_reply(c, 403, "", "Path too long\n");
            return -1;
        }
        else
        {
            strlcpy(enc_path, host_path, pathlen+1);
        }
    }
    else
    {
        strcpy(path, "/");
        strcpy(enc_path, "/");
    }

    if (browse_html_escape(path, strlen(path), esc_path, sizeof(esc_path)) < 0)
    {
        strcpy(esc_path, "&lt;-- Path too long --&gt;");
    }


    if (!pFS->dir_open(path, "", 0))
    {
        FileHandler *fh = pFS->filehandler_open(path);
        if (fh != nullptr)
        {
            browse_sendfile(c, fh, fnHttpService::get_basename(path), pFS->filesize(fh));
            fh->close();
            return 0;
        }
        else
        {
            Debug_printf("Couldn't open host directory: %s\n", path);
            mg_http_reply(c, 400, "", "Couldn't open directory\n");
            return -1;
        }
    }

    mg_printf(c, "%s\r\n", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n");
    mg_http_printf_chunk(
        c,
        "<!DOCTYPE html><html><head><title>Browse Host %d</title>"
        "<style>th,td {text-align: left; padding-right: 1em; "
        "font-family: monospace; }</style></head>"
        "<body><h1>[Host %d] %s %s</h1><table cellpadding=\"0\"><thead>"
        "<tr><th>Size</th><th>Modified</th><th>Name</th></tr>"
        "<tr><td colspan=\"3\"><hr></td></tr></thead><tbody>",
        slot+1, slot+1, theFuji.get_hosts(slot)->get_hostname(), esc_path, esc_path); // TODO escape hostname

	fsdir_entry *dp;
	while ((dp = pFS->dir_read()) != nullptr)
    {
        Debug_printf("%d %s\t%d\t%lu\n", dp->isDir, dp->filename, (int)dp->size, (unsigned long)dp->modified_time);
        // Do not show current dir and hidden files
        if (!strcmp(dp->filename, ".") || !strcmp(dp->filename, ".."))
            continue;
        browse_printdentry(c, dp, slot, enc_path);
    }
    mg_http_printf_chunk(
        c,
        "</tbody><tfoot><tr><td colspan=\"3\"><hr></td></tr></tfoot>"
        "</table></body></html>");
    mg_http_write_chunk(c, "", 0);

    pFS->dir_close();
    return 0;
}


void fnHttpServiceBrowser::browse_printdentry(mg_connection *c, fsdir_entry *dp, int slot, const char *enc_path)
{
    char size[64], mod[64];
    const char *slash = dp->isDir ? "/" : "";
    const char *sep = enc_path[strlen(enc_path)-1] == '/' ? "" : "/";
    char enc_filename[128]; // URL encoded file name
    char esc_filename[128]; // HTML escaped file name

    if (browse_url_encode(dp->filename, strlen(dp->filename), enc_filename, sizeof(enc_filename)) < 0)
    {
        enc_filename[0] = '\0';
    }

    if (browse_html_escape(dp->filename, strlen(dp->filename), esc_filename, sizeof(esc_filename)) < 0)
    {
        strcpy(esc_filename, "&lt;-- Name too long --&gt;");
    }

    if (dp->isDir) {
        snprintf(size, sizeof(size), "%s", "[DIR]");
    } else {
        if (dp->size < 1024) {
            snprintf(size, sizeof(size), "%d", (int) dp->size);
        } else if (dp->size < 0x100000) {
            snprintf(size, sizeof(size), "%.1fk", (double) dp->size / 1024.0);
        } else if (dp->size < 0x40000000) {
            snprintf(size, sizeof(size), "%.1fM", (double) dp->size / 1048576);
        } else {
            snprintf(size, sizeof(size), "%.1fG", (double) dp->size / 1073741824);
        }
    }
    strftime(mod, sizeof(mod), "%d-%b-%Y %H:%M", localtime(&dp->modified_time));
    mg_http_printf_chunk(c,
        "<tr><td>%s</td><td>%s</td><td><a href=\"/browse/host/%d%s%s%s\">%s%s</a></td></tr>",
        size, mod, slot+1, enc_path, sep, enc_filename, esc_filename, slash);
}


void fnHttpServiceBrowser::browse_sendfile(mg_connection *c, FileHandler *fh, const char *filename, unsigned long filesize)
{
    mg_printf(c, "HTTP/1.1 200 OK\r\n");
    // Set the response content type
    fnHttpService::set_file_content_type(c, filename);
    // Set the expected length of the content
    mg_printf(c, "Content-Length: %lu\r\n\r\n", filesize);

    // Send the file content out in chunks
    char *buf = (char *)malloc(FNWS_SEND_BUFF_SIZE);
    size_t count = 0, total = 0;
    do
    {
        count = fh->read((uint8_t *)buf, 1, FNWS_SEND_BUFF_SIZE);
        total += count;
        mg_send(c, buf, count);
    } while (count > 0);
    free(buf);

    Debug_printf("Sent %lu of %lu bytes from %s\n", (unsigned long)total, (unsigned long)filesize, filename);
}


int fnHttpServiceBrowser::process_browse_get(mg_connection *c, int host_slot, const char *host_path, unsigned pathlen)
{
    fujiHost *pHost = theFuji.get_hosts(host_slot);
    FileSystem *pFS;
    int host_type;
    bool started = false;

    Debug_printf("Browse host %d (%s)\n", host_slot, pHost->get_hostname());

    if (strcmp("SD", pHost->get_hostname()) == 0)
    {
        pFS = new FileSystemSDFAT;
        host_type = HOSTTYPE_LOCAL;
    }
    else
    {
        pFS = new FileSystemTNFS;
        host_type = HOSTTYPE_TNFS;
    }

    if (pFS == nullptr)
    {
        Debug_println("Couldn't create a new File System");
        mg_http_reply(c, 400, "", "Couldn't create a new File System\n");
        return -1;
    }

    Debug_println("Starting temporary File System");
    // why is start() not in base class ?
    if (host_type == HOSTTYPE_LOCAL)
        started = ((FileSystemSDFAT *)pFS)->start();
    else
        started = ((FileSystemTNFS *)pFS)->start(pHost->get_hostname());

    if (!started)
    {
        Debug_println("Couldn't start File System");
        mg_http_reply(c, 400, "", "File System error\n");
        delete pFS;
        return -1;
    }


    int result = browse_listdir(c, pFS, host_slot, host_path, pathlen);

    Debug_println("Destroying temporary File System");
    delete pFS;

    return result;
}