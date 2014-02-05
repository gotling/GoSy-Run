#include <pebble.h>
#include "tools.h"

char *format_time(char *formated_time, int seconds) {
	if (seconds < 60) {
		snprintf(formated_time, 3, "%d", seconds);
	} else {
		snprintf(formated_time, 7, "%d:%02d", seconds / 60, (seconds % 60));
	}
	
	return formated_time;
}

char *format_time_long(char *formated_time, int seconds) {
	if (seconds < 60) {
		snprintf(formated_time, 11, "%d seconds", seconds);
	} else {
		snprintf(formated_time, 7, "%d:%02d", seconds / 60, (seconds % 60));
	}
	
	return formated_time;
}