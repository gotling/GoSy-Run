#include <pebble.h>
#include "storage.h"
#include "stretch/config.h"

#define STORAGE_VERSION_PKEY 30
#define CURRENT_STORAGE_VERSION 7
#define STRETCH_PKEY 31

int persist_version(void) {
    uint32_t version = persist_read_int(STORAGE_VERSION_PKEY);
    
    if (version == 0) {
        // If setting was stored using old format with KEY_ID 1
        if (persist_exists(1)) {
            return 6;
        }

        return 0;
    }

    return version;
}

static void persist_version_set(int version) {
    persist_write_int(STORAGE_VERSION_PKEY, version);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist:version:set - pkey: %d, version: %d", STORAGE_VERSION_PKEY, version);
}

int persist_current_version(void) {
    return CURRENT_STORAGE_VERSION;
}

static void persist_migrate(void) {
    int version = persist_version();
    if (version == persist_current_version()) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "persist:migrate - already up to date");
        return;
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist:migrate - v%d to v%d", version, persist_current_version());
    stretch_persist_migrate(STRETCH_PKEY, version);

    //persist_version_set(persist_current_version());
}

void persist_stretch_write(void) {
    stretch_persist_write(STRETCH_PKEY);
}

void persist_read(void) {
    persist_migrate();

    stretch_persist_read(STRETCH_PKEY);
}