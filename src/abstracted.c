/*
	Abstracted
		by Kevin Weber
			8/12/2013
 */

#include "pebble_os.h"
#include "pebble_app.h"
//#include "pebble_fonts.h"

#define MY_UUID { 0x74, 0x30, 0xBC, 0xF6, 0x1C, 0xED, 0x48, 0xA7, 0x8E, 0x23, 0xD4, 0x96, 0x79, 0x52, 0xC1, 0xF8 }
PBL_APP_INFO(MY_UUID,
             "Abstracted", "Kevin Weber",
             0, 4, /* App version */
             DEFAULT_MENU_ICON, /* Change this for watchface icon?? */
             APP_INFO_WATCH_FACE);

Window window;
Layer abst;

PblTm Ctime;




bool get_pm( unsigned short hour )
{
	return hour<12 ? false : true;
}



GPoint loc( unsigned short x, unsigned short y ){
	return GPoint( x, y );
}

/*
GSize scale( unsigned short x, unsigned short y){
	return GSize( x, y );
}

GRect area( GPoint corner, GSize otherCorner ){
	return GRect( corner, otherCorner );
}*/



#define HOUR_BAR_HEIGHT 6
#define MINUTE_SPACING 4
#define PM_SIZE 72
	
void draw_face( unsigned short count_m, unsigned short count_h, bool isPM, AppContextRef ctx )
{
	// Set Drawing Colors
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_context_set_stroke_color(ctx, GColorBlack);
	
	graphics_draw_pixel( ctx, loc( 20, 20 ) );
	graphics_draw_pixel( ctx, loc( 21, 20 ) );
	graphics_draw_pixel( ctx, loc( 22, 20 ) );
	graphics_draw_pixel( ctx, loc( 23, 21 ) );
	graphics_draw_pixel( ctx, loc( 24, 22 ) );
	
	// Draw Static Block
	for( unsigned short b=0; b<72; b++ ){
		graphics_draw_line( ctx, loc( 144-b, 0 ), loc( 144, b ) );
	}
	
	// Draw PM Shade
	if( isPM )
	{
		for( unsigned short p=1; p<PM_SIZE+1; p++ ){
			graphics_draw_line( ctx, loc( p*2, 168 ), loc( 144, 168 - p*2 ) );
			//line( width, height - i*2, 0, width + height - i*2 );
		}
	}
	
	// Draw Hour Rectangles
	for( unsigned short h=0; h<count_h; h++){
		//rect( 0, h*barHeight*2 + startHeight, width, h*barHeight*2 + barHeight + startHeight );
		/*void graphics_fill_rect( ctx, 
								
						   area( loc( 0, HOUR_BAR_HEIGHT + (h*HOUR_BAR_HEIGHT*2) ),
						   	    scale( 144, HOUR_BAR_HEIGHT ) ), 
								
						   0, GCornerNone );*/
		for( unsigned short i=0; i<=HOUR_BAR_HEIGHT; i++ ){
			graphics_draw_line( ctx, loc( 0, HOUR_BAR_HEIGHT + (h*HOUR_BAR_HEIGHT*2) + i ), loc( 144, HOUR_BAR_HEIGHT + (h*HOUR_BAR_HEIGHT*2) + i ) );
		}
	}
	
	// Draw Minute Lines
	for( unsigned short m=0; m<count_m; m++ )
	{
		unsigned short y_pos_left = 168 - MINUTE_SPACING*(m+1);
		graphics_draw_line( ctx, loc( 0, y_pos_left ), loc( 144, y_pos_left+144 ) );
		//line( 0, 168 - minute_spacing*(m+1), 144, 168 - minute_spacing*(m+1) + 144 );
	}
}


void abst_update_callback(Layer *me, GContext* ctx )
{
	(void)me;
	PblTm Ctime;
	get_time( &Ctime );
	
	draw_face( Ctime.tm_min, 
			Ctime.tm_hour%12, 
			Ctime.tm_hour<12 ? false : true, 
			ctx );
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  (void)ctx;
  layer_mark_dirty(&abst);
  //display_time(t->tick_time, ctx);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Abstracted");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorWhite);

  layer_init(&abst, window.layer.frame);
  abst.update_proc = abst_update_callback; // Pointer problem here
  layer_add_child(&window.layer, &abst);

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
