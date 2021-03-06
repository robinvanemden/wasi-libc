// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef COMMON_ERRNO_H
#define COMMON_ERRNO_H

#include <wasi/api.h>

#ifdef __wasilibc_unmodified_upstream
// Translates ENOTCAPABLE to ENOTDIR if not a directory.
static inline __wasi_errno_t errno_fixup_directory(__wasi_fd_t fd,
                                                     __wasi_errno_t error) {
  if (error == __WASI_ENOTCAPABLE) {
    __wasi_fdstat_t fds;
    if (__wasi_fd_stat_get(fd, &fds) == 0 &&
        fds.fs_filetype != __WASI_FILETYPE_DIRECTORY)
      return __WASI_ENOTDIR;
  }
  return error;
}
#else
// WASI syscalls should just return ENOTDIR if that's what the problem is.
static inline __wasi_errno_t errno_fixup_directory(__wasi_fd_t fd,
                                                     __wasi_errno_t error) {
  return error;
}
#endif

#ifdef __wasilibc_unmodified_upstream // posix_spawn etc.
// Translates ENOTCAPABLE to EBADF if a regular file or EACCES otherwise.
static inline __wasi_errno_t errno_fixup_executable(__wasi_fd_t fd,
                                                      __wasi_errno_t error) {
  if (error == __WASI_ENOTCAPABLE) {
    __wasi_fdstat_t fds;
    if (__wasi_fd_stat_get(fd, &fds) == 0)
      return fds.fs_filetype == __WASI_FILETYPE_REGULAR_FILE
                 ? __WASI_EBADF
                 : __WASI_EACCES;
  }
  return error;
}
#endif

#ifdef __wasilibc_unmodified_upstream // process file descriptors
// Translates ENOTCAPABLE to EINVAL if not a process.
static inline __wasi_errno_t errno_fixup_process(__wasi_fd_t fd,
                                                   __wasi_errno_t error) {
  if (error == __WASI_ENOTCAPABLE) {
    __wasi_fdstat_t fds;
    if (__wasi_fd_stat_get(fd, &fds) == 0 &&
        fds.fs_filetype != __WASI_FILETYPE_PROCESS)
      return __WASI_EINVAL;
  }
  return error;
}
#endif

#ifdef __wasilibc_unmodified_upstream
// Translates ENOTCAPABLE to ENOTSOCK if not a socket.
static inline __wasi_errno_t errno_fixup_socket(__wasi_fd_t fd,
                                                  __wasi_errno_t error) {
  if (error == __WASI_ENOTCAPABLE) {
    __wasi_fdstat_t fds;
    if (__wasi_fd_stat_get(fd, &fds) == 0 &&
#ifdef __wasilibc_unmodified_upstream // don't hard-code magic numbers
        (fds.fs_filetype & 0xf0) != 0x80)
#else
        (fds.fs_filetype != __WASI_FILETYPE_SOCKET_STREAM &&
         fds.fs_filetype != __WASI_FILETYPE_SOCKET_DGRAM))
#endif
      return __WASI_ENOTSOCK;
  }
  return error;
}
#else
// WASI syscalls should just return ENOTSOCK if that's what the problem is.
static inline __wasi_errno_t errno_fixup_socket(__wasi_fd_t fd,
                                                  __wasi_errno_t error) {
  return error;
}
#endif

#endif
