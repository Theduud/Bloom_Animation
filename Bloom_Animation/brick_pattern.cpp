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

struct Rotate_Sequence {
	Rotate_Sequence(int start_frame, int frame_duration, float rotate_inc) :
		start_frame{ start_frame }, frame_duration{ frame_duration }, rotate_inc{ rotate_inc }{};
	int start_frame, frame_duration;
	float rotate_inc;
};

struct Scale_Sequence{
	Scale_Sequence(int start_frame, int frame_duration) :start_frame{ start_frame }, frame_duration{ frame_duration }{};
	int start_frame, frame_duration;
};

std::vector<Line> lines;
std::vector<Rotate_Sequence> rotate_sequences;
float current_rotation{ 0.f };
float current_scale{ 1.f };

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
	for (int i = 0; i < rotate_sequences.size(); i++) {
		if (frameCount - rotate_sequences.operator[](i).start_frame > rotate_sequences.operator[](i).frame_duration) {
			rotate_sequences.erase(rotate_sequences.begin() + i);
		}
		else if(frameCount >= rotate_sequences.operator[](i).start_frame) {
			current_rotation += rotate_sequences.operator[](i).rotate_inc;
		}
	}
	//if (is_scaling) {
	//	if (frameCount - start_frame_scale > total_frames_scale) {
	//		is_rotating = false;
	//	}
	//	else {
	//		current_scale += scale_inc;
	//	}
	//}
	for (Line line : lines) {
		cairo_save(cr);
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

void Brick_Pattern_Rotate(int frames, int starting_frame, float angle_adjust) {
	auto total_frames_rotate = frames;
	auto start_frame_rotate = starting_frame;
	auto is_rotating = true;
	auto rotate_inc = angle_adjust / frames;
	rotate_sequences.push_back(Rotate_Sequence(start_frame_rotate, total_frames_rotate, rotate_inc));
}
void Brick_Pattern_Scale(int frames, int starting_frame, float scale) {
	auto total_frames_scale = frames;
	auto start_frame_scale = starting_frame;
	auto is_scaling = true;
	auto scale_inc = (scale - current_scale) / frames;
}