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

#define LAZYBSD_EPERM         1        /* Operation not permitted */
#define LAZYBSD_ENOENT        2        /* No such file or directory */
#define LAZYBSD_ESRCH         3        /* No such process */
#define LAZYBSD_EINTR         4        /* Interrupted system call */
#define LAZYBSD_EIO           5        /* Input/output error */
#define LAZYBSD_ENXIO         6        /* Device not configured */
#define LAZYBSD_E2BIG         7        /* Argument list too long */
#define LAZYBSD_ENOEXEC       8        /* Exec format error */
#define LAZYBSD_EBADF         9        /* Bad file descriptor */
#define LAZYBSD_ECHILD        10        /* No child processes */
#define LAZYBSD_EDEADLK       11        /* Resource deadlock avoided */
#define LAZYBSD_ENOMEM        12        /* Cannot allocate memory */
#define LAZYBSD_EACCES        13        /* Permission denied */
#define LAZYBSD_EFAULT        14        /* Bad address */
#define LAZYBSD_ENOTBLK       15        /* Block device required */
#define LAZYBSD_EBUSY         16        /* Device busy */
#define LAZYBSD_EEXIST        17        /* File exists */
#define LAZYBSD_EXDEV         18        /* Cross-device link */
#define LAZYBSD_ENODEV        19        /* Operation not supported by device */
#define LAZYBSD_ENOTDIR       20        /* Not a directory */
#define LAZYBSD_EISDIR        21        /* Is a directory */
#define LAZYBSD_EINVAL        22        /* Invalid argument */
#define LAZYBSD_ENFILE        23        /* Too many open files in system */
#define LAZYBSD_EMFILE        24        /* Too many open files */
#define LAZYBSD_ENOTTY        25        /* Inappropriate ioctl for device */
#define LAZYBSD_ETXTBSY       26        /* Text file busy */
#define LAZYBSD_EFBIG         27        /* File too large */
#define LAZYBSD_ENOSPC        28        /* No space left on device */
#define LAZYBSD_ESPIPE        29        /* Illegal seek */
#define LAZYBSD_EROFS         30        /* Read-only filesystem */
#define LAZYBSD_EMLINK        31        /* Too many links */
#define LAZYBSD_EPIPE         32        /* Broken pipe */

/* math software */
#define LAZYBSD_EDOM          33        /* Numerical argument out of domain */
#define LAZYBSD_ERANGE        34        /* Result too large */

/* non-blocking and interrupt i/o */
#define LAZYBSD_EAGAIN        35        /* Resource temporarily unavailable */
#define LAZYBSD_EWOULDBLOCK   LAZYBSD_EAGAIN        /* Operation would block */
#define LAZYBSD_EINPROGRESS   36        /* Operation now in progress */
#define LAZYBSD_EALREADY      37        /* Operation already in progress */

/* ipc/network software -- argument errors */
#define LAZYBSD_ENOTSOCK      38        /* Socket operation on non-socket */
#define LAZYBSD_EDESTADDRREQ  39        /* Destination address required */
#define LAZYBSD_EMSGSIZE      40        /* Message too long */
#define LAZYBSD_EPROTOTYPE    41        /* Protocol wrong type for socket */
#define LAZYBSD_ENOPROTOOPT   42        /* Protocol not available */
#define LAZYBSD_EPROTONOSUPPORT    43        /* Protocol not supported */
#define LAZYBSD_ESOCKTNOSUPPORT    44        /* Socket type not supported */
#define LAZYBSD_EOPNOTSUPP         45        /* Operation not supported */
#define LAZYBSD_ENOTSUP        LAZYBSD_EOPNOTSUPP    /* Operation not supported */
#define LAZYBSD_EPFNOSUPPORT       46        /* Protocol family not supported */
#define LAZYBSD_EAFNOSUPPORT       47        /* Address family not supported by protocol family */
#define LAZYBSD_EADDRINUSE         48        /* Address already in use */
#define LAZYBSD_EADDRNOTAVAIL      49        /* Can't assign requested address */

