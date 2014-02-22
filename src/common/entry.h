#pragma once

typedef enum {
	NUMBER,
	TIME
} EntryType;

typedef enum {
	TIME_NORMAL,
	TIME_STEP
} TimeType;

void entry_init_number(char *title, char *format, int step, int *entry);
void entry_init_time(char *title, int *entry);
void entry_init_time_callback(char *title, int *entry, void (*callback)(void));
void entry_init_time_step(char *title, int step, int *entry);
void entry_deinit();