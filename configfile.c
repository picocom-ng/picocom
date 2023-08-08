#include <confuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <errno.h>

#include "configfile.h"
#include "picocom.h"

cfg_value_map valid_flow_values[] = {
    {"none", FC_NONE},
    {"hard", FC_RTSCTS},
    {"rtscts", FC_RTSCTS},
    {"soft", FC_XONXOFF},
    {"xonxoff", FC_XONXOFF},
    {NULL, 0},
};

cfg_value_map valid_parity_values[] = {
    {"none", P_NONE},
    {"even", P_EVEN},
    {"odd", P_ODD},
    {"mark", P_MARK},
    {"space", P_SPACE},
    {NULL, 0}
};

cfg_value_map valid_rts_values[] = {
    {"none", RTS_DTR_NONE},
    {"raise", RTS_DTR_RAISE},
    {"lower", RTS_DTR_LOWER},
    {NULL, 0}
};

cfg_value_map *valid_dtr_values = valid_rts_values;

cfg_t *cfg;

static int
cb_validate_single_name(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result, cfg_value_map *valid_values)
{
    for (int i=0; valid_values[i].name; i++) {
        if (strcmp(value, valid_values[i].name) == 0) {
            *(long int *)result = valid_values[i].value;
            return 0;
        }
    }

    cfg_error(cfg, "invalid value for %s: %s", opt->name, value);
    return -1;
}

static int
cb_validate_map(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result)
{
    return cb_validate_single_name(cfg, opt, value, result, valid_map_values);
}

static int
cb_validate_flow(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result)
{
    return cb_validate_single_name(cfg, opt, value, result, valid_flow_values);
}

static int
cb_validate_parity(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result)
{
    return cb_validate_single_name(cfg, opt, value, result, valid_parity_values);
}

static int
cb_validate_rts(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result)
{
    return cb_validate_single_name(cfg, opt, value, result, valid_rts_values);
}

static int
cb_validate_dtr(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result)
{
    return cb_validate_single_name(cfg, opt, value, result, valid_dtr_values);
}

static int
cb_validate_char(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result)
{
    if (strcmp("none", value) == 0) {
        *(long int *)result = 0;
        return 0;
    }

    if (strlen(value) != 1) {
        cfg_error(cfg, "%s must be a single character, not %s", opt->name, value);
        return -1;
    }

    *(long int *)result = CKEY(value[0]);
    return 0;
}

char
*default_config_file()
{
    char *cfdir;
    static char cfpath[PATH_MAX];

    if ((cfdir = getenv("XDG_CONFIG_HOME")) == NULL) {
        char *homedir;
        if ((homedir = getenv("HOME")) == NULL) {
            return NULL;
        }

        snprintf(cfpath, PATH_MAX, "%s/.config/%s", homedir, CONFIGFILE_GLOBAL_NAME);
    } else {
        snprintf(cfpath, PATH_MAX, "%s/%s", cfdir, CONFIGFILE_GLOBAL_NAME);
    }

    return cfpath;
}

char
*find_config_file(void)
{
    char pathbuf[PATH_MAX];
    static char filebuf[PATH_MAX];
    char *path;

    // If PICOCOM_CONFIG_FILE is set, use that.
    if ((path  = getenv("PICOCOM_CONFIG_FILE")) != NULL) {
        return path;
    }

    // Otherwise, search for .picocom.conf in current directory and
    // all parent directories.
    path = getcwd(pathbuf, PATH_MAX);
    while (1) {
        snprintf(filebuf, PATH_MAX, "%s/%s", path, ".picocom.conf");
        if (access(filebuf, R_OK) == 0) {
            return filebuf;
        }

        if (strcmp(path, "/") == 0) {
            break;
        }

        path = dirname(path);
    }

    // Use the default if nothing else exists.
    return default_config_file();
}

// Convert a list of map names in the configuration file
// into a map bitfield.
int
map_from_config(cfg_t *cfg, const char *name)
{
    int value = 0;
    cfg_opt_t *opt = cfg_getopt(cfg, name);

    for (int i=0; i<opt->nvalues; i++) {
        value |= opt->values[i]->number;
    }

    return value;
}

// Initialize configuration file support and parse the config
// file. It is an error if the configuration file is found and
// contains invalid values, but it's okay if the configuration
// file doesn't exist.
void init_config() {
    // This is defined here instead of globally so that we can
    // reference values in opts as defaults, which allows us to
    // keep defaults in one place.
    cfg_opt_t cfgopts[] = {
        CFG_INT("baud", opts.baud, CFGF_NONE),
        CFG_INT("databits", opts.databits, CFGF_NONE),
        CFG_INT_CB("dtr", opts.raise_lower_dtr, CFGF_NONE, &cb_validate_dtr),
        CFG_BOOL("echo", opts.lecho, CFGF_NONE),
        CFG_INT_CB("escape", opts.escape, CFGF_NONE, &cb_validate_char),
        CFG_INT_CB("flow", opts.flow, CFGF_NONE, &cb_validate_flow),
        CFG_BOOL("hangup", opts.hangup, CFGF_NONE),
        CFG_STR("initstring", NULL, CFGF_NONE),
        CFG_STR("logfile", NULL, CFGF_NONE),
        CFG_BOOL("noinit", opts.noinit, CFGF_NONE),
        CFG_BOOL("noreset", opts.noreset, CFGF_NONE),
        CFG_INT_CB("parity", opts.parity, CFGF_NONE, &cb_validate_parity),
        CFG_STR("port", NULL, CFGF_NONE),
        CFG_BOOL("excl", opts.excl, CFGF_NONE),
        CFG_BOOL("quiet", opts.quiet, CFGF_NONE),
        CFG_STR("receive-cmd", opts.receive_cmd, CFGF_NONE),
        CFG_INT_CB("rts", opts.raise_lower_rts, CFGF_NONE, &cb_validate_rts),
        CFG_STR("send-cmd", opts.send_cmd, CFGF_NONE),
        CFG_INT("stopbits", opts.stopbits, CFGF_NONE),
        CFG_INT("tx-delay", opts.txdelay.tv_nsec, CFGF_NONE),
        CFG_INT_LIST_CB("emap", "{crcrlf, delbs}", CFGF_NONE, &cb_validate_map),
        CFG_INT_LIST_CB("imap", NULL, CFGF_NONE, &cb_validate_map),
        CFG_INT_LIST_CB("omap", NULL, CFGF_NONE, &cb_validate_map),
#if defined (UUCP_LOCK_DIR) || defined (USE_FLOCK)
        CFG_BOOL("nolock", opts.nolock, CFGF_NONE),
#endif
        CFG_END()
    };

    cfg = cfg_init(cfgopts, 0);
}

int parse_config_fp(FILE *fp) {
        if (cfg_parse_fp(cfg, fp) == CFG_PARSE_ERROR) {
            fprintf(stderr, "Failed to parse configuration file\n");
            return -1;
        }

        return 0;
}

int parse_config_file(const char *cfgfile) {
    if (!cfgfile) {
        cfgfile = find_config_file();
    }

    if (cfgfile) {
        FILE *fp;
        int res;

        if ((fp = fopen(cfgfile, "r")) != NULL) {
            res = parse_config_fp(fp);
            fclose(fp);
            return res;
        }
    }

    return 0;
}
