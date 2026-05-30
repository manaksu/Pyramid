#include <pebble.h>
#include <ctype.h>

// ── Keys ──────────────────────────────────────────────────
#define KEY_BG_CHOICE    0
#define KEY_STAT_STYLE   1
#define KEY_STAT_1       2
#define KEY_STAT_2       3
#define KEY_STAT_3       4

// ── Pyramid geometry ──────────────────────────────────────
#define S        26
#define TRI_H    22
#define HGAP      6
#define VGAP      5
#define PYR_W   (4*S + 3*HGAP)
#define PYR_H   (4*TRI_H + 3*VGAP)
#define PYR_OX  ((144 - PYR_W) / 2)
#define PYR_OY   4

// ── Char band ─────────────────────────────────────────────
#define CHAR_W   7
#define CHAR_H   9

// ── State ─────────────────────────────────────────────────
static Window *s_window;
static Layer  *s_canvas;
static GFont   s_time_font;
static GFont   s_stat_font;

static int s_bg_choice  = 0;
static int s_stat_style = 0;
static int s_stats[3]   = {0,1,2};
static int s_batt_pct   = 100;
static int s_hour       = 0;
static int s_minute     = 0;
static int s_steps      = 0;
static int s_hr         = 0;
static int s_sleep_hr   = 0;
static int s_sleep_min  = 0;
static int s_calories   = 0;
static int s_dist_m     = 0;

// ── PRNG ──────────────────────────────────────────────────
static uint32_t s_prng = 0;
static void     prng_seed(uint32_t s) { s_prng = s; }
static uint32_t prng_next() { s_prng = s_prng*1664525+1013904223; return s_prng; }
static int      prng_range(int n) { return n>0?(int)(prng_next()%(uint32_t)n):0; }

// ── Date strings ──────────────────────────────────────────
static char s_day_str[4];
static char s_mon_str[6];
static char s_wday_str[12];

static void build_date_strings() {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  strftime(s_day_str,  sizeof(s_day_str),  "%d", t);
  strftime(s_mon_str,  sizeof(s_mon_str),  "%b", t);
  strftime(s_wday_str, sizeof(s_wday_str), "%A", t);
}

// ── Colors ────────────────────────────────────────────────
static GColor col_bg()    { return s_bg_choice==1?GColorBlack:s_bg_choice==2?GColorWhite:GColorWhite; }
static GColor col_text()  { return s_bg_choice==1?GColorWhite:GColorBlack; }
static GColor col_muted() { return s_bg_choice==1?GColorDarkGray:GColorLightGray; }

// ── Fill order ────────────────────────────────────────────
static const int FILL_ORDER[10] = {9,7,8,4,5,6,0,1,2,3};

