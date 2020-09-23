#include "canvas.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Pixel {
	Pixel(float x, float y) :x{ x }, y{ y }{};
	float x, y;
	void Draw(cairo_t* cr) { cairo_rectangle(cr, x, y, 1, 1); cairo_fill(cr); }
};

struct Line {
	Line(std::vector<Pixel> pixels, float xc, float yc) :pixels{ pixels }, xc{ xc }, yc{ yc } {};
	std::vector<Pixel> pixels;
	float xc, yc;
};

std::vector<Line> lines;
bool is_rotating = false;
bool is_scaling = false;
float rotate_inc = 0.f;
float scale_inc = 0.f;
int start_frame_scale;
int start_frame_rotate;
int total_frames_rotate;
int total_frames_scale;
float current_rotation;
float current_scale;

void Initialize_Brick_Pattern(float x_init, float y_init, float line_width,
	float rect_width, float rect_height, float pattern_width, float pattern_height, float gap) {
	std::vector<Pixel> pixels;
	for (float x = x_init; x <= pattern_width; x += rect_width ) {
		for (float y = y_init; y < pattern_height; y += rect_height ) {
			for (int i = 0; i < rect_width - 2.f * gap; i++) {
				for (int j = 0; j < line_width; j++) {
					pixels.push_back(Pixel(x + gap + i, y + j));
				}
			}
			lines.push_back(Line(pixels, x + rect_width / 2, y + line_width / 2));
			pixels.clear();
			for (int i = 0; i < rect_width - 2.f * gap; i++) {
				for (int j = 0; j < line_width; j++) {
					pixels.push_back(Pixel(x + rect_width + gap + i, y + j));
				}
			}
			lines.push_back(Line(pixels, x + rect_width + rect_width / 2, y + line_width / 2));
			pixels.clear();
		}
	}
	for (float x = x_init; x <= pattern_width; x += rect_width ) {
		for (float y = y_init; y < pattern_height; y += rect_height ) {
			std::vector<Pixel> pixels;
			for (int i = 0; i < rect_height - 2.f * gap; i++) {
				for (int j = 0; j < line_width; j++) {
					pixels.push_back(Pixel(x + j, y + gap + i));
				}
			}
			lines.push_back(Line(pixels, x + line_width / 2, y + rect_height / 2));
			pixels.clear();
			for (int i = 0; i < rect_height - 2.f * gap; i++) {
				for (int j = 0; j < line_width; j++) {
					pixels.push_back(Pixel(x + j, y + rect_height + gap + i));
				}
			}
			lines.push_back(Line(pixels, x + line_width / 2, y + rect_height + rect_height / 2));
			pixels.clear();
		}
	}

}
void Draw_Brick_Pattern(cairo_t* cr, int frameCount) {
	for (Line line: lines) {
		cairo_save(cr);
		if (is_rotating) {
			if (frameCount - start_frame_rotate > total_frames_rotate) {
				is_rotating = false;
			}
			else {
				current_rotation += rotate_inc;
			}
		}
		if (is_scaling) {
			if (frameCount - start_frame_scale > total_frames_scale) {
				is_rotating = false;
			}
			else {
				current_scale += scale_inc;
			}
		}
		cairo_translate(cr, line.xc, line.yc);
		cairo_scale(cr, current_scale, current_scale);
		cairo_rotate(cr, current_rotation);
		cairo_translate(cr, -line.xc, -line.yc);
		for (Pixel pixel : line.pixels) {
			pixel.Draw(cr);
		}
		cairo_restore(cr);
	}
}

void Brick_Pattern_Rotate(int frames, int starting_frame, float angle) {
	total_frames_rotate = frames;
	start_frame_rotate = starting_frame;
	is_rotating = true;
	rotate_inc = angle / frames;
}
void Brick_Pattern_Scale(int frames, int starting_frame, float scale) {
	total_frames_scale = frames;
	start_frame_scale = starting_frame;
	is_rotating = true;
	scale_inc = scale / frames;
}