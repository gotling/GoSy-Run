#include <pebble.h>

#define LENGTH_DELAY_START 3
#define LENGTH_DELAY 3
#define LENGTH_STRETCH 20

static struct StretchUi {
	Window *window;
	TextLayer *top_text;
	TextLayer *middle_text;
	TextLayer *time_text;
	BitmapLayer *image;
} ui;

static struct StretchState {
	AppTimer *timer;
	uint16_t running;
	uint16_t round;
	uint16_t round_time;
	uint16_t stretch;
} state;

static struct StretchImages {
	GBitmap *checkmark;
	GBitmap *side_lunge;
	GBitmap *quad;
	GBitmap *hamstring_standing;
	GBitmap *lateral_thigh;
	GBitmap *inner_thigh;
	GBitmap *chest_and_arm;
} image;

char buf[6];

static void update_ui() {
	switch (state.round) {
		case 0:
			text_layer_set_text(ui.middle_text, "Left Side Lunge");
			bitmap_layer_set_bitmap(ui.image, image.side_lunge);
			break;
		case 1:
			text_layer_set_text(ui.middle_text, "Right Side Lunge");
			break;
		case 2:
			text_layer_set_text(ui.middle_text, "Left Hamstring Standing");
			bitmap_layer_set_bitmap(ui.image, image.hamstring_standing);
			break;
		case 3:
			text_layer_set_text(ui.middle_text, "Right Hamstring Standing");
			break;
		case 4:
			text_layer_set_text(ui.middle_text, "Left Quad");
			bitmap_layer_set_bitmap(ui.image, image.quad);
			break;
		case 5:
			text_layer_set_text(ui.middle_text, "Right Quad");
			break;
		case 6:
			text_layer_set_text(ui.middle_text, "Left Lateral Thigh");
			bitmap_layer_set_bitmap(ui.image, image.lateral_thigh);
			break;
		case 7:
			text_layer_set_text(ui.middle_text, "Right Lateral Thigh");
			break;
		case 8:
			text_layer_set_text(ui.middle_text, "Inner Thigh");
			bitmap_layer_set_bitmap(ui.image, image.inner_thigh);
			break;
		case 9:
			text_layer_set_text(ui.middle_text, "Left Chest and Arm");
			bitmap_layer_set_bitmap(ui.image, image.chest_and_arm);
			break;
		case 10:
			text_layer_set_text(ui.middle_text, "Right Chest and Arm");
			break;
		case 11:
			text_layer_set_text(ui.middle_text, "Left Calf");
			break;
		case 12:
			text_layer_set_text(ui.middle_text, "Right Calf");
			
		default:
			text_layer_set_text(ui.top_text, "You are done!");
			text_layer_set_text(ui.middle_text, "");
			text_layer_set_text(ui.time_text, "");
			
			bitmap_layer_set_bitmap(ui.image, image.checkmark);
			
			vibes_double_pulse();
			app_timer_cancel(state.timer);
			break;
	}
}

static void timer_callback(void *data) {
	state.timer = app_timer_register(1000, timer_callback, NULL);
	
	if (state.round_time == 0) {
		if (state.stretch == 0) {
			state.round_time = LENGTH_STRETCH;
			vibes_short_pulse();
			text_layer_set_text(ui.top_text, "Stretch");
			state.stretch = 1;
		} else {
			state.round++;
			state.round_time = LENGTH_DELAY;      
			vibes_long_pulse();
			text_layer_set_text(ui.top_text, "Prepare");
			state.stretch = 0;
			
			update_ui();
		}
	}
	
	snprintf(buf, 6, "%d", state.round_time);
	text_layer_set_text(ui.time_text, buf);
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Round: %d Round Time: %d Time display: %s", round, round_time, buf);
	
	state.round_time--;
}

static void start() {
	state.running = 1;
	
	vibes_short_pulse();
	
	if (state.stretch) {
		text_layer_set_text(ui.top_text, "Stretch");
	} else {
		text_layer_set_text(ui.top_text, "Prepare");
	}
	
	timer_callback(NULL);
}

static void pause() {
	app_timer_cancel(state.timer);
	
	state.running = 0;
	
	text_layer_set_text(ui.top_text, "PAUSED");
}

static void reset() {
	app_timer_cancel(state.timer);
	
	state.running = 0;
	state.round = 0;
	state.round_time = LENGTH_DELAY;
	state.stretch = 0;
	
	update_ui();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (state.running) {
		pause();
	} else {
		start();
	}
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	ui.top_text = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text_alignment(ui.top_text, GTextAlignmentCenter);
	text_layer_set_font(ui.top_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.top_text));
	
	ui.middle_text = text_layer_create((GRect) { .origin = { 0, 28 }, .size = { bounds.size.w, 48 } });
	text_layer_set_text_alignment(ui.middle_text, GTextAlignmentCenter);
	text_layer_set_overflow_mode(ui.top_text, GTextOverflowModeWordWrap);
	text_layer_set_font(ui.middle_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.middle_text));
	
	ui.time_text = text_layer_create((GRect) { .origin = { 5, 82 }, .size = { 80, 49 } });
	text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
	text_layer_set_font(ui.time_text, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(window_layer, text_layer_get_layer(ui.time_text));
	
	image.checkmark = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHECKMARK);
	image.side_lunge = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SIDE_LUNGE_LEFT);
	image.quad = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_QUAD);
	image.hamstring_standing = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HAMSTRING_STANDING);
	image.lateral_thigh = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LATERAL_THIGH);
	image.inner_thigh = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_INNER_THIGH);
	image.chest_and_arm = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHEST_AND_ARM);
	
	GRect image_frame = (GRect) { .origin = grect_center_point(&bounds), .size = image.checkmark->bounds.size };
	image_frame.origin.x = 72;
	image_frame.origin.y = 80;
	ui.image = bitmap_layer_create(image_frame);
	layer_add_child(window_layer, bitmap_layer_get_layer(ui.image));
	
	reset();
	start();
}

static void window_unload(Window *window) {
	text_layer_destroy(ui.top_text);
	text_layer_destroy(ui.middle_text);
	text_layer_destroy(ui.time_text);	
	bitmap_layer_destroy(ui.image);
	
	gbitmap_destroy(image.checkmark);
	gbitmap_destroy(image.side_lunge);
	gbitmap_destroy(image.quad);
	gbitmap_destroy(image.hamstring_standing);
	gbitmap_destroy(image.lateral_thigh);
	gbitmap_destroy(image.inner_thigh);
	gbitmap_destroy(image.chest_and_arm);
	
	reset();
	window_destroy(window);
}

void stretch_init(void) {
	ui.window = window_create();
	window_set_click_config_provider(ui.window, click_config_provider);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

void stretch_deinit(void) {
	window_destroy(ui.window);
}
