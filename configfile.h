#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <confuse.h>
#include "picocom.h"

#define CONFIGFILE_LOCAL_NAME ".picocom.conf"
#define CONFIGFILE_GLOBAL_NAME "picocom/picocom.conf"

typedef struct {
    const char *name;
    int value;
} cfg_value_map;

extern cfg_value_map valid_map_values[];
extern cfg_t *cfg;

/* config.c */
extern char *default_config_file(void);
extern char *find_config_file(void);
extern int map_from_config(cfg_t *cfg, const char *name);
extern void init_config(void);
extern int parse_config_file(const char *cfgfile);

#endif // CONFIGFILE_H