// ── Noise chars ───────────────────────────────────────────
static const char NOISE[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
#define NOISE_LEN 62
static char noise_char() { return NOISE[prng_range(NOISE_LEN)]; }

// ── Pyramid x range at pixel y ────────────────────────────
static void pyr_x_at_y(int py, int *lx, int *rx) {
  int y2 = PYR_OY;
  int rc[4]={4,3,2,1};
  for (int r=0;r<4;r++) {
    if (py < y2+TRI_H) {
      int rw=rc[r]*S+(rc[r]-1)*HGAP;
      int rpx=PYR_OX+(PYR_W-rw)/2;
      *lx=rpx; *rx=rpx+rw; return;
    }
    y2+=TRI_H+VGAP;
  }
  *lx=PYR_OX; *rx=PYR_OX+PYR_W;
}

// ── Tile system ───────────────────────────────────────────
#define MAX_TILES 220
typedef struct { int16_t x,y; char ch; bool hi; } Tile;
static Tile s_tiles[MAX_TILES];
static int  s_tile_count=0;

static void build_tiles(uint32_t seed) {
  prng_seed(seed);
  s_tile_count=0;

  char left_date[12], right_date[12];
  snprintf(left_date,  sizeof(left_date),  "%s%s", s_day_str, s_mon_str);
  snprintf(right_date, sizeof(right_date), "%s",   s_wday_str);
  int llen=(int)strlen(left_date), rlen=(int)strlen(right_date);

  int rows=PYR_H/CHAR_H, cols=144/CHAR_W;

  // collect left/right slots
  typedef struct{int16_t row,col;} Slot;
  static Slot lslots[110], rslots[110];
  int lsc=0,rsc=0;
  for (int row=0;row<rows;row++) {
    int py=PYR_OY+row*CHAR_H+CHAR_H/2, plx,prx;
    pyr_x_at_y(py,&plx,&prx);
    for (int col=0;col<cols;col++) {
      int cx=col*CHAR_W;
      if (cx+CHAR_W<=plx && lsc<110) lslots[lsc++]=(Slot){row,col};
      else if (cx>=prx   && rsc<110) rslots[rsc++]=(Slot){row,col};
    }
  }

  // pick highlight positions — evenly spaced for readability
  int lhi[12], rhi[12];
  for (int i=0;i<llen&&i<12;i++) lhi[i]=(lsc>0)?((i+1)*lsc/(llen+1)):0;
  for (int i=0;i<rlen&&i<12;i++) rhi[i]=(rsc>0)?((i+1)*rsc/(rlen+1)):0;

  // left tiles
  int di=0;
  for (int i=0;i<lsc&&s_tile_count<MAX_TILES;i++) {
    bool hi=(di<llen && lhi[di]==i);
    char ch=hi?left_date[di++]:noise_char();
    s_tiles[s_tile_count++]=(Tile){lslots[i].col*CHAR_W, PYR_OY+lslots[i].row*CHAR_H, ch, hi};
  }
  // right tiles
  di=0;
  for (int i=0;i<rsc&&s_tile_count<MAX_TILES;i++) {
    bool hi=(di<rlen && rhi[di]==i);
    char ch=hi?right_date[di++]:noise_char();
    s_tiles[s_tile_count++]=(Tile){rslots[i].col*CHAR_W, PYR_OY+rslots[i].row*CHAR_H, ch, hi};
  }
}

// ── Canvas draw ───────────────────────────────────────────
static void canvas_draw(Layer *layer, GContext *ctx) {
  GRect bounds=layer_get_bounds(layer);

  // background
  graphics_context_set_fill_color(ctx, col_bg());
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  GFont small_fnt = fonts_get_system_font(FONT_KEY_GOTHIC_09);
  GFont stat_fnt  = s_stat_font ? s_stat_font : fonts_get_system_font(FONT_KEY_GOTHIC_14);
  GFont time_fnt  = s_time_font ? s_time_font : fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);

  // ── Char band ──────────────────────────────────────────
  char buf[2]={0,0};
  for (int i=0;i<s_tile_count;i++) {
    buf[0]=s_tiles[i].ch;
    graphics_context_set_text_color(ctx, s_tiles[i].hi?col_text():col_muted());
    graphics_draw_text(ctx,buf,small_fnt,
      GRect(s_tiles[i].x,s_tiles[i].y,CHAR_W+2,CHAR_H+2),
      GTextOverflowModeWordWrap,GTextAlignmentLeft,NULL);
  }

  // ── Pyramid ────────────────────────────────────────────
  int filled=(s_batt_pct+9)/10;
  int rc[4]={4,3,2,1};
  int ti=0, y=PYR_OY;
  for (int r=0;r<4;r++) {
    int count=rc[r];
    int rw=count*S+(count-1)*HGAP;
    int rx=PYR_OX+(PYR_W-rw)/2;
    for (int i=0;i<count;i++) {
      int cx=rx+i*(S+HGAP)+S/2;
      bool is_f=false;
      for (int f=0;f<filled;f++) if(FILL_ORDER[f]==ti){is_f=true;break;}
      GPathInfo info={.num_points=3,.points=(GPoint[]){{cx-S/2,y},{cx+S/2,y},{cx,y+TRI_H}}};
      GPath *path=gpath_create(&info);
      graphics_context_set_fill_color(ctx,is_f?col_text():col_muted());
      gpath_draw_filled(ctx,path);
      graphics_context_set_stroke_color(ctx,col_text());
      graphics_context_set_stroke_width(ctx,1);
      gpath_draw_outline(ctx,path);
      gpath_destroy(path);
      ti++;
    }
    y+=TRI_H+VGAP;
  }

  // ── Time HH / MM ───────────────────────────────────────
  // TIP_Y=107  BOT_H=61px
  // sz=28: actual line_h=27px, 2 lines=54px fits ✓
  // sz=11 stats: lh=8px, 3 lines=24px fits ✓ side-by-side
  #define TIME_LH 27
  #define STAT_LH  9
  #define TIME_W  36   // measured width of "00" at sz=28

  int bottom_y = PYR_OY + PYR_H;   // 107 — pin directly, no gap
  char hh[3],mm[3];
  snprintf(hh,3,"%02d",s_hour);
  snprintf(mm,3,"%02d",s_minute);

  graphics_context_set_text_color(ctx,col_text());
  graphics_draw_text(ctx,hh,time_fnt,GRect(4,bottom_y,          TIME_W+4,TIME_LH+4),GTextOverflowModeWordWrap,GTextAlignmentLeft,NULL);
  graphics_draw_text(ctx,mm,time_fnt,GRect(4,bottom_y+TIME_LH,  TIME_W+4,TIME_LH+4),GTextOverflowModeWordWrap,GTextAlignmentLeft,NULL);

  int stats_x = 4+TIME_W+6;
  int stats_w = 144-stats_x-2;

  // ── Stats ──────────────────────────────────────────────
  const char *sl[]={"STP","HR","SL","CAL","DST"};
  const char *fl[]={"Steps","Heart","Sleep","Cal","Dist"};
  const char **lbls=(s_stat_style==0)?sl:fl;

  char sv[3][24];
  for (int i=0;i<3;i++) {
    switch(s_stats[i]) {
      case 0: snprintf(sv[i],24,"%d",s_steps); break;
      case 1: snprintf(sv[i],24,"%d BPM",s_hr); break;
      case 2: snprintf(sv[i],24,"%dh%02dm",s_sleep_hr,s_sleep_min); break;
      case 3: snprintf(sv[i],24,"%d",s_calories); break;
      case 4: snprintf(sv[i],24,"%d.%dkm",s_dist_m/1000,(s_dist_m%1000)/100); break;
      default: snprintf(sv[i],24,"--"); break;
    }
  }

  // vertically center 3 stat lines in BOT_H (61px)
  int stats_total_h = STAT_LH * 3;
  int s_start = bottom_y + (61 - stats_total_h) / 2;
  for (int i=0;i<3;i++) {
    int sy=s_start+i*STAT_LH;
    char line[32];
    snprintf(line,sizeof(line),"%s:%s",lbls[s_stats[i]],sv[i]);
    graphics_context_set_text_color(ctx,col_text());
    graphics_draw_text(ctx,line,stat_fnt,GRect(stats_x,sy,stats_w,STAT_LH+2),GTextOverflowModeWordWrap,GTextAlignmentLeft,NULL);
  }
}

