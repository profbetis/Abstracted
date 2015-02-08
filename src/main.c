#include <pebble.h>
#include "resource_ids.auto.h"
	
// Adjustable settings
#define SHOW_DATE false
#define HOUR_MARKINGS true
#define DOUBLE_MINUTES false
#define HIGHLIGHT_MINUTES false
#define HIGHLIGHT_INTERVAL 10

// Constants, best to not edit
#define HOUR_BAR_HEIGHT 7
#define MINUTE_SPACING 4
#define PM_SIZE 72

	
static Window *window;
static Layer *main_layer;

static GPath *corner_path = NULL;
static const GPathInfo CORNER_BLOCK_PATH = {
	.num_points = 3,
	.points = (GPoint []) { {72,0}, {144,0}, {144,72} }
};	
	


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(main_layer);
}

static void update_proc(Layer *this_layer, GContext *ctx) {
	
	// Init Time Variables
	time_t time_value = time(NULL);
	struct tm* mTime = localtime(&time_value);
	
	unsigned short count_m = mTime->tm_min;
	unsigned short count_h = mTime->tm_hour%12;
	bool isPM = mTime->tm_hour<12 ? false : true;
	
	// Set Drawing Colors
	graphics_context_set_fill_color( ctx, GColorBlack );
	graphics_context_set_stroke_color( ctx, GColorBlack );
	graphics_context_set_text_color( ctx, GColorBlack );
	
	// Draw PM Shade
	if( isPM )
	{
		// A bunch of lines spaced 1px apart to make a checkerboard, which will appear as a grey shade
		for( unsigned short p=1; p<PM_SIZE+1; p++ ){
			graphics_draw_line( ctx, GPoint( p*2, 168 ), GPoint( 144, 24 + p*2 ) );
		}
	}
	
	// Draw Hour Rectangles
	//count_h = 11; // Debug test value for full amount of hours
	graphics_context_set_stroke_color( ctx, GColorWhite );
	
	for( unsigned short h=0; h<count_h; h++)
	{
		graphics_fill_rect( ctx, 
					   GRect( 0, h*HOUR_BAR_HEIGHT*2 + HOUR_BAR_HEIGHT, 144, HOUR_BAR_HEIGHT+1 ), 
					   0, GCornerNone );
		
		/* Ghetto rectangles (pancaked lines); very slow
		for( unsigned short i=0; i<=HOUR_BAR_HEIGHT; i++ ){
			graphics_draw_line( ctx, loc( 0, HOUR_BAR_HEIGHT + (h*HOUR_BAR_HEIGHT*2) + i ), loc( 144, HOUR_BAR_HEIGHT + (h*HOUR_BAR_HEIGHT*2) + i ) );
		}*/
		
		if( HOUR_MARKINGS )
		{
			for( int m=0; m<h; m++ )
			{
				// Draw Hour Markings at the left side of each hour bar
			}
		}
	}
	
	// Draw Minute Lines
	graphics_context_set_stroke_color( ctx, GColorBlack );
	
	for( unsigned short m=0; m<count_m; m++ )
	{
		unsigned short y_pos_left = 168 - MINUTE_SPACING*(m+1);
		
		graphics_draw_line( ctx, GPoint( 0, y_pos_left ), GPoint( 144, y_pos_left+144 ) );
		
		if( (m+1)%HIGHLIGHT_INTERVAL == 0 && HIGHLIGHT_MINUTES )
		{
			graphics_draw_line( ctx, GPoint( 0, y_pos_left-1 ), GPoint( 144, y_pos_left+143 ) );
		}
		
		if( DOUBLE_MINUTES ){ graphics_draw_line( ctx, GPoint( 0, y_pos_left-1 ), GPoint( 144, y_pos_left+143 ) ); }
	}
	
	// Draw Static Block
	gpath_draw_filled( ctx, corner_path );
}

void setup_corner_path(void) {
  corner_path = gpath_create(&CORNER_BLOCK_PATH);
}



static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create Layer
  main_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer,main_layer);

  // Set the update_proc
  layer_set_update_proc(main_layer, update_proc);

  // Subscribe to timer service
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void window_unload(Window *window) {
  // Destroy Layer
  layer_destroy(main_layer);
}



static void init ( ) {
  // Create main Window element and assign to pointer
  window = window_create();
	
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
	window_set_background_color(window, GColorWhite);
    window_set_fullscreen(window, true);

  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
	
  // Set up static shapes
  setup_corner_path();
}

static void deinit() {
    // Destroy Window
    window_destroy(window);
}



int main(void) {
  init();
  app_event_loop();
  deinit();
}