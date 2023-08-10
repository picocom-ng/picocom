#ifndef PICOCOM_H
#define PICOCOM_H

#include <time.h>
#include "term.h"
#include "fdio.h"
#include "split.h"
#include "custbaud.h"

#define RTS_DTR_NONE 0
#define RTS_DTR_RAISE 1
#define RTS_DTR_LOWER 2
#define RTS_DTR_ERROR 3

/* implemented caracter mappings */
#define M_CRLF    (1 << 0)  /* map CR  --> LF */
#define M_CRCRLF  (1 << 1)  /* map CR  --> CR + LF */
#define M_IGNCR   (1 << 2)  /* map CR  --> <nothing> */
#define M_LFCR    (1 << 3)  /* map LF  --> CR */
#define M_LFCRLF  (1 << 4)  /* map LF  --> CR + LF */
#define M_IGNLF   (1 << 5)  /* map LF  --> <nothing> */
#define M_DELBS   (1 << 6)  /* map DEL --> BS */
#define M_BSDEL   (1 << 7)  /* map BS  --> DEL */
#define M_SPCHEX  (1 << 8)  /* map special chars --> hex */
#define M_TABHEX  (1 << 9)  /* map TAB --> hex */
#define M_CRHEX   (1 << 10)  /* map CR --> hex */
#define M_LFHEX   (1 << 11) /* map LF --> hex */
#define M_8BITHEX (1 << 12) /* map 8-bit chars --> hex */
#define M_NRMHEX  (1 << 13) /* map normal ascii chars --> hex */
#define M_NFLAGS 14

/* default character mappings */
#define M_I_DFL 0
#define M_O_DFL 0
#define M_E_DFL (M_DELBS | M_CRCRLF)

/* control-key to printable character (lowcase) */
#define KEYC(k) ((k) | 0x60)
/* printable character to control-key */
#define CKEY(c) ((c) & 0x1f)

#define RUNCMD_ARGS_MAX 32
#define RUNCMD_EXEC_FAIL 126

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