// ── Health ────────────────────────────────────────────────
static void update_health() {
#if defined(PBL_HEALTH)
  time_t start=time_start_of_today(), now=time(NULL);
  HealthServiceAccessibilityMask m;
  m=health_service_metric_accessible(HealthMetricStepCount,start,now);
  if(m&HealthServiceAccessibilityMaskAvailable) s_steps=(int)health_service_sum(HealthMetricStepCount,start,now);
  m=health_service_metric_accessible(HealthMetricHeartRateBPM,start,now);
  if(m&HealthServiceAccessibilityMaskAvailable) s_hr=(int)health_service_sum_averaged(HealthMetricHeartRateBPM,start,now,HealthServiceTimeScopeOnce);
  m=health_service_metric_accessible(HealthMetricSleepSeconds,start,now);
  if(m&HealthServiceAccessibilityMaskAvailable){int s=(int)health_service_sum(HealthMetricSleepSeconds,start,now);s_sleep_hr=s/3600;s_sleep_min=(s%3600)/60;}
  m=health_service_metric_accessible(HealthMetricActiveKCalories,start,now);
  if(m&HealthServiceAccessibilityMaskAvailable) s_calories=(int)health_service_sum(HealthMetricActiveKCalories,start,now);
  m=health_service_metric_accessible(HealthMetricWalkedDistanceMeters,start,now);
  if(m&HealthServiceAccessibilityMaskAvailable) s_dist_m=(int)health_service_sum(HealthMetricWalkedDistanceMeters,start,now);
#endif
}

