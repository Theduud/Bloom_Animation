#include "canvas.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Pixel {
	Pixel(float x, float y) :x{ x }, y{ y }{};
	float x, y;
	void Draw(const Cairo::RefPtr<Cairo::Context>& cr) { cr->rectangle(x, y, 1, 1); cr->fill(); }
};

struct Line {
	Line(std::vector<Pixel> pixels, float xc, float yc) :pixels{ pixels }, xc{ xc }, yc{ yc } {};
	std::vector<Pixel> pixels;
	float xc, yc;
};

std::vector<Line> lines;
bool is_rotating = false;
float rotate_inc = 0.f;
int start_frame;
int total_frames;
float current_rotation;

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
void Draw_Brick_Pattern(const Cairo::RefPtr<Cairo::Context>& cr, int frameCount) {
	for (Line line: lines) {
		cr->save();
		if (is_rotating) {
			if (frameCount - start_frame > total_frames) {
				is_rotating = false;
			}
			else {
				current_rotation = rotate_inc * (frameCount - start_frame);
			}
		}
		cr->translate(line.xc, line.yc);
		cr->rotate(current_rotation);
		cr->translate(-line.xc, -line.yc);
		for (Pixel pixel : line.pixels) {
			pixel.Draw(cr);
		}
		cr->restore();
	}
}

void Brick_Pattern_Rotate(int frames, int starting_frame, float angle) {
	total_frames = frames;
	start_frame = starting_frame;
	is_rotating = true;
	rotate_inc = angle / frames;
}