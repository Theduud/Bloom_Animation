#include <gtkmm.h>
#include <gtk/gtkdrawingarea.h>

static int frame_count{ 0 };
const unsigned frames_per_second = 60;

void Initialize_Brick_Pattern(float x_init, float y_init, float line_width,
    float rect_width, float rect_height, float pattern_width, float pattern_height, float gap);
void Draw_Brick_Pattern(const Cairo::RefPtr<Cairo::Context>& cr, int frameCount);
void Brick_Pattern_Rotate(int frames, int start_frame, float angle);

class CCanvas : public Gtk::DrawingArea
{
public:
    CCanvas()
    {
        double interval = 1000 / frames_per_second;
        m_afConnection = Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this, &CCanvas::FrameInc), 0), interval);
    }
    virtual ~CCanvas() {
        m_afConnection.disconnect();
    };
protected:
    bool FrameInc(int c)
    {
        queue_draw();
        return true;
    }
    sigc::connection m_afConnection;
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

