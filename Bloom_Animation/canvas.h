#include <gtkmm.h>
#include <gtk/gtkdrawingarea.h>

static int frame_count{ 0 };
const unsigned frames_per_second = 60;

void Initialize_Brick_Pattern(float x_init, float y_init, float line_width,
    float rect_width, float rect_height, float pattern_width, float pattern_height, float gap);
void Draw_Brick_Pattern(cairo_t* cr, int frameCount);
void Brick_Pattern_Rotate(int frames, int start_frame, float angle);
