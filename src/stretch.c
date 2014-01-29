#include <pebble.h>

#define LENGTH_DELAY_START 3
#define LENGTH_DELAY 3
#define LENGTH_STRETCH 20

static struct StretchUi {
	Window *window;
	TextLayer *top_text;
	TextLayer *middle_text;
	TextLayer *time_text;	
} ui;

static struct StretchState {
	AppTimer *timer;
	uint16_t running;
	uint16_t round;
	uint16_t round_time;
	uint16_t stretch;
} state;

static BitmapLayer *image_layer;
static GBitmap *image_checkmark;
static GBitmap *image_left;
static GBitmap *image_right;
static GBitmap *image_quad;
static GBitmap *image_hamstring_standing;
static GBitmap *image_lateral_thigh;
static GBitmap *image_inner_thigh;
static GBitmap *image_chest_and_arm;

char buf[6];
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
		}
	}
	
	snprintf(buf, 6, "%d", state.round_time);
	text_layer_set_text(ui.time_text, buf);
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Round: %d Round Time: %d Time display: %s", round, round_time, buf);
	
	state.round_time--;
	
	switch (state.round) {
		case 0:
			text_layer_set_text(ui.middle_text, "Left Side Lunge");
			bitmap_layer_set_bitmap(image_layer, image_left);
			break;
		case 1:
			text_layer_set_text(ui.middle_text, "Right Side Lunge");
			bitmap_layer_set_bitmap(image_layer, image_right);
			break;
		case 2:
			text_layer_set_text(ui.middle_text, "Left Hamstring Standing");
			bitmap_layer_set_bitmap(image_layer, image_hamstring_standing);
			break;
		case 3:
			text_layer_set_text(ui.middle_text, "Right Hamstring Standing");
			break;
		case 4:
			text_layer_set_text(ui.middle_text, "Left Quad");
			bitmap_layer_set_bitmap(image_layer, image_quad);
			break;
		case 5:
			text_layer_set_text(ui.middle_text, "Right Quad");
			break;
		case 6:
			text_layer_set_text(ui.middle_text, "Left Lateral Thigh");
			bitmap_layer_set_bitmap(image_layer, image_lateral_thigh);
			break;
		case 7:
			text_layer_set_text(ui.middle_text, "Right Lateral Thigh");
			break;
		case 8:
			text_layer_set_text(ui.middle_text, "Inner Thigh");
			bitmap_layer_set_bitmap(image_layer, image_inner_thigh);
			break;
		case 9:
			text_layer_set_text(ui.middle_text, "Left Chest and Arm");
			bitmap_layer_set_bitmap(image_layer, image_chest_and_arm);
			break;
		case 10:
			text_layer_set_text(ui.middle_text, "Right Chest and Arm");
			break;
			
		default:
			text_layer_set_text(ui.top_text, "You are done!");
			text_layer_set_text(ui.middle_text, "");
			text_layer_set_text(ui.time_text, "");
			
			bitmap_layer_set_bitmap(image_layer, image_checkmark);
			vibes_double_pulse();
			app_timer_cancel(state.timer);
			break;
	}
	
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
	//text_layer_set_text(ui.top_text, "Stretch Timer");
	text_layer_set_text_alignment(ui.top_text, GTextAlignmentCenter);
	text_layer_set_font(ui.top_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	
	ui.middle_text = text_layer_create((GRect) { .origin = { 0, 28 }, .size = { bounds.size.w, 48 } });
	//text_layer_set_text(ui.middle_text, "Press Select to start");
	text_layer_set_text_alignment(ui.middle_text, GTextAlignmentCenter);
	text_layer_set_overflow_mode(ui.top_text, GTextOverflowModeWordWrap);
	text_layer_set_font(ui.middle_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	
	ui.time_text = text_layer_create((GRect) { .origin = { 5, 82 }, .size = { 80, 49 } });
	text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
	text_layer_set_font(ui.time_text, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	
	// Images
	image_checkmark = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHECKMARK);
	image_left = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SIDE_LUNGE_LEFT);
	image_right = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SIDE_LUNGE_RIGHT);
	image_quad = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_QUAD);
	image_hamstring_standing = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HAMSTRING_STANDING);
	image_lateral_thigh = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LATERAL_THIGH);
	image_inner_thigh = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_INNER_THIGH);
	image_chest_and_arm = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHEST_AND_ARM);
	
	const GPoint center = grect_center_point(&bounds);
	GRect image_frame = (GRect) { .origin = center, .size = image_left->bounds.size };
	image_frame.origin.x = 72;
	image_frame.origin.y = 80;
	
	image_layer = bitmap_layer_create(image_frame);
	
	layer_add_child(window_layer, text_layer_get_layer(ui.middle_text));
	layer_add_child(window_layer, text_layer_get_layer(ui.top_text));
	layer_add_child(window_layer, text_layer_get_layer(ui.time_text));
	layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
	
	reset();
	start();
}

static void window_unload(Window *window) {
	text_layer_destroy(ui.top_text);
	text_layer_destroy(ui.middle_text);
	text_layer_destroy(ui.time_text);	
	
	bitmap_layer_destroy(image_layer);
	gbitmap_destroy(image_checkmark);
	gbitmap_destroy(image_left);
	gbitmap_destroy(image_right);
	gbitmap_destroy(image_quad);
	gbitmap_destroy(image_hamstring_standing);
	gbitmap_destroy(image_lateral_thigh);
	gbitmap_destroy(image_inner_thigh);
	gbitmap_destroy(image_chest_and_arm);
	
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
