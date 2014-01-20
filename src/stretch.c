#include <pebble.h>

static Window *window;
static TextLayer *text_layer_top;
static TextLayer *text_layer_middle;
static TextLayer *time_layer;
static AppTimer *timer;

static BitmapLayer *image_layer;
static GBitmap *image_checkmark;
static GBitmap *image_left;
static GBitmap *image_right;
static GBitmap *image_quad;
static GBitmap *image_hamstring_standing;
static GBitmap *image_lateral_thigh;
static GBitmap *image_inner_thigh;
static GBitmap *image_chest_and_arm;

const uint16_t LENGTH_DELAY = 3;
const uint16_t LENGTH_STRETCH = 5;
static uint16_t round = 0;
static uint16_t round_time = 3;
static uint16_t stretch = 0;

char buf[6];
static void timer_callback(void *data) {
	timer = app_timer_register(1000, timer_callback, NULL);
	
	if (round_time == 0) {
		if (stretch == 0) {
			round_time = LENGTH_STRETCH;
			vibes_short_pulse();
			text_layer_set_text(text_layer_top, "Stretch");
			stretch = 1;
		} else {
			round++;
			round_time = LENGTH_DELAY;      
			vibes_long_pulse();
			text_layer_set_text(text_layer_top, "Prepare");
			stretch = 0;
		}
	}
	
	snprintf(buf, 6, "%d", round_time);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Round: %d Round Time: %d Time display: %s", round, round_time, buf);
	text_layer_set_text(time_layer, buf);
	
	round_time--;
	
	switch (round) {
		case 0:
			text_layer_set_text(text_layer_middle, "Left Side Lunge");
			bitmap_layer_set_bitmap(image_layer, image_left);
			break;
		case 1:
			text_layer_set_text(text_layer_middle, "Right Side Lunge");
			bitmap_layer_set_bitmap(image_layer, image_right);
			break;
		case 2:
			text_layer_set_text(text_layer_middle, "Left Hamstring Standing");
			bitmap_layer_set_bitmap(image_layer, image_hamstring_standing);
			break;
		case 3:
			text_layer_set_text(text_layer_middle, "Right Hamstring Standing");
			break;
		case 4:
			text_layer_set_text(text_layer_middle, "Left Quad");
			bitmap_layer_set_bitmap(image_layer, image_quad);
			break;
		case 5:
			text_layer_set_text(text_layer_middle, "Right Quad");
			break;
		case 6:
			text_layer_set_text(text_layer_middle, "Left Lateral Thigh");
			bitmap_layer_set_bitmap(image_layer, image_lateral_thigh);
			break;
		case 7:
			text_layer_set_text(text_layer_middle, "Right Lateral Thigh");
			break;
		case 8:
			text_layer_set_text(text_layer_middle, "Inner Thigh");
			bitmap_layer_set_bitmap(image_layer, image_inner_thigh);
			break;
		case 9:
			text_layer_set_text(text_layer_middle, "Left Chest and Arm");
			bitmap_layer_set_bitmap(image_layer, image_chest_and_arm);
			break;
		case 10:
			text_layer_set_text(text_layer_middle, "Right Chest and Arm");
			break;
			
		default:
			text_layer_set_text(text_layer_top, "You are done!");
			text_layer_set_text(text_layer_middle, "");
			text_layer_set_text(time_layer, "");
			
			bitmap_layer_set_bitmap(image_layer, image_checkmark);
			vibes_double_pulse();
			app_timer_cancel(timer);
			break;
	}
	
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	vibes_short_pulse();
	text_layer_set_text(text_layer_top, "Prepare");
	timer_callback(NULL);
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	text_layer_top = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text(text_layer_top, "GoSy Run");
	text_layer_set_text_alignment(text_layer_top, GTextAlignmentCenter);
	text_layer_set_font(text_layer_top, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	
	text_layer_middle = text_layer_create((GRect) { .origin = { 0, 28 }, .size = { bounds.size.w, 48 } });
	text_layer_set_text(text_layer_middle, "Press Select to start Stretch");
	text_layer_set_text_alignment(text_layer_middle, GTextAlignmentCenter);
	text_layer_set_overflow_mode(text_layer_top, GTextOverflowModeWordWrap);
	text_layer_set_font(text_layer_middle, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	
	time_layer = text_layer_create((GRect) { .origin = { 5, 82 }, .size = { 80, 49 } });
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	
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
	
	layer_add_child(window_layer, text_layer_get_layer(text_layer_middle));
	layer_add_child(window_layer, text_layer_get_layer(text_layer_top));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
}

static void window_unload(Window *window) {
	text_layer_destroy(text_layer_top);
	text_layer_destroy(text_layer_middle);
	text_layer_destroy(time_layer);	
	
	bitmap_layer_destroy(image_layer);
	gbitmap_destroy(image_checkmark);
	gbitmap_destroy(image_left);
	gbitmap_destroy(image_right);
	gbitmap_destroy(image_quad);
	gbitmap_destroy(image_hamstring_standing);
	gbitmap_destroy(image_lateral_thigh);
	gbitmap_destroy(image_inner_thigh);
	gbitmap_destroy(image_chest_and_arm);
}

static void init(void) {
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	window_stack_push(window, animated);
}

static void deinit(void) {
	window_destroy(window);
}

int main(void) {
	init();
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
	
	app_event_loop();
	deinit();
}
