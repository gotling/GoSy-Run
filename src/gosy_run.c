#include <pebble.h>
#include "menu.h"

static void init(void) {
  menu_init();
}

static void deinit(void) {
  menu_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();

  return 0;
}
