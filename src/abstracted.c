/*
	Abstracted
		by Kevin Weber
			8/12/2013
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"


#define MY_UUID { 0x74, 0x30, 0xBC, 0xF6, 0x1C, 0xED, 0x48, 0xA7, 0x8E, 0x23, 0xD4, 0x96, 0x79, 0x52, 0xC1, 0xF8 }
PBL_APP_INFO(MY_UUID,
             "Abstracted", "Kevin Weber",
             1, 0, /* App version */
              RESOURCE_ID_IMAGE_MENU_ICON, /* Change this for watchface icon?? */
             APP_INFO_WATCH_FACE);

Window window;
Layer abst;
Layer date;


GPath cornerPath, pmPath;
static const GPathInfo CORNER_BLOCK_PATH = {
	.num_points = 3,
	.points = (GPoint []) { {72,0}, {144,0}, {144,72} }
};
	
static const GPathInfo PM_BLOCK_PATH = {
	.num_points = 3,
	.points = (GPoint []) { {0,168}, {144,168}, {144,24} }
};



// Adjustable settings
#define SHOW_DATE true
#define HOUR_MARKINGS true
#define DOUBLE_MINUTES false
#define HIGHLIGHT_MINUTES false
#define HIGHLIGHT_INTERVAL 10

// Constants, best to not edit
#define HOUR_BAR_HEIGHT 7
#define MINUTE_SPACING 4
#define PM_SIZE 72

void draw_face( AppContextRef ctx )
{
	PblTm t;
	get_time( &t );
	
	
	// Declare Time Variables
	unsigned short count_m = t.tm_min;
	unsigned short count_h = t.tm_hour%12;
	unsigned short isPM = t.tm_hour<12 ? false : true;
	
	// Set Drawing Colors
	graphics_context_set_fill_color( ctx, GColorBlack );
	graphics_context_set_stroke_color( ctx, GColorBlack );
	graphics_context_set_text_color( ctx, GColorBlack );
	
	
	// Draw PM Shade
	if( isPM )
	{
		// Basically a bunch of lines spaced 1px apart to make a grey shade
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
		
		if( DOUBLE_MINUTES ) graphics_draw_line( ctx, GPoint( 0, y_pos_left-1 ), GPoint( 144, y_pos_left+143 ) );
	}
	
	// Draw Static Block
	gpath_draw_filled( ctx, &cornerPath );
}

void draw_date( AppContextRef ctx )
{
	if( SHOW_DATE )
	{
		//Draw Date
	}
}

void abst_update_callback(Layer *me, GContext* ctx )
{
	(void)me;
	draw_face( ctx );
}

void date_update_callback( Layer *me, GContext* ctx )
{
	(void)me;
	draw_date( ctx );
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;
  layer_mark_dirty( &abst );
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  resource_init_current_app(&APP_RESOURCES);

  window_init(&window, "Abstracted");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorWhite);

  layer_init(&abst, window.layer.frame);
  layer_init(&date, GRect( 0, 108, 36, 36 ) );
  abst.update_proc = abst_update_callback;
  date.update_proc = date_update_callback;

  gpath_init( &cornerPath, &CORNER_BLOCK_PATH );

  layer_add_child(&window.layer, &abst);
  layer_add_child(&window.layer, &date);

  handle_minute_tick(ctx, NULL);
}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };

  app_event_loop(params, &handlers);
}
