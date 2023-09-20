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
#include "args_parser.h"
#include "debug.h"

int main(int argc, const char **argv) {
    assert(argv != NULL);

    if (argc == 1)
        errx(1, "You need to provide a sub-command of: `run`, `build` or `clean`");

    args_t args = parse_args(argc, argv);

    #ifdef DEBUG
        char *stringed = to_string(&args);
        printf("%s\n", stringed);
        free(stringed);
    #endif
    
    if (!args.run_func)
        errx(1, "You need to provide a sub-command of: `run`, `build` or `clean`");

    assert(args.run_args != NULL);
    assert(args.build_name != NULL);

    run_whiteboard(&args);

    free_args(args);
    return 0;
}