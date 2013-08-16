/*
	Abstracted
		by Kevin Weber
			8/12/2013
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "pixeldigit_paths.c"

#define MY_UUID { 0x74, 0x30, 0xBC, 0xF6, 0x1C, 0xED, 0x48, 0xA7, 0x8E, 0x23, 0xD4, 0x96, 0x79, 0x52, 0xC1, 0xF8 }
PBL_APP_INFO(MY_UUID,
             "Abstracted", "Kevin Weber",
             1, 2, /* App version */
             DEFAULT_MENU_ICON, /* Change this for watchface icon?? */
             APP_INFO_WATCH_FACE);

Window window;
Layer abst;
TextLayer cornerDate;

//PblTm Ctime;

GPath cornerPath, pmPath;
static const GPathInfo CORNER_BLOCK_PATH = {
	.num_points = 3,
	.points = (GPoint []) { {72,0}, {144,0}, {144,72} }
};
	
static const GPathInfo PM_BLOCK_PATH = {
	.num_points = 3,
	.points = (GPoint []) { {0,168}, {144,168}, {144,24} }
};

GPoint[12][20] pixelDigitArray;




#define HOUR_BAR_HEIGHT 7
#define MINUTE_SPACING 4
#define PM_SIZE 72

#define HOUR_MARKINGS true
#define DOUBLE_MINUTES false
#define HIGHLIGHT_MINUTES false
#define HIGHLIGHT_INTERVAL 10
	
void draw_face( AppContextRef ctx )
{
	PblTm t;
	get_time( &t );
	
	// Declare/define date string to write to later
	static char date_text[] = "00";
	
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
		if( h > 0 && HOUR_MARKINGS )
		{
			for( int m=0; m<sizeof(pixelDigitArray[h]); m++ )
			{
				graphics_draw_pixel( ctx, pixelDigitArray[h][m] );
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
	
	// Draw Date Text
	
	// "%a %d"
	string_format_time( date_text, sizeof(date_text), "%a %d", &t );
	//text_layer_set_text( &cornerDate, date_text );
	/*graphics_text_draw( ctx, 
				   date_text, 
				   FONT_KEY_ROBOTO_CONDENSED_21, 
				   GRect(108, 0, 144, 36),
				   GTextOverflowModeWordWrap,
				   GTextAlignmentCenter, 
				   NULL
				 );*/
}

void date_update_callback( Layer *me, GContext* ctx )
{
	(void)me;
	// Draw Date Text
	
	// "%a %d"
	//string_format_time( date_text, sizeof(date_text), "%a %d", &t );
	//text_layer_set_text( &cornerDate, date_text );
}


void abst_update_callback(Layer *me, GContext* ctx )
{
	(void)me;
	draw_face( ctx );
}

void init_pixelDigitArray( )
{
	for( int i=0; i<12; i++)
	{
		for( int j=0; j<20; j++)
		{
			switch( i )
			{
				case 0: pixelDigitArray[i][j] = PD_ONE[j]; break;
				case 1: pixelDigitArray[i][j] = PD_TWO[j]; break;
				case 2: pixelDigitArray[i][j] = PD_THREE[j]; break;
				case 3: pixelDigitArray[i][j] = PD_FOUR[j]; break;
				case 4: pixelDigitArray[i][j] = PD_FIVE[j]; break;
				case 5: pixelDigitArray[i][j] = PD_SIX[j]; break;
				case 6: pixelDigitArray[i][j] = PD_SEVEN[j]; break;
				case 7: pixelDigitArray[i][j] = PD_EIGHT[j]; break;
				case 8: pixelDigitArray[i][j] = PD_NINE[j]; break;
				case 9: pixelDigitArray[i][j] = PD_TEN[j]; break;
				case 10: pixelDigitArray[i][j] = PD_ELEVEN[j]; break;
				case 11: pixelDigitArray[i][j] = PD_TWELVE[j]; break;
				default: pixelDigitArray[i][j] = GPoint(0,0);
			}
		}
	}
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;
  layer_mark_dirty( &abst );
  //layer_mark_dirty( &cornerDate );
  //display_time(t->tick_time, ctx);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  init_pixelDigitArray( );

  window_init(&window, "Abstracted");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorWhite);

  layer_init(&abst, window.layer.frame);
  abst.update_proc = abst_update_callback;

  gpath_init( &cornerPath, &CORNER_BLOCK_PATH );

  text_layer_init(&cornerDate, GRect(108, 0, 144, 36));
  //cornerDate.update_proc = date_update_callback;
  text_layer_set_text_alignment(&cornerDate, GTextAlignmentCenter);
  text_layer_set_background_color( &cornerDate, GColorBlack );
  text_layer_set_text_color( &cornerDate, GColorWhite );
  text_layer_set_font(&cornerDate, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));

  layer_add_child(&window.layer, &abst);
  layer_add_child(&window.layer, &cornerDate.layer);

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
