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
vector<int> vec_frames;

void readFile()
{
    ifstream iFile;
    string strFileName = working_dir + string("\\resource\\beat_frames.txt");
    int frame;
    iFile.open(strFileName.c_str());
    while (iFile >> frame) 
    {
        vec_frames.push_back(frame);
    }
    iFile.close();
}

struct RGB {
    RGB(float r, float g, float b) : r{ r }, g{ g }, b{ b }{};
    float r, g, b;
};

void Clear(cairo_t* cr) {
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);
}

void SetRGB(cairo_t* cr, RGB color) {
    cairo_set_source_rgb(cr, color.r, color.g, color.b);
}

void Draw_Color_Pallete(cairo_t* cr, float height) {
    auto x = 0.f;
    auto width = WIDTH / colors_rgb.size();
    for (auto color : colors_rgb) {
        SetRGB(cr, color);
        cairo_rectangle(cr, x, 0, width, height);
        cairo_fill(cr);
        x += width;
    }
}

vector<int> Get_Beat_Frames(float threshold) {
    double sample_rate = audioFile.getSampleRate();
    double max_sample{ 0 };
    vector<int> frames;
    int channel = 0;
    int numSamples = audioFile.getNumSamplesPerChannel();
    int last_frame{ 0 };
    for (int i = 0; i < numSamples; i++) {
        if (audioFile.samples[channel][i] > max_sample) {
            max_sample = audioFile.samples[channel][i];
        }
    }

    for (int i = 0; i < numSamples; i++)
    {
        double currentSample = audioFile.samples[channel][i];
        if (currentSample >= max_sample * threshold) {
            int frame = i / (sample_rate * (1.f / frames_per_second));
            if ((find(frames.begin(), frames.end(), frame) == frames.end() || frames.empty()) && frame > 0 && frame > last_frame + 1) {
                frames.push_back(frame);
                last_frame = frame;
            }
        }
    }
    double finalSample = audioFile.samples[channel][numSamples - 1];
    int frame = (numSamples - 1) / (sample_rate * (1.f / frames_per_second));
    frames.push_back(frame);

    return frames;
}

bool Write_Beats_to_File(vector<int> v) {
    ofstream myfile;
    myfile.open(working_dir + string("\\resource\\beat_frames.txt"));
    for (auto value : v) {
        myfile << value;
        myfile << "\n";
    }
    myfile.close();
    return true;
}
cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, WIDTH, HEIGHT);
cairo_t* cr = cairo_create(surface);
static gboolean savePNG()
{
    auto fileName = string("output\\") + to_string(frame_count) + string(".png");
    cairo_surface_write_to_png(surface, fileName.c_str());
    return true;
}
int next_frame{ 0 };
static bool draw() {
    frame_count++;
    Clear(cr);
    Draw_Color_Pallete(cr, 30);
    SetRGB(cr, colors_rgb[0]);

    Draw_Brick_Pattern(cr, frame_count);

    //double current_sample = Get_Current_Sample();

    savePNG();
    return true;
}

void Load_Audio_File(float threshold) {
    audioFile.load("C:\\Users\\Adam\\source\\repos\\Bloom_Animation\\Bloom_Animation\\resource\\audio\\bloom_10seconds.wav");
    Write_Beats_to_File(Get_Beat_Frames(threshold));
}

int main(int argc, char** argv) {
    Load_Audio_File(.9f);
    readFile();

    //auto frames = 100;
    auto frames = vec_frames.operator[](vec_frames.size()-1);
    auto colors_hex = { "f20089",  "e500a4", "db00b6", "d100d1", "bc00dd", "b100e8", "a100f2",
    "8900f2", "6930c3", "5e60ce", "5390d9", "4ea8de", "48bfe3", "56cfe1", "64dfdf",
    "72efdd", "80ffdb" };

    int r, g, b;
    for (auto color : colors_hex) {
        if (sscanf_s(color, "%02x%02x%02x", &r, &g, &b) == 3) {
            colors_rgb.push_back(RGB(r / 255.f, g / 255.f, b / 255.f));
        }
    }
    Initialize_Brick_Pattern(0, 0, 1, 100, 100, WIDTH, HEIGHT, 25);
    for (int i = 0; i < vec_frames.size() - 1; i++) {
        int frame_duration = (vec_frames.operator[](i + 1) - vec_frames.operator[](i)) / 4.f;
        if (frame_duration > 0) {
            if(i%2==0)
                Brick_Pattern_Rotate(10, vec_frames.operator[](i), M_PI / 2.f);
            else
                Brick_Pattern_Rotate(10, vec_frames.operator[](i), -M_PI / 2.f);
        }
        //Brick_Pattern_Scale(80, frame_count, 5);
    }
    //Brick_Pattern_Rotate(10, 1, M_PI / 2.f);
    //Brick_Pattern_Rotate(10, 20, -M_PI / 2.f);
    //Brick_Pattern_Rotate(10, 40, M_PI / 2.f);
    //Brick_Pattern_Rotate(10, 60, -M_PI / 2.f);
    //Brick_Pattern_Rotate(10, 80, -M_PI / 2.f);

    for (int i = 0; i < frames; i++) {
        draw();
        printf("%f\n", float(((i + 1.f) / frames)) * 100.f);
    }

    auto command = string("ffmpeg -y -framerate 60 -i ") + working_dir + string("\\output\\%d.png ") + working_dir + string("\\output\\output.mp4");
    auto result = system(command.c_str());
    for (int i = 1; i <= frames; i++) {
        auto fileName = working_dir + string("\\output\\") + to_string(i) + string(".png");
        remove(fileName.c_str());
    }

    return 0;
}