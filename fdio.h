/* vi: set sw=4 ts=4 et:
 *
 * fdio.h
 *
 * Functions for doing I/O on file descriptors.
 *
 * by Nick Patavalis (npat@efault.net)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef FDIO_H

ssize_t writen_ni(int fd, const void *buff, size_t n);

int fd_vprintf (int fd, const char *format, va_list ap);

int fd_printf (int fd, const char *format, ...);

#ifndef LINENOISE

int fd_readline (int fdi, int fdo, char *b, int bsz);

#endif

#endif /* of FDIO_H */

/**********************************************************************/

/*
 * Local Variables:
 * mode:c
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
