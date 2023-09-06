#include "toml_format.h"
#include "vector.h"

#include <err.h>
#include <stdbool.h>

// Inits a blank package
package_t init_package() {
    package_t package;
    package.name = NULL;
    package.version = NULL;
    package.callbacks.make_package = &make_package;
    return package;
}

// Makes a package struct given the toml
void make_package(package_t *self, toml_table_t *toml) {
    toml_table_t *package = toml_table_in(toml, "package");
    if (!package) {
        err(1, "Package doesn't exist in whiteboard.toml");
    }
  
    toml_datum_t name = toml_string_in(package, "name");
    toml_datum_t version = toml_string_in(package, "version");

    self->name = name.u.s;
    self->version = version.u.s;
}

bin_t init_bin() {
    bin_t bin;
    bin.default_bin = false;
    bin.name = NULL;
    return bin;
}

void make_bin(config_t *self, toml_table_t *toml) {
    toml_array_t *array = toml_array_in(toml, "bin");
    if (!array) {
        err(1, "Bins doesn't exist in whiteboard.toml");
    }
    for (int i = 0; ; i++) {
        toml_table_t *key = toml_table_at(array, i);
        self->bin.callbacks.push(&self->bin, (void *)key);

        if (!key)
            break;
    }
}

// Inits a blank config
config_t init_config() {
    config_t config;
    config.package = init_package();
    config.bin = init_vector();
    config.callbacks.make_bin = &make_bin;
    return config;
}