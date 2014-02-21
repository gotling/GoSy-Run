#pragma once

typedef enum {
	TIME,
	TIME_CALLBACK,
	TIME_STEP,
} EntryType;


void entry_init_number(char *title, char *format, int step, int *entry);
void entry_init_time(char *title, int *entry);
void entry_init_time_callback(char *title, int *entry, void (*callback)(void));
void entry_init_time_step(char *title, int step, int *entry);
void entry_deinit();