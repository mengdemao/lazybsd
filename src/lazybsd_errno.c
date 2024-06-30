/**
 * @file lazybsd_lazybsd_errno.cc
 * @author Meng Demao (mengdemao19951021@163.com)
 * @brief
 * @version 0.1
 * @date 2024-05-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "lazybsd_errno.h"
#include <errno.h>

void lazybsd_os_errno(int error)
{
    switch (error) {
        case LAZYBSD_EPERM:       lazybsd_errno = EPERM; break;
        case LAZYBSD_ENOENT:      lazybsd_errno = ENOENT; break;
        case LAZYBSD_ESRCH:       lazybsd_errno = ESRCH; break;
        case LAZYBSD_EINTR:       lazybsd_errno = EINTR; break;
        case LAZYBSD_EIO:         lazybsd_errno = EIO; break;
        case LAZYBSD_ENXIO:       lazybsd_errno = ENXIO; break;
        case LAZYBSD_E2BIG:       lazybsd_errno = E2BIG; break;
        case LAZYBSD_ENOEXEC:     lazybsd_errno = ENOEXEC; break;
        case LAZYBSD_EBADF:       lazybsd_errno = EBADF; break;
        case LAZYBSD_ECHILD:      lazybsd_errno = ECHILD; break;
        case LAZYBSD_EDEADLK:     lazybsd_errno = EDEADLK; break;
        case LAZYBSD_ENOMEM:      lazybsd_errno = ENOMEM; break;
        case LAZYBSD_EACCES:      lazybsd_errno = EACCES; break;
        case LAZYBSD_EFAULT:      lazybsd_errno = EFAULT; break;
        case LAZYBSD_ENOTBLK:     lazybsd_errno = ENOTBLK; break;
        case LAZYBSD_EBUSY:       lazybsd_errno = EBUSY; break;
        case LAZYBSD_EEXIST:      lazybsd_errno = EEXIST; break;
        case LAZYBSD_EXDEV:       lazybsd_errno = EXDEV; break;
        case LAZYBSD_ENODEV:      lazybsd_errno = ENODEV; break;
        case LAZYBSD_ENOTDIR:     lazybsd_errno = ENOTDIR; break;
        case LAZYBSD_EISDIR:      lazybsd_errno = EISDIR; break;
        case LAZYBSD_EINVAL:      lazybsd_errno = EINVAL; break;
        case LAZYBSD_ENFILE:      lazybsd_errno = ENFILE; break;
        case LAZYBSD_EMFILE:      lazybsd_errno = EMFILE; break;
        case LAZYBSD_ENOTTY:      lazybsd_errno = ENOTTY; break;
        case LAZYBSD_ETXTBSY:     lazybsd_errno = ETXTBSY; break;
        case LAZYBSD_EFBIG:       lazybsd_errno = EFBIG; break;
        case LAZYBSD_ENOSPC:      lazybsd_errno = ENOSPC; break;
        case LAZYBSD_ESPIPE:      lazybsd_errno = ESPIPE; break;
        case LAZYBSD_EROFS:       lazybsd_errno = EROFS; break;
        case LAZYBSD_EMLINK:      lazybsd_errno = EMLINK; break;
        case LAZYBSD_EPIPE:       lazybsd_errno = EPIPE; break;
        case LAZYBSD_EDOM:        lazybsd_errno = EDOM; break;
        case LAZYBSD_ERANGE:      lazybsd_errno = ERANGE; break;

        /* case LAZYBSD_EAGAIN:       same as EWOULDBLOCK */
        case LAZYBSD_EWOULDBLOCK:     lazybsd_errno = EWOULDBLOCK; break;

        case LAZYBSD_EINPROGRESS:     lazybsd_errno = EINPROGRESS; break;
        case LAZYBSD_EALREADY:        lazybsd_errno = EALREADY; break;
        case LAZYBSD_ENOTSOCK:        lazybsd_errno = ENOTSOCK; break;
        case LAZYBSD_EDESTADDRREQ:    lazybsd_errno = EDESTADDRREQ; break;
        case LAZYBSD_EMSGSIZE:        lazybsd_errno = EMSGSIZE; break;
        case LAZYBSD_EPROTOTYPE:      lazybsd_errno = EPROTOTYPE; break;
        case LAZYBSD_ENOPROTOOPT:     lazybsd_errno = ENOPROTOOPT; break;
        case LAZYBSD_EPROTONOSUPPORT: lazybsd_errno = EPROTONOSUPPORT; break;
        case LAZYBSD_ESOCKTNOSUPPORT: lazybsd_errno = ESOCKTNOSUPPORT; break;

        /* case LAZYBSD_EOPNOTSUPP:   same as ENOTSUP */
        case LAZYBSD_ENOTSUP:         lazybsd_errno = ENOTSUP; break;

        case LAZYBSD_EPFNOSUPPORT:    lazybsd_errno = EPFNOSUPPORT; break;
        case LAZYBSD_EAFNOSUPPORT:    lazybsd_errno = EAFNOSUPPORT; break;
        case LAZYBSD_EADDRINUSE:      lazybsd_errno = EADDRINUSE; break;
        case LAZYBSD_EADDRNOTAVAIL:   lazybsd_errno = EADDRNOTAVAIL; break;
        case LAZYBSD_ENETDOWN:        lazybsd_errno = ENETDOWN; break;
        case LAZYBSD_ENETUNREACH:     lazybsd_errno = ENETUNREACH; break;
        case LAZYBSD_ENETRESET:       lazybsd_errno = ENETRESET; break;
        case LAZYBSD_ECONNABORTED:    lazybsd_errno = ECONNABORTED; break;
        case LAZYBSD_ECONNRESET:      lazybsd_errno = ECONNRESET; break;
        case LAZYBSD_ENOBUFS:         lazybsd_errno = ENOBUFS; break;
        case LAZYBSD_EISCONN:         lazybsd_errno = EISCONN; break;
        case LAZYBSD_ENOTCONN:        lazybsd_errno = ENOTCONN; break;
        case LAZYBSD_ESHUTDOWN:       lazybsd_errno = ESHUTDOWN; break;
        case LAZYBSD_ETOOMANYREFS:    lazybsd_errno = ETOOMANYREFS; break;
        case LAZYBSD_ETIMEDOUT:       lazybsd_errno = ETIMEDOUT; break;
        case LAZYBSD_ECONNREFUSED:    lazybsd_errno = ECONNREFUSED; break;
        case LAZYBSD_ELOOP:           lazybsd_errno = ELOOP; break;
        case LAZYBSD_ENAMETOOLONG:    lazybsd_errno = ENAMETOOLONG; break;
        case LAZYBSD_EHOSTDOWN:       lazybsd_errno = EHOSTDOWN; break;
        case LAZYBSD_EHOSTUNREACH:    lazybsd_errno = EHOSTUNREACH; break;
        case LAZYBSD_ENOTEMPTY:       lazybsd_errno = ENOTEMPTY; break;
        case LAZYBSD_EUSERS:      lazybsd_errno = EUSERS; break;
        case LAZYBSD_EDQUOT:      lazybsd_errno = EDQUOT; break;
        case LAZYBSD_ESTALE:      lazybsd_errno = ESTALE; break;
        case LAZYBSD_EREMOTE:     lazybsd_errno = EREMOTE; break;
        case LAZYBSD_ENOLCK:      lazybsd_errno = ENOLCK; break;
        case LAZYBSD_ENOSYS:      lazybsd_errno = ENOSYS; break;
        case LAZYBSD_EIDRM:       lazybsd_errno = EIDRM; break;
        case LAZYBSD_ENOMSG:      lazybsd_errno = ENOMSG; break;
        case LAZYBSD_EOVERFLOW:   lazybsd_errno = EOVERFLOW; break;
        case LAZYBSD_ECANCELED:   lazybsd_errno = ECANCELED; break;
        case LAZYBSD_EILSEQ:      lazybsd_errno = EILSEQ; break;
        case LAZYBSD_EBADMSG:     lazybsd_errno = EBADMSG; break;
        case LAZYBSD_EMULTIHOP:   lazybsd_errno = EMULTIHOP; break;
        case LAZYBSD_ENOLINK:     lazybsd_errno = ENOLINK; break;
        case LAZYBSD_EPROTO:      lazybsd_errno = EPROTO; break;
        default:              lazybsd_errno = error; break;
    }
}

__thread int lazybsd_errno;

