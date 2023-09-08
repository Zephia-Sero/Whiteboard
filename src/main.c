#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <err.h>
#include <assert.h>

#include "run_bin.h"
#include "rust_types.h"
#include "toml.h"
#include "toml_format.h"

i32 main(int argc, const char **argv) {
    assert(argv != NULL);

    bool run_mode = false;
    bool default_build = true;
    char *build_name;
    char *run_args = malloc(sizeof(char));
    assert(run_args != NULL);
    
    if (argc < 2)
        errx(1, "Whiteboard requires a sub-command of: `build`, or `run`");
    if (argc >= 3)
        if (strcmp(argv[2], "--") != 0)
            default_build = false;

    if (strcmp(argv[1], "run") == 0)
        run_mode = true;
    if (argc >= 4 && run_mode) {
        bool got_two_dashes = false;
        for (int i = 3; i <= argc; i++) {
            const char *arg = argv[i - 1];

            if (got_two_dashes) {
                char *formatted = malloc(sizeof(char) * (strlen(arg) + 5));
                sprintf(formatted, "%s ", arg);
                run_args = (char *)realloc(run_args, sizeof(char) * (strlen(run_args) + strlen(formatted) + 1));
                strcat(run_args, formatted);
                free(formatted);
            }

            if (strcmp(arg, "--") == 0) {
                got_two_dashes = true;
            }
        }
    }
    if (!run_args)
        printf("%s\n", run_args);

    if (!default_build)
        build_name = (char *)argv[2];

    FILE *fp;
    char errorBuffer[200];

    fp = fopen("whiteboard.toml", "r");
    if (!fp) {
        err(1, "Failed to open whiteboard.toml");
    }

    // Actually parse the toml now
    toml_table_t *conf = toml_parse_file(fp, errorBuffer, sizeof(errorBuffer));
    fclose(fp);

    if (!conf) {
        errx(1, "Can't parse: %s", errorBuffer);
    }

    config_t config = init_config();
    config.callbacks.make_config(&config, conf);
    for (int i = 0; ; i++) {
        bin_t *value = (bin_t *)config.bin.callbacks.get(&config.bin, i);
        if (value == NULL) {
            printf("Can't find a build by that name\n");
            break;
        }

        if (strcmp(value->name, build_name) == false) {
            build_bin(&config.package, value, run_mode, run_args);
            break;
        }

        if (default_build) {
            build_bin(&config.package, value, run_mode, run_args);
            break;
        }
    }

    free(run_args);
    toml_free(conf);
    free_config(config);
    return 0;
}
