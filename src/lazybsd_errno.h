/**
 * @file lazybsd_errno.h
 * @author mengdemao (mengdemao19951021@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LAZYBSD_ERRNO_H
#define LAZYBSD_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

#define lazybsd_EPERM         1        /* Operation not permitted */
#define lazybsd_ENOENT        2        /* No such file or directory */
#define lazybsd_ESRCH         3        /* No such process */
#define lazybsd_EINTR         4        /* Interrupted system call */
#define lazybsd_EIO           5        /* Input/output error */
#define lazybsd_ENXIO         6        /* Device not configured */
#define lazybsd_E2BIG         7        /* Argument list too long */
#define lazybsd_ENOEXEC       8        /* Exec format error */
#define lazybsd_EBADF         9        /* Bad file descriptor */
#define lazybsd_ECHILD        10        /* No child processes */
#define lazybsd_EDEADLK       11        /* Resource deadlock avoided */
#define lazybsd_ENOMEM        12        /* Cannot allocate memory */
#define lazybsd_EACCES        13        /* Permission denied */
#define lazybsd_EFAULT        14        /* Bad address */
#define lazybsd_ENOTBLK       15        /* Block device required */
#define lazybsd_EBUSY         16        /* Device busy */
#define lazybsd_EEXIST        17        /* File exists */
#define lazybsd_EXDEV         18        /* Cross-device link */
#define lazybsd_ENODEV        19        /* Operation not supported by device */
#define lazybsd_ENOTDIR       20        /* Not a directory */
#define lazybsd_EISDIR        21        /* Is a directory */
#define lazybsd_EINVAL        22        /* Invalid argument */
#define lazybsd_ENFILE        23        /* Too many open files in system */
#define lazybsd_EMFILE        24        /* Too many open files */
#define lazybsd_ENOTTY        25        /* Inappropriate ioctl for device */
#define lazybsd_ETXTBSY       26        /* Text file busy */
#define lazybsd_EFBIG         27        /* File too large */
#define lazybsd_ENOSPC        28        /* No space left on device */
#define lazybsd_ESPIPE        29        /* Illegal seek */
#define lazybsd_EROFS         30        /* Read-only filesystem */
#define lazybsd_EMLINK        31        /* Too many links */
#define lazybsd_EPIPE         32        /* Broken pipe */

/* math software */
#define lazybsd_EDOM          33        /* Numerical argument out of domain */
#define lazybsd_ERANGE        34        /* Result too large */

/* non-blocking and interrupt i/o */
#define lazybsd_EAGAIN        35        /* Resource temporarily unavailable */
#define lazybsd_EWOULDBLOCK   lazybsd_EAGAIN        /* Operation would block */
#define lazybsd_EINPROGRESS   36        /* Operation now in progress */
#define lazybsd_EALREADY      37        /* Operation already in progress */

/* ipc/network software -- argument errors */
#define lazybsd_ENOTSOCK      38        /* Socket operation on non-socket */
#define lazybsd_EDESTADDRREQ  39        /* Destination address required */
#define lazybsd_EMSGSIZE      40        /* Message too long */
#define lazybsd_EPROTOTYPE    41        /* Protocol wrong type for socket */
#define lazybsd_ENOPROTOOPT   42        /* Protocol not available */
#define lazybsd_EPROTONOSUPPORT    43        /* Protocol not supported */
#define lazybsd_ESOCKTNOSUPPORT    44        /* Socket type not supported */
#define lazybsd_EOPNOTSUPP         45        /* Operation not supported */
#define lazybsd_ENOTSUP        lazybsd_EOPNOTSUPP    /* Operation not supported */
#define lazybsd_EPFNOSUPPORT       46        /* Protocol family not supported */
#define lazybsd_EAFNOSUPPORT       47        /* Address family not supported by protocol family */
#define lazybsd_EADDRINUSE         48        /* Address already in use */
#define lazybsd_EADDRNOTAVAIL      49        /* Can't assign requested address */

/* ipc/network software -- operational errors */
#define lazybsd_ENETDOWN       50        /* Network is down */
#define lazybsd_ENETUNREACH    51        /* Network is unreachable */
#define lazybsd_ENETRESET      52        /* Network dropped connection on reset */
#define lazybsd_ECONNABORTED   53        /* Software caused connection abort */
#define lazybsd_ECONNRESET     54        /* Connection reset by peer */
#define lazybsd_ENOBUFS        55        /* No buffer space available */
#define lazybsd_EISCONN        56        /* Socket is already connected */
#define lazybsd_ENOTCONN       57        /* Socket is not connected */
#define lazybsd_ESHUTDOWN      58        /* Can't send after socket shutdown */
#define lazybsd_ETOOMANYREFS   59        /* Too many references: can't splice */
#define lazybsd_ETIMEDOUT      60        /* Operation timed out */
#define lazybsd_ECONNREFUSED   61        /* Connection refused */

#define lazybsd_ELOOP          62        /* Too many levels of symbolic links */
#define lazybsd_ENAMETOOLONG   63        /* File name too long */

/* should be rearranged */
#define lazybsd_EHOSTDOWN      64        /* Host is down */
#define lazybsd_EHOSTUNREACH   65        /* No route to host */
#define lazybsd_ENOTEMPTY      66        /* Directory not empty */

/* quotas & mush */
#define lazybsd_EPROCLIM       67        /* Too many processes */
#define lazybsd_EUSERS         68        /* Too many users */
#define lazybsd_EDQUOT         69        /* Disc quota exceeded */

#define lazybsd_ESTALE         70        /* Stale NFS file handle */
#define lazybsd_EREMOTE        71        /* Too many levels of remote in path */
#define lazybsd_EBADRPC        72        /* RPC struct is bad */
#define lazybsd_ERPCMISMATCH   73        /* RPC version wrong */
#define lazybsd_EPROGUNAVAIL   74        /* RPC prog. not avail */
#define lazybsd_EPROGMISMATCH  75        /* Program version wrong */
#define lazybsd_EPROCUNAVAIL   76        /* Bad procedure for program */

#define lazybsd_ENOLCK         77        /* No locks available */
#define lazybsd_ENOSYS         78        /* Function not implemented */

#define lazybsd_EFTYPE         79        /* Inappropriate file type or format */
#define lazybsd_EAUTH          80        /* Authentication error */
#define lazybsd_ENEEDAUTH      81        /* Need authenticator */
#define lazybsd_EIDRM          82        /* Identifier removed */
#define lazybsd_ENOMSG         83        /* No message of desired type */
#define lazybsd_EOVERFLOW      84        /* Value too large to be stored in data type */
#define lazybsd_ECANCELED      85        /* Operation canceled */
#define lazybsd_EILSEQ         86        /* Illegal byte sequence */
#define lazybsd_ENOATTR        87        /* Attribute not found */

#define lazybsd_EDOOFUS        88        /* Programming error */

#define lazybsd_EBADMSG        89        /* Bad message */
#define lazybsd_EMULTIHOP      90        /* Multihop attempted */
#define lazybsd_ENOLINK        91        /* Link has been severed */
#define lazybsd_EPROTO         92        /* Protocol error */

#define lazybsd_ENOTCAPABLE    93        /* Capabilities insufficient */
#define lazybsd_ECAPMODE       94        /* Not permitted in capability mode */
#define lazybsd_ENOTRECOVERABLE 95        /* State not recoverable */
#define lazybsd_EOWNERDEAD      96        /* Previous owner died */

#ifdef __cplusplus
}
#endif

#endif // LAZYBSD_ERRNO_H