static void health_handler(HealthEventType event, void *ctx) {
  update_health();
  layer_mark_dirty(s_canvas);
}

static void batt_handler(BatteryChargeState state) {
  s_batt_pct=state.charge_percent;
  layer_mark_dirty(s_canvas);
}

static void tick_handler(struct tm *t, TimeUnits units) {
  s_hour=t->tm_hour; s_minute=t->tm_min;
  if(units&HOUR_UNIT){
    build_date_strings();
    build_tiles((uint32_t)(t->tm_hour*1000+t->tm_mday*100));
  }
  layer_mark_dirty(s_canvas);
}

static void inbox_received(DictionaryIterator *iter, void *ctx) {
  Tuple *t;
  t=dict_find(iter,KEY_BG_CHOICE);   if(t){s_bg_choice=  (int)t->value->int32;persist_write_int(KEY_BG_CHOICE,  s_bg_choice);}
  t=dict_find(iter,KEY_STAT_STYLE);  if(t){s_stat_style= (int)t->value->int32;persist_write_int(KEY_STAT_STYLE,s_stat_style);}
  t=dict_find(iter,KEY_STAT_1);      if(t){s_stats[0]=   (int)t->value->int32;persist_write_int(KEY_STAT_1,    s_stats[0]);}
  t=dict_find(iter,KEY_STAT_2);      if(t){s_stats[1]=   (int)t->value->int32;persist_write_int(KEY_STAT_2,    s_stats[1]);}
  t=dict_find(iter,KEY_STAT_3);      if(t){s_stats[2]=   (int)t->value->int32;persist_write_int(KEY_STAT_3,    s_stats[2]);}
  build_date_strings();
  time_t now=time(NULL); struct tm *tm=localtime(&now);
  build_tiles((uint32_t)(tm->tm_hour*1000+tm->tm_mday*100+prng_next()%97));
  layer_mark_dirty(s_canvas);
}

// ── Window lifecycle ──────────────────────────────────────
static void window_load(Window *window) {
  Layer *root=window_get_root_layer(window);
  GRect bounds=layer_get_bounds(root);

  s_bg_choice  =persist_exists(KEY_BG_CHOICE)  ?persist_read_int(KEY_BG_CHOICE)  :0;
  s_stat_style =persist_exists(KEY_STAT_STYLE) ?persist_read_int(KEY_STAT_STYLE) :0;
  s_stats[0]   =persist_exists(KEY_STAT_1)     ?persist_read_int(KEY_STAT_1)     :0;
  s_stats[1]   =persist_exists(KEY_STAT_2)     ?persist_read_int(KEY_STAT_2)     :1;
  s_stats[2]   =persist_exists(KEY_STAT_3)     ?persist_read_int(KEY_STAT_3)     :2;

  s_canvas=layer_create(bounds);
  layer_set_update_proc(s_canvas,canvas_draw);
  layer_add_child(root,s_canvas);

  s_time_font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SG_MEDIUM_28));
  s_stat_font=fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SG_REGULAR_11));

  build_date_strings();
  time_t now=time(NULL); struct tm *t=localtime(&now);
  s_hour=t->tm_hour; s_minute=t->tm_min;
  build_tiles((uint32_t)(t->tm_hour*1000+t->tm_mday*100));

  s_batt_pct=battery_state_service_peek().charge_percent;
  battery_state_service_subscribe(batt_handler);
  update_health();
#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler,NULL);
#endif
  layer_mark_dirty(s_canvas);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  if(s_time_font) fonts_unload_custom_font(s_time_font);
  if(s_stat_font) fonts_unload_custom_font(s_stat_font);
  battery_state_service_unsubscribe();
#if defined(PBL_HEALTH)
  health_service_events_unsubscribe();
#endif
}

static void init() {
  s_window=window_create();
  window_set_window_handlers(s_window,(WindowHandlers){.load=window_load,.unload=window_unload});
  window_stack_push(s_window,true);
  app_message_open(128,64);
  app_message_register_inbox_received(inbox_received);
  tick_timer_service_subscribe(MINUTE_UNIT|HOUR_UNIT,tick_handler);
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(){init();app_event_loop();deinit();}
