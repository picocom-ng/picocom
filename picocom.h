#ifndef PICOCOM_H
#define PICOCOM_H

#include <time.h>
#include "term.h"

typedef struct {
    char *port;
    int baud;
    enum flowcntrl_e flow;
    enum parity_e parity;
    int databits;
    int stopbits;
    int lecho;
    int noinit;
    int noreset;
    int hangup;
#if defined (UUCP_LOCK_DIR) || defined (USE_FLOCK)
    int nolock;
#endif
    unsigned char escape;
    int noescape;
    char *send_cmd;
    char *receive_cmd;
    int imap;
    int omap;
    int emap;
    char *log_filename;
    char *initstring;
    int exit_after;
    int exit;
    int raise_lower_rts;
    int raise_lower_dtr;
    int excl;
    int quiet;
    struct timespec txdelay;
} picocom_options_t;

extern picocom_options_t opts;

#endif // PICOCOM_H