/* ipc/network software -- operational errors */
#define LAZYBSD_ENETDOWN       50        /* Network is down */
#define LAZYBSD_ENETUNREACH    51        /* Network is unreachable */
#define LAZYBSD_ENETRESET      52        /* Network dropped connection on reset */
#define LAZYBSD_ECONNABORTED   53        /* Software caused connection abort */
#define LAZYBSD_ECONNRESET     54        /* Connection reset by peer */
#define LAZYBSD_ENOBUFS        55        /* No buffer space available */
#define LAZYBSD_EISCONN        56        /* Socket is already connected */
#define LAZYBSD_ENOTCONN       57        /* Socket is not connected */
#define LAZYBSD_ESHUTDOWN      58        /* Can't send after socket shutdown */
#define LAZYBSD_ETOOMANYREFS   59        /* Too many references: can't splice */
#define LAZYBSD_ETIMEDOUT      60        /* Operation timed out */
#define LAZYBSD_ECONNREFUSED   61        /* Connection refused */

#define LAZYBSD_ELOOP          62        /* Too many levels of symbolic links */
#define LAZYBSD_ENAMETOOLONG   63        /* File name too long */

/* should be rearranged */
#define LAZYBSD_EHOSTDOWN      64        /* Host is down */
#define LAZYBSD_EHOSTUNREACH   65        /* No route to host */
#define LAZYBSD_ENOTEMPTY      66        /* Directory not empty */

/* quotas & mush */
#define LAZYBSD_EPROCLIM       67        /* Too many processes */
#define LAZYBSD_EUSERS         68        /* Too many users */
#define LAZYBSD_EDQUOT         69        /* Disc quota exceeded */

#define LAZYBSD_ESTALE         70        /* Stale NFS file handle */
#define LAZYBSD_EREMOTE        71        /* Too many levels of remote in path */
#define LAZYBSD_EBADRPC        72        /* RPC struct is bad */
#define LAZYBSD_ERPCMISMATCH   73        /* RPC version wrong */
#define LAZYBSD_EPROGUNAVAIL   74        /* RPC prog. not avail */
#define LAZYBSD_EPROGMISMATCH  75        /* Program version wrong */
#define LAZYBSD_EPROCUNAVAIL   76        /* Bad procedure for program */

#define LAZYBSD_ENOLCK         77        /* No locks available */
#define LAZYBSD_ENOSYS         78        /* Function not implemented */

#define LAZYBSD_EFTYPE         79        /* Inappropriate file type or format */
#define LAZYBSD_EAUTH          80        /* Authentication error */
#define LAZYBSD_ENEEDAUTH      81        /* Need authenticator */
#define LAZYBSD_EIDRM          82        /* Identifier removed */
#define LAZYBSD_ENOMSG         83        /* No message of desired type */
#define LAZYBSD_EOVERFLOW      84        /* Value too large to be stored in data type */
#define LAZYBSD_ECANCELED      85        /* Operation canceled */
#define LAZYBSD_EILSEQ         86        /* Illegal byte sequence */
#define LAZYBSD_ENOATTR        87        /* Attribute not found */

#define LAZYBSD_EDOOFUS        88        /* Programming error */

#define LAZYBSD_EBADMSG        89        /* Bad message */
#define LAZYBSD_EMULTIHOP      90        /* Multihop attempted */
#define LAZYBSD_ENOLINK        91        /* Link has been severed */
#define LAZYBSD_EPROTO         92        /* Protocol error */

#define LAZYBSD_ENOTCAPABLE    93        /* Capabilities insufficient */
#define LAZYBSD_ECAPMODE       94        /* Not permitted in capability mode */
#define LAZYBSD_ENOTRECOVERABLE 95        /* State not recoverable */
#define LAZYBSD_EOWNERDEAD      96        /* Previous owner died */

extern __thread int lazybsd_errno;

#ifdef __cplusplus
}
#endif

#endif // LAZYBSD_ERRNO_H