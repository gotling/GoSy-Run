#include <pebble.h>
#include "config.h"
#include "common/tools.h"
#include "common/storage.h"

#define STRETCH_TIME_DEFAULT 20
#define PREPARE_TIME_DEFAULT 3

#define SETS 13

StretchSettings stretch_settings;

void stretch_persist_read(int pkey) {
	int bytesRead = persist_read_data(pkey, &stretch_settings, sizeof(StretchSettings));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "stretch:persist:read - pkey: %d, read: %d bytes", pkey, bytesRead);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "stretch settings - time: %d, prepare: %d", stretch_settings.time, stretch_settings.prepare);
}

void stretch_persist_write(int pkey) {
	int bytesWritten = persist_write_data(pkey, &stretch_settings, sizeof(StretchSettings));
	int expectedBytesWritten = sizeof(StretchSettings);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "stretch:persist:write - pkey: %d, expected written: %d bytes. actual written: %d bytes", pkey, expectedBytesWritten, bytesWritten);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "stretch settings - time: %d, prepare: %d", stretch_settings.time, stretch_settings.prepare);
}

static void stretch_persist_reset(int pkey) {
	stretch_settings.time = STRETCH_TIME_DEFAULT;
	stretch_settings.prepare = PREPARE_TIME_DEFAULT;
	stretch_persist_write(pkey);
}

void stretch_persist_migrate(int pkey, int version) {
	switch(version) {
		case 0:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "stretch:persist:migrate - unknown version, resetting settings");
			stretch_persist_reset(pkey);
			break;
		case 6:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "stretch:persist:migrate - v6 to current version");
			#define STRETCH_TIME_PKEY 10
			#define PREPARE_TIME_PKEY 11
			stretch_settings.time = persist_exists(STRETCH_TIME_PKEY) ? persist_read_int(STRETCH_TIME_PKEY) : STRETCH_TIME_DEFAULT;
			stretch_settings.prepare = persist_exists(PREPARE_TIME_PKEY) ? persist_read_int(PREPARE_TIME_PKEY) : PREPARE_TIME_DEFAULT;
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "stretch:persist:migrate - already up to date");
			break;
	}
}

char *stretch_tostring(char *output, int length) {
	char stretch_time_text[7];
	format_time(stretch_time_text, stretch_settings.time);
	
	char prepare_time_text[7];
	format_time(prepare_time_text, stretch_settings.prepare);
	
	snprintf(output, length, "%s+%s", stretch_time_text, prepare_time_text);
	
	return output;
}

int stretch_get_total_time() {
	int total_time = 0;
	
	total_time += stretch_settings.time;
	total_time += stretch_settings.prepare;
	total_time *= SETS;

	return total_time;
}