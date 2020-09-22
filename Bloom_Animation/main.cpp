#include "canvas.h"
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include "AudioFile.h"
#include <iostream>
#include <fstream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

float WIDTH{ 1280 };
float HEIGHT{ 720 };
bool write_image = true;
int max_image_count = 100;

struct RGB;
AudioFile<float> audioFile;
vector<RGB> colors_rgb;
char full[_MAX_PATH];
auto working_dir = _fullpath(full, ".\\", _MAX_PATH);
Cairo::RefPtr<Cairo::ImageSurface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, WIDTH, HEIGHT);

struct RGB {
    RGB(float r, float g, float b) : r{ r }, g{ g }, b{ b }{};
    float r, g, b;
};

void Clear(Cairo::RefPtr<Cairo::Context> const& cr) {
    cr->set_source_rgb(0, 0, 0);
    cr->paint();
}

void SetRGB(Cairo::RefPtr<Cairo::Context> const& cr, RGB color) {
    cr->set_source_rgb(color.r, color.g, color.b);
}

void Draw_Color_Pallete(Cairo::RefPtr<Cairo::Context> const& cr, float height) {
    auto x = 0.f;
    auto width = WIDTH / colors_rgb.size();
    for (auto color : colors_rgb) {
        SetRGB(cr, color);
        cr->rectangle(x, 0, width, height);
        cr->fill();
        x += width;
    }
}

double max_sample{ 0 };
double sample_rate{ 0 };
double length_in_seconds{ 0 };
int num_samples{ 0 };

double Get_Current_Sample() {
    int current_sample_index = frame_count * (sample_rate * (1. / frames_per_second));
    double current_sample = 0;
    if (current_sample_index < audioFile.samples[0].size())
        current_sample = audioFile.samples[0][current_sample_index];
    return current_sample;
}

vector<int> Get_Beat_Frames(float threshold) {
    vector<int> frames;
    int channel = 0;
    int numSamples = audioFile.getNumSamplesPerChannel();

    for (int i = 0; i < numSamples; i++)
    {
        double currentSample = audioFile.samples[channel][i];
        if (currentSample >= max_sample * threshold) {
            int frame = i / (sample_rate * (1. / frames_per_second));
            if (find(frames.begin(), frames.end(), frame) == frames.end() || frames.empty()) {
                frames.push_back(frame);
            }
        }
    }

    return frames;
}

bool Write_Beats_to_File(vector<int> v) {
    ofstream myfile;
    myfile.open(working_dir + string("beat_frames.txt"));
    for (auto value : v) {
        myfile << value;
        myfile << "\n";
    }
    myfile.close();
    return true;
}

bool CCanvas::on_draw(Cairo::RefPtr<Cairo::Context> const& cr)
{
    frame_count++;
    Clear(cr);
    Draw_Color_Pallete(cr, 30);
    SetRGB(cr, colors_rgb[0]);

    if (frame_count == 1) {
        Brick_Pattern_Rotate(10, 1, M_PI / 2);
    }
    Draw_Brick_Pattern(cr, frame_count);

    //double current_sample = Get_Current_Sample();

    if (write_image) {
        surface->write_to_png(working_dir + to_string(frame_count) + string(".png"));
    }

    return true;
}

int main(int argc, char** argv)
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.cairo.tut");

    Gtk::Window window;
    window.resize(WIDTH, HEIGHT);
    CCanvas area;
    window.add(area);
    area.show();
    //audioFile.load(working_dir + string("resource\\audio\\CantinaBand3.wav"));
    //audioFile.load(working_dir + string("resource\\audio\\bloom.wav"));

    //set colors
    auto colors_hex = { "f20089",  "e500a4", "db00b6", "d100d1", "bc00dd", "b100e8", "a100f2", 
        "8900f2", "6930c3", "5e60ce", "5390d9", "4ea8de", "48bfe3", "56cfe1", "64dfdf", 
        "72efdd", "80ffdb" };

    int r, g, b;
    for (auto color : colors_hex) {
        if (sscanf_s(color, "%02x%02x%02x", &r, &g, &b) == 3) {
            colors_rgb.push_back(RGB(r/255.f,g/255.f,b/255.f));
        }
    }
    Initialize_Brick_Pattern(0, 0, 1, 200, 200, WIDTH, HEIGHT, 50);

    //int channel = 0;
    //sample_rate = audioFile.getSampleRate();
    //length_in_seconds = audioFile.getLengthInSeconds();
    //num_samples = audioFile.getNumSamplesPerChannel();

    //for (int i = 0; i < num_samples; i++)
    //{
    //    double currentSample = audioFile.samples[channel][i];
    //    if (currentSample > max_sample) max_sample = currentSample;
    //}

    //
    //Write_Beats_to_File(Get_Beat_Frames(.8f));

    return app->run(window);
}
