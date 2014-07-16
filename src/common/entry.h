#pragma once

typedef enum {
	NUMBER,
	TIME,
	ENUM
} EntryType;

typedef enum {
	TIME_NORMAL,
	TIME_STEP
} TimeType;

void entry_init_number(char *title, char *format, int step, int *entry);
void entry_init_time(char *title, int *entry);
void entry_init_time_zero_allowed(char *title, int *entry);
void entry_init_time_callback(char *title, int *entry, void (*callback)(void));
void entry_init_time_step(char *title, int step, int *entry);
void entry_init_enum(char *title, char* (*lookup_function)(char *buf, int direction), int choices_count, int *entry);
void entry_deinit();