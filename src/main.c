// Copyright (c) 2015 Marcus Fritzsch
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <pebble.h>

static Window *window;
static GRect bounds;
static GPoint center;

static void draw_hand(GContext *ctx, int angle, int len)
{
   int sin = sin_lookup(angle);
   int cos = cos_lookup(angle);

   GPoint p1 = {
      sin * len / TRIG_MAX_RATIO + center.x,
      -cos * len / TRIG_MAX_RATIO + center.y,
   };

   graphics_draw_line(ctx, center, p1);
}

static void update_effect_layer(Layer *l, GContext *ctx)
{
   time_t t = time(NULL);
   struct tm *tm = localtime(&t);
   int32_t min = tm->tm_min;
   int32_t hr = tm->tm_hour;

   graphics_context_set_stroke_width(ctx, 7);
   graphics_context_set_fill_color(ctx, GColorBlack);
   graphics_fill_rect(ctx, bounds, 0, 0);

   graphics_context_set_fill_color(ctx, GColorLightGray);
   graphics_fill_circle(ctx, GPoint(144 / 2, 18), 3);

   graphics_context_set_stroke_color(ctx, GColorDarkCandyAppleRed);
   draw_hand(ctx, TRIG_MAX_ANGLE * (hr * 60 + min) / (12 * 60), 35);

   graphics_context_set_stroke_color(ctx, GColorWhite);
   draw_hand(ctx, TRIG_MAX_ANGLE * min / 60, 67);
}

static void window_load(Window *window)
{
   Layer *window_layer = window_get_root_layer(window);
   bounds = layer_get_bounds(window_layer);
   center = grect_center_point(&bounds);
   layer_set_update_proc(window_layer, update_effect_layer);
}

static void window_unload(Window *window) {}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
   layer_mark_dirty(window_get_root_layer(window));
}

static void init(void)
{
   window = window_create();
   tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
   window_set_window_handlers(window,
                              (WindowHandlers){
                                 .load = window_load, .unload = window_unload,
                              });
   window_stack_push(window, true);
}

static void deinit(void)
{
   tick_timer_service_unsubscribe();
   window_destroy(window);
}

int main(void)
{
   init();
   app_event_loop();
   deinit();
}